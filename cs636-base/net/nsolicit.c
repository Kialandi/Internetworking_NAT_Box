#include "xinu.h"

//third step in handshake
//only gets called if you were solicited in the first place
void sendNSOL(byte * ip_src, byte * mac_dest, byte * ip_dest, byte * target) {
    struct netpacket * packet = (struct netpacket *) getbuf(ipv6bufpool);
    memset((char *) packet, NULLCH, PACKLEN);

    uint16 totalOptLen = 8;
    uint32 len = ETH_HDR_LEN + IPV6_HDR_LEN + NSOLSIZE + totalOptLen;
    
    fillEthernet(packet, mac_dest);
    fillIPdatagram(packet, ip_src, ip_dest, NSOLSIZE + totalOptLen, IPV6_ICMP);
    uint8 nopt[1] = {1};

    struct base_header * pkt_ip = (struct base_header *) ((char *) packet + ETH_HDR_LEN);
    struct nsolicit * solicit = (struct nsolicit *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN);

    uint32 pseudoSize = 0;
    byte src[IPV6_ASIZE]; 
    byte dest[IPV6_ASIZE];
    memcpy(src, pkt_ip->src, IPV6_ASIZE);
    memcpy(dest, pkt_ip->dest, IPV6_ASIZE);

    solicit->type = NEIGHBS;
    solicit->code = 0;
    solicit->checksum = 0;
    
    memcpy((void *) solicit->target, target, IPV6_ASIZE);
    
    fillOptions((char *) solicit->opt, nopt, 1);
    pseudoSize = PSEUDOLEN + NSOLSIZE + totalOptLen;
    
    //set up pseudo header for checksum
    void * pseudo = (void *) getmem(pseudoSize);
    makePseudoHdr((struct pseudoHdr *) pseudo, src, dest, (void *) solicit, pseudoSize - PSEUDOLEN);
    uint16 sum = checksumv6(pseudo, pseudoSize);
    solicit->checksum = htons(sum);
    freemem(pseudo, pseudoSize);

    //fire off packet
    if( write(ETHER0, (char *) packet, len) == SYSERR) {
        kprintf("THE WORLD IS BURNING\n");
        kill(getpid());
    }   

    kprintf("OUTGOING PKT PRINTING\n");
    print6(packet);
    kprintf("OUTGOING PKT DONE PRINTING\n");

    freebuf((char *) packet);
}

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
    uint8 exists = 0;

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
        exists = 1;
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

    //TODO: check if unspecified address, if unspecified, send to all nodes
    //multcast    

    //TODO: i'm not sure which packet is right but they're still all not working
    
    sendNAD(msg->target, pkt->net_src, ipdatagram->src, msg->target);
    //sendNAD(link_local, pkt->net_src, ipdatagram->src, msg->target);

    //if this is a new pairing, have to do 4 waay handshake
    if (!exists) {
        //give it a chance to process before sending sol
        sleep(3);
        //target is now the the ip address of what first solicited us
        sendNSOL(ipdatagram->dest, pkt->net_src, ipdatagram->src, ipdatagram->src);
        kprintf("should get advert back\n");
    }
}


