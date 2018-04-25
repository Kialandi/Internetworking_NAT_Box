#include "xinu.h"

bool8 nsolicit_valid(struct base_header * ipdatagram) {
    struct nsolicit * msg = (struct nsolicit *) ((char *) ipdatagram + IPV6_HDR_LEN);
    if (ipdatagram->hop_limit != 255) 
        return FALSE;
    if (msg->code != 0)
        return FALSE;
    if (ipdatagram->payload_len < 24) 
        return FALSE;

    //this is in bytes
    int16 opt_len = ntohs(ipdatagram->payload_len) - sizeof(struct nsolicit);
    int16 src_link_layer_opt = 0;
    //check for option lengths
    if (opt_len > 0) {
        //options exist
        while (opt_len > 0) {
            struct icmpopt * opt = (struct icmpopt *) msg->opt;
            if (opt->type == 1) //flag for src link layer option
                src_link_layer_opt = 1;
            if (opt->length <= 0) 
                return FALSE;    
            //length is in units of 8 octets so multiply by 8 to get bytes
            opt_len -= opt->length * 8;
        }
    }

    //reset back to actual length
    opt_len = ntohs(ipdatagram->payload_len) - sizeof(struct nsolicit);
    byte buf[IPV6_ASIZE];
    memset(buf, NULLCH, IPV6_ASIZE);

    //if unspecified ip sourcce
    if (match(ipdatagram->src, buf, IPV6_ASIZE)) {
        //make sure there's no source link layer option
        if (src_link_layer_opt)
            return FALSE;

        //check ip dest is SNM address
        if (!match(ipdatagram->dest, snm_addr, IPV6_ASIZE))
            return FALSE;
    }

    //Note: inefficient but trying to not change too much code
    uint16 icmpSize = ntohs(ipdatagram->payload_len);

    //let C handle the casting to 32 bit
    uint32 pseudoSize = PSEUDOLEN + icmpSize;

    struct pseudoHdr * pseudo = (struct pseudoHdr *) getmem(pseudoSize);

    //reconstruct the complete pseudo header 
    memset(pseudo, NULLCH, pseudoSize);
    memcpy(pseudo->src, ipdatagram->src, IPV6_ASIZE);
    memcpy(pseudo->dest, ipdatagram->dest, IPV6_ASIZE);

    pseudo->len = htonl((uint32) icmpSize);
    pseudo->next_header = IPV6_ICMP;
    memcpy(pseudo->icmppayload, ipdatagram->payload, icmpSize);

    if (cksum_valid_1buf(pseudo, pseudoSize)) {
        freemem((char *) pseudo, pseudoSize);
        return FALSE;
    }

    freemem((char *) pseudo, pseudoSize);

    return TRUE;
    
    //TODO: target address cannot be a multcast address
    
}

byte * getmac_sol(struct nsolicit * solicit) {
    struct icmpopt * opt = (struct icmpopt *) solicit->opt;
    //make sure the option is 01
    if (opt->type != 1) {
        kprintf("getmac: Invalid option type\n");
        return NULL;
    }

    //skip over opt type and optlen, 2 bytes
    return opt->payload;
}

void nsolicit_handler(struct netpacket * pkt){
    struct base_header * ipdatagram = (struct base_header *) pkt->net_payload;
    struct nsolicit * msg = (struct nsolicit *) ((char *) ipdatagram + IPV6_HDR_LEN);

    struct NDCacheEntry * entry = lookupNDEntry(ipdatagram->src);
    //assumes the packet has been validated by the time it gets here 
    if (entry == NULL) {
        //entry doesnt exist, make a new one
        entry = getAvailNDEntry();

        if (entry == NULL) {
            kprintf("nsolicit_handler: ND table is full, doing nothing\n");
            return;
        }
        //add ipaddr to the entry
        memcpy(entry->ipAddr, ipdatagram->src, IPV6_ASIZE);
    }
    else {
        //entry does exist
        //kprintf("nsolicit_handler: entry exists, updating!\n");
    }

    entry->ttl = MAXNDTTL;
    entry->state = NDREACHABLE;
    
    //update new mac address 
    byte * mac = getmac_sol(msg);
    if (mac == NULL) {
        kprintf("nsolicit_handler: no link layer address option found!\n");
        return;
    }
    memcpy(entry->macAddr, mac, ETH_ADDR_LEN);
        
    //since we did a 2 way handshake, should probably look at nd table for dest
    sendipv6pkt(NEIGHBA, pkt->net_src, ipdatagram->src);
}
