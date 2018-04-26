#include "xinu.h"

void sendNAD(byte * ip_src, byte * mac_dest, byte * ip_dest, byte * target, uint8 iface) {
    uint8 flag =1;
    kprintf("sendNAD: sending nadvert\n");
    struct netpacket * packet = (struct netpacket *) getbuf(ipv6bufpool);
    memset((char *) packet, NULLCH, PACKLEN);
    
    uint8 totalOptLen = 0;
    
    if (flag)
        totalOptLen = 8;
    
    uint32 len = ETH_HDR_LEN + IPV6_HDR_LEN + NADSIZE + totalOptLen;
    
    fillEthernet(packet, mac_dest, iface);
    fillIPdatagram(packet, ip_src, ip_dest, NADSIZE + totalOptLen, IPV6_ICMP);
    uint8 nopt[1] = {1};

    struct base_header * pkt_ip = (struct base_header *) ((char *) packet + ETH_HDR_LEN);
    struct nadvert * advert = (struct nadvert *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN);

    uint32 pseudoSize = 0;
    byte src[IPV6_ASIZE]; 
    byte dest[IPV6_ASIZE];
    memcpy(src, pkt_ip->src, IPV6_ASIZE);
    memcpy(dest, pkt_ip->dest, IPV6_ASIZE);

    advert->type = NEIGHBA;
    advert->code = 0;
    advert->checksum = 0;
    //S and O set
    advert->mosreserved[0] = 96;
    memcpy((void *) advert->target, target, IPV6_ASIZE);
    
    if (flag)
        fillOptions((char *) advert->opt, nopt, 1, ifprime);

    pseudoSize = PSEUDOLEN + NADSIZE + totalOptLen;
    
    //set up pseudo header for checksum
    void * pseudo = (void *) getmem(pseudoSize);
    makePseudoHdr((struct pseudoHdr *) pseudo, src, dest, (void *) advert, pseudoSize - PSEUDOLEN);
    uint16 sum = checksumv6(pseudo, pseudoSize);
    advert->checksum = htons(sum);
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


bool8 nadvert_valid(struct base_header * ipdatagram) {
    struct nadvert * msg = (struct nadvert *) ((char *) ipdatagram + IPV6_HDR_LEN);
    if (ipdatagram->hop_limit != 255) 
        return FALSE;
    if (msg->code != 0)
        return FALSE;
    if (ipdatagram->payload_len < 24) 
        return FALSE;

    //TODO: figure out if there's options using packet size instead of hard
    //coding
    /*    kprintf("ckpt1\n");
    //this is in bytes
    int16 opt_len = ntohs(ipdatagram->payload_len) - sizeof(struct nadvert);
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

    kprintf("ckpt2\n");
    //reset back to actual length
    opt_len = ntohs(ipdatagram->payload_len) - sizeof(struct nadvert);
    */  byte buf[IPV6_ASIZE];
    memset(buf, NULLCH, IPV6_ASIZE);

    //if unspecified ip sourcce
    if (match(ipdatagram->src, buf, IPV6_ASIZE)) {
        //make sure there's no source link layer option
        //      if (src_link_layer_opt)
        //        return FALSE;

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

    //target address is not multicast
    //if ip destination address is multicast, the solicited flag is 0
}

byte * getmac_ad(struct nadvert * advert) {
    struct icmpopt * opt = (struct icmpopt *) advert->opt;
    //make sure the option is 01
    if (opt->type != 1) {
        return NULL;
    }

    //skip over opt type and optlen, 2 bytes
    return opt->payload;
}

void nadvert_handler(struct netpacket * pkt){
    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    struct nadvert * msg = (struct nadvert *) ((char *) ipdatagram + IPV6_HDR_LEN);

    struct NDCacheEntry * entry = lookupNDEntry(ipdatagram->src);
    //assumes the packet has been validated by the time it gets here 
    if (entry == NULL) {

        kprintf("nadvert_handler: no entry exists, silently dropping\n");
        return;
/*
        entry = getAvailNDEntry();

        if (entry == NULL) {
            kprintf("nadvert_handler: ND table is full, doing nothing\n");
            return;
        }
        //add ipaddr to the entry
        memcpy(entry->ipAddr, ipdatagram->src, IPV6_ASIZE);
*/    
    }
    else {
        //entry does exist
        kprintf("nadvert_handler: solicited advert received\n");
        kprintf("nadvert_handler: entry added to neighbor cache\n");
    }

    entry->ttl = MAXNDTTL;
    entry->state = NDREACHABLE;

    //update new mac address 
    byte * mac = getmac_ad(msg);
    if (mac == NULL) {
        //kprintf("nadvert_handler: no link layer address option found!\n");
        return;
    }
    memcpy(entry->macAddr, mac, ETH_ADDR_LEN);
}
