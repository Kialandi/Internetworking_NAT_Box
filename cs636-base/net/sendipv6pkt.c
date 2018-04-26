#include <xinu.h>

uint8   fill_option_prefix(void * pkt);
uint8   fill_option_MTU(void * pkt) ;
uint8   fill_option_one(void * pkt, uint8 iface) ;

void    fill_dest_ip_all_routers(byte* dest) ;
void    fill_dest_mac_all_router(byte* dest) ;
void    fillEthernet(struct netpacket *, byte *, uint8);
void    fillIPdatagram(struct netpacket *, byte *, byte *, uint16, byte);
void 	fillICMP(struct netpacket * pkt, byte type, uint8* option_types, uint8 option_types_length, uint8 iface);
bpid32  ipv6bufpool; //pool of buffers for IPV6

void makePseudoHdr(struct pseudoHdr * pseudo, byte * src, byte * dest,
        void * pkt, uint32 pktLen) {

    //zero out the buffer
    memset(pseudo, NULLCH, PSEUDOLEN + pktLen);

    //copy the src and destination
    memcpy(pseudo->src, src, IPV6_ASIZE);
    memcpy(pseudo->dest, dest, IPV6_ASIZE);

    pseudo->len = htonl(pktLen);
    pseudo->next_header = IPV6_ICMP;

    //copy the payload
    memcpy(pseudo->icmppayload, pkt, pktLen);
}

status sendipv6pkt(byte type, byte * link_dest, byte * ip_dest, uint8 iface) {
    struct netpacket * packet = (struct netpacket *) getbuf(ipv6bufpool);
    memset((char *) packet, NULLCH, PACKLEN);
    uint32 len;
    uint16 totalOptLen;

    switch (type) {
        case ROUTERS:
            kprintf("Sending Router Solicitation...\n");

            //entire packet length
            len = ETH_HDR_LEN + IPV6_HDR_LEN + RSOLSIZE;
            //unspecified address
            byte src_ip[IPV6_ASIZE];
            memset(src_ip, NULLCH, IPV6_ASIZE);

            fillEthernet(packet, link_dest, iface);
            fillIPdatagram(packet, src_ip, allrIPmulti, RSOLSIZE, IPV6_ICMP);
            fillICMP(packet, ROUTERS, NULL, 0, iface);

            if (!rsolicit_valid((struct base_header *) ((char *) packet + ETH_HDR_LEN))) {
                kprintf("Invalid router solicitation constructed, aborting!\n");
                freebuf((char *) packet);
                return 0;
            }
            break;

        case ROUTERA:
            if (host) {
                kprintf("Hosts cannot send router advertisements!\n");
                freebuf((char *) packet);
                return 0;
            }
            kprintf("Sending Router Advertisement...\n");
            totalOptLen = 48;

            uint8 option_types[3] = {1, 5, 3};

            //entire packet length
            len = ETH_HDR_LEN + IPV6_HDR_LEN + RADVERTSIZE + totalOptLen;

            fillEthernet(packet, link_dest, iface);
            fillIPdatagram(packet, link_local, allnIPmulti, RADVERTSIZE + totalOptLen, IPV6_ICMP);
            fillICMP(packet, ROUTERA, option_types, 3, iface);

            if (!radvert_valid((struct base_header *) ((char *) packet + ETH_HDR_LEN))) {
                kprintf("Invalid router advert constructed, aborting!\n");
                freebuf((char *) packet);
                return 0;
            }
            break;

        case NEIGHBS:
            kprintf("sendipv6pkt: Sending neighbor solicit...\n");
            totalOptLen = 8;
            len = ETH_HDR_LEN + IPV6_HDR_LEN + NSOLSIZE + totalOptLen;
            fillEthernet(packet, link_dest, iface);
            //caller has to pass in which dest to use. probably lookup routing
            //table first
            //fillIPdatagram(packet, link_local, ip_dest, NSOLSIZE + totalOptLen, IPV6_ICMP);
            fillIPdatagram(packet, ipv6_addr, ip_dest, NSOLSIZE + totalOptLen, IPV6_ICMP);
            uint8 nopt[1] = {1};
            fillICMP(packet, NEIGHBS, nopt,1, iface);
            
            if (!nsolicit_valid((struct base_header *) ((char *) packet + ETH_HDR_LEN))) {
                kprintf("Invalid neighbor solicit constructed, aborting!\n");
                freebuf((char *) packet);
                return 0;
            }
            
            break;

            //TODO: this is working right now for
        case NEIGHBA://should add a case for unsolicited
            kprintf("sendipv6pkt: Sending neighbor advert...\n");
            //totalOptLen = 8;
            totalOptLen = 0;
            len = ETH_HDR_LEN + IPV6_HDR_LEN + NADSIZE + totalOptLen;
            fillEthernet(packet, link_dest, iface);
            fillIPdatagram(packet, ipv6_addr, ip_dest, NADSIZE + totalOptLen, IPV6_ICMP);
            //uint8 nadops[1] = {1};
            //fillICMPNAD(packet, NEIGHBA, target);
            
            if (!nadvert_valid((struct base_header *) ((char *) packet + ETH_HDR_LEN))) {
                kprintf("Invalid neighbor advert constructed, aborting!\n");
                freebuf((char *) packet);
                return 0;
            }
            break;

        case ECHOREQ:
            kprintf("Sending echo request...\n");

            //entire packet length
            len = ETH_HDR_LEN + IPV6_HDR_LEN + ECHOREQSIZE;

            //TODO: add fwding table and nat table
            //for now, send to default router
            fillEthernet(packet, router_mac_addr, iface);
            
            fillIPdatagram(packet, ipv6_addr, ip_dest, ECHOREQSIZE, IPV6_ICMP);
            //fillIPdatagram(packet, link_local, ip_dest, ECHOREQSIZE, IPV6_ICMP);
            fillICMP(packet, ECHOREQ, NULL, 0, iface);
            break;
    }
    //print6(packet);
    //fire off packet
    if( write(ETHER0, (char *) packet, len) == SYSERR) {
        kprintf("THE WORLD IS BURNING\n");
        kill(getpid());
    }

    if (type == NEIGHBA || type == ECHOREQ) {
       kprintf("OUTGOING PKT PRINTING\n");
       print6(packet);
       kprintf("OUTGOING PKT DONE PRINTING\n");
    }
    if (type == NEIGHBS) {
       kprintf("OUTGOING PKT PRINTING\n");
       print6(packet);
       kprintf("OUTGOING PKT DONE PRINTING\n");
    }
    freebuf((char *) packet);
    return 1;
}

void fillOptions(void * pkt, uint8* option_types, uint8 option_types_length, uint8 iface) {
    uint8 i = 0;
    uint8 length = 0;
    while (i < option_types_length) {
        switch(option_types[i]) {
            case 1:
                length = fill_option_one(pkt, iface);
                //kprintf("option_one:\n");
                //payload_hexdump(pkt, length);
                break;
            case 5:
                length = fill_option_MTU(pkt);
                //kprintf("option_MTU:\n");
                //payload_hexdump(pkt, length);
                break;
            case 3:
                length = fill_option_prefix(pkt);
                //kprintf("option_prefix:\n");
                //payload_hexdump(pkt, length);
                break;
            default:
                break;

        }
        pkt = pkt + length;
        i++;
    }
}

//prefix information options (look at page 20 of RFC 4861)
uint8 fill_option_prefix(void * pkt) {
    memcpy(pkt, &option_prefix_default, sizeof(option_prefix));
    //change to the appropriate values based on interface
    return sizeof(struct option_prefix);
}

//MTU option
uint8 fill_option_MTU(void * pkt) {
    struct option_MTU * temp = pkt;
    memset(temp, NULLCH, sizeof(struct option_MTU));
    temp->type = 0x05;
    temp->length = 0x01;
    temp->payload = htons(MTU);
    return sizeof(struct option_MTU);
}

//source link-layer address
uint8 fill_option_one(void * pkt, uint8 iface) {
    struct option_one* temp = pkt;
    temp->type = 0x01;
    temp->length = 0x01;
    memcpy((char*)temp + 2, if_tab[iface].if_macucast, ETH_ADDR_LEN);
    return sizeof(struct option_one); // equal to length(1) * 8 bytes
}

void fillEthernet(struct netpacket * pkt, byte* dest, uint8 iface) {
    memcpy(pkt->net_dst, dest, ETH_ADDR_LEN);
    memcpy(pkt->net_src, if_tab[iface].if_macucast, ETH_ADDR_LEN);
    
    pkt->net_type = htons(ETH_IPv6);
}

void fillIPdatagram(struct netpacket * packet, byte* src_ip, byte* dest_ip, uint16 length, byte next_header) {
    struct base_header * pkt = (struct base_header *) ((char *) packet + ETH_HDR_LEN);

    pkt->info[0] = 0x60;
    pkt->info[1] = 0x00;
    pkt->info[2] = 0x00;
    pkt->info[3] = 0x00;

    pkt->payload_len = htons(length);
    pkt->next_header = next_header;
    pkt->hop_limit = 255;

    memcpy(pkt->src, src_ip, IPV6_ASIZE);
    memcpy(pkt->dest, dest_ip, IPV6_ASIZE);
}

void fillICMP(struct netpacket * packet, byte type, uint8* option_types, uint8 option_types_length, uint8 iface) {
    struct base_header * pkt_ip = (struct base_header *) ((char *) packet + ETH_HDR_LEN);
    struct icmpv6general * pkt_icmp = (struct icmpv6general *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN);
    pkt_icmp->type = type;

    uint32 pseudoSize = 0;
    byte src[IPV6_ASIZE];
    byte dest[IPV6_ASIZE];
    memcpy(src, pkt_ip->src, IPV6_ASIZE);
    memcpy(dest, pkt_ip->dest, IPV6_ASIZE);
    uint16 totalOptLen;

    switch (type) {
        case ROUTERS: ;//semicolon to get rid of pesky compiler error for labels
            pseudoSize = PSEUDOLEN + RSOLSIZE;
            break;

        case ROUTERA: ;
            totalOptLen = 48;

            //copy from default router
            memcpy(pkt_icmp, &radvert_from_router, RADVERTSIZE);
            //zero out the checksum after copying
            pkt_icmp->checksum = 0;
            //handle options
            fillOptions((char*) pkt_icmp + RADVERTSIZE,  option_types, totalOptLen, iface);
            pseudoSize = PSEUDOLEN + RADVERTSIZE + totalOptLen;
            break;

        case NEIGHBS: ;
            struct nsolicit * pkt_icmp_nsol = (struct nsolicit *) pkt_icmp;
            totalOptLen = 8;
            pkt_icmp_nsol->type = NEIGHBS;
            pkt_icmp_nsol->code = 0;
            pkt_icmp_nsol->checksum = 0;
            //target is any address associated with recipient interface
            
            memcpy((void *) pkt_icmp_nsol->target, router_link_local, IPV6_ASIZE);
            fillOptions((char *) pkt_icmp_nsol->opt, option_types, option_types_length, ifprime);
            pseudoSize = PSEUDOLEN + NSOLSIZE + totalOptLen;
            break;

        case NEIGHBA: ;
                      pkt_icmp->type = NEIGHBA;
                      pkt_icmp->code = 0;
                      pkt_icmp->checksum = 0;
                      struct nadvert * nadvertptr = (struct nadvert *) pkt_icmp;
                      //set the solicited and overwrite bits
                      //nadvertptr->mosreserved[0] = 96;
                      nadvertptr->mosreserved[0] = 96;

                      //if solicited advertisement, have to send address that was in the
                      //original solicitation msg
                      memcpy((void *) nadvertptr->target, ipv6_addr, IPV6_ASIZE);
                      //memcpy((void *) nadvertptr->target, link_local, IPV6_ASIZE);
                      //fillOptions((char*) pkt_icmp + NADSIZE,  option_types, option_types_length);
                      pseudoSize = PSEUDOLEN + NADSIZE;// + totalOptLen;
                      break;


        case ECHOREQ: ;
                      struct icmpv6echoreq * echo_pkt = (struct icmpv6echoreq *) pkt_icmp;
                      uint16 x = 15;
                      echo_pkt->identifier = htons(x);
                      echo_pkt->seqNumber = htons(x);
                      pseudoSize = PSEUDOLEN + ECHOREQSIZE;
                      break;

        default:
                      kprintf("fillICMP: invalid type\n");
                      return;
                      break;
    }
    //create pseudo header and compute checksum
    void * pseudo = (void *) getmem(pseudoSize);
    makePseudoHdr((struct pseudoHdr *) pseudo, src, dest, (void *) pkt_icmp, pseudoSize - PSEUDOLEN);
    uint16 sum = checksumv6(pseudo, pseudoSize);
    pkt_icmp->checksum = htons(sum);
    freemem(pseudo, pseudoSize);
}

