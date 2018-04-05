#include <xinu.h>

uint8 fill_option_prefix(void * pkt);
uint8 fill_option_MTU(void * pkt) ;
uint8 fill_option_one(void * pkt) ;
void fillOptions(void * pkt, uint8* option_types, uint8 option_types_length);
void create_packet(byte type, byte * etherdest, byte * ipdest, uint32 optlen, unsigned char * optarr);
void fill_dest_ip_all_routers(byte* dest) ;
void fill_dest_mac_all_router(byte* dest) ;
void    fillEthernet(struct netpacket *, byte *);
void    fillIPdatagram(struct base_header *, byte *, byte *, uint16);
void 	fillICMP(void *pkt, byte type, uint8* option_types, uint8 option_types_length);
bpid32  ipv6bufpool; //pool of buffers for IPV6

void makePseudoHdr(struct pseudoHdr * pseudo, byte * src, byte * dest,
        void * pkt, int32 pktLen) {

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

/*
   void    fillICMP(struct rsolicit * pkt) {
   pkt->type = ROUTERS;
//assumed code, checksum, and reserved were all set to 0 before coming in
void * pseudo = (void *) getmem(PSEUDOLEN);
makePseudoHdr((struct pseudoHdr *) pseudo, pkt);

uint16 sum = checksumv6(pseudo, PSEUDOLEN);
//kprintf("checksum: %d\n", sum);
pkt->checksum = htons(sum);
freemem(pseudo, PSEUDOLEN);
}
*/
void    fillICMP(void * pkt_icmp, byte type, uint8* option_types, uint8 option_types_length) {

    if (type == ROUTERS){

        struct rsolicit * pkt = pkt_icmp;
        pkt->type = type;
        //assumed code, checksum, and reserved were all set to 0 before coming in
        void * pseudo = (void *) getmem(PSEUDOLEN + RSOLSIZE);

        byte src[IPV6_ASIZE];

        //zero the buffers
        memset(src, NULLCH, IPV6_ASIZE);

        makePseudoHdr((struct pseudoHdr *) pseudo, src, allrIPmulti, (void *) pkt, RSOLSIZE);

        uint16 sum = checksumv6(pseudo, PSEUDOLEN + RSOLSIZE);
        pkt->checksum = htons(sum);
        freemem(pseudo, PSEUDOLEN + RSOLSIZE);

    } else if (type == ROUTERA) {
        //kprintf("Filling router advert icmp\n");
        struct radvert * pkt = pkt_icmp;
        uint16 totalOptLen = 48;

        //copy from default router
        memcpy(pkt, &radvert_from_router, sizeof(struct radvert));

        //zero out the checksum after copying
        pkt->checksum = 0;

        /*
        pkt->type = type;
        pkt->code = 0;
        pkt->curhoplim = 255;
        pkt->m_o_res = 0;*/

        //might have to do stuff with code and curhoplimit
        fillOptions((char*) pkt + RADVERTSIZE,  option_types, option_types_length);

        //assumed code, checksum, and reserved were all set to 0 before coming in
        void * pseudo = (void *) getmem(PSEUDOLEN + RADVERTSIZE + totalOptLen);

        //TODO: maybe have to change link local to your brand new ip address?
        makePseudoHdr((struct pseudoHdr *) pseudo, link_local, allnIPmulti, (void *) pkt, RADVERTSIZE + totalOptLen);

        uint16 sum = checksumv6(pseudo, PSEUDOLEN + RADVERTSIZE + totalOptLen);
        pkt->checksum = htons(sum);
        freemem(pseudo, PSEUDOLEN + RADVERTSIZE + totalOptLen);

        //TODO: this is wrong to just copy, src and dest are all different
        //need to recompute checksum

        //memcpy(pkt, &radvert_from_router, sizeof(struct radvert));

        //pkt->type = type;
        //why is this + 16? shouldn't it be size of radvert?

        //use a different length + options
        //void * pseudo = (void *) getmem(PSEUDOLEN);
        //makePseudoHdr((struct pseudoHdr *) pseudo, pkt);

        //TODO: pseudo + options
        //uint16 sum = checksumv6(pseudo, PSEUDOLEN);
        //pkt->checksum = htons(sum);
        //freemem(pseudo, PSEUDOLEN);
    }else if(type == NEIGHBS) {

        struct nsolicit * pkt = pkt_icmp;
        pkt->type = type;
        pkt->checksum = 0;
        fill_option_one(&pkt->opt);
        //assumed code, checksum, and reserved were all set to 0 before coming in
        void * pseudo = (void *) getmem(PSEUDOLEN + NSOLSIZE);


        makePseudoHdr((struct pseudoHdr *) pseudo, link_local, if_tab[1].if_macbcast, (void *) pkt, NSOLSIZE);

        uint16 sum = checksumv6(pseudo, PSEUDOLEN + NSOLSIZE);
        pkt->checksum = htons(sum);
        freemem(pseudo, PSEUDOLEN + RSOLSIZE);

    }else if(type == NEIGHBA) {

        struct nadvert * pkt = pkt_icmp;
        pkt->type = type;
        fill_option_one(&pkt->opt);
        if (host){
            pkt->R = 0;
        }else if (!host){
            pkt->R = 1;
        }
        pkt->S = 1;
        pkt->O = 1;
        //assumed code, checksum, and reserved were all set to 0 before coming in
        void * pseudo = (void *) getmem(PSEUDOLEN + NSOLSIZE);
        makePseudoHdr((struct pseudoHdr *) pseudo, link_local, allnIPmulti, (void *) pkt, NSOLSIZE);
        uint16 sum = checksumv6(pseudo, PSEUDOLEN + NSOLSIZE);
        pkt->checksum = htons(sum);
        freemem(pseudo, PSEUDOLEN + RSOLSIZE);

    }

}

status  sendipv6pkt(byte type, byte * dest) {
    struct netpacket * packet;
    uint32 len;
    uint16 totalOptLen = 0;

    switch (type) {
        case ROUTERS:

            kprintf("Sending Router Solicitation...\n");

            //entire packet length
            len = ETH_HDR_LEN + IPV6_HDR_LEN + RSOLSIZE;
            packet = (struct netpacket *) getbuf(ipv6bufpool);
            memset((char *) packet, NULLCH, len);

            //fillEthernet(packet, allrMACmulti);
            fillEthernet(packet, dest);

            //unspecified address
            byte src_ip[IPV6_ASIZE];
            memset(src_ip, NULLCH, IPV6_ASIZE);

            fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN), src_ip, allrIPmulti, RSOLSIZE);

            fillICMP((struct rsolicit *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN), ROUTERS, NULL, 0);

            //fire off packet
            if( write(ETHER0, (char *) packet, len) == SYSERR) {
                kprintf("THE WORLD IS BURNING\n");
                kill(getpid());
            }

            if (!rsolicit_valid((struct base_header *) ((char *) packet + ETH_HDR_LEN))) {
                kprintf("Invalid router solicitation going out!\n");
            }

            //kprintf("OUTGOING PKT PRINTING\n");
            //print6(packet);
            //kprintf("OUTGOING PKT PRINTING\n");

            freebuf((char *) packet);
            return 1;

        case ROUTERA:
            if (host) {
                kprintf("Hosts cannot send router advertisements!\n");
                return 0;
            }
            totalOptLen = 48;
            kprintf("\nSending Router Advertisement...\n");

            //entire packet length
            len = ETH_HDR_LEN + IPV6_HDR_LEN + RADVERTSIZE + totalOptLen;
            packet = (struct netpacket *) getbuf(ipv6bufpool);
            memset((char*) packet, NULLCH, len);

            fillEthernet(packet, dest);

            fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN), link_local, allnIPmulti, RADVERTSIZE + totalOptLen);

            uint8 option_types[3] = {1, 5, 3};
            fillICMP((struct radvert *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN), ROUTERA, option_types, 3);

            if( write(ETHER0, (char *) packet, len) == SYSERR) {
                kprintf("THE WORLD IS BURNING\n");
                kill(getpid());
            }
/*
            kprintf("OUTGOING PKT PRINTING\n");
            print6(packet);
            kprintf("OUTGOING PKT PRINTING\n");
  */
            if (!radvert_valid((struct base_header *) ((char *) packet + ETH_HDR_LEN))) {
                kprintf("Invalid router advert going out!\n");
            }

            freebuf((char *) packet);
            return 1;

        case NEIGHBS:
            totalOptLen = 10;
            kprintf("\n sending neighbor Solicitation \n");

            len = ETH_HDR_LEN + IPV6_HDR_LEN + NSOLSIZE + totalOptLen;
            packet = (struct netpacket *) getbuf(ipv6bufpool);
            memset((char*) packet, NULLCH, len);

            fillEthernet(packet, dest);
            fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN), link_local, allnIPmulti, NSOLSIZE + totalOptLen);

            uint8 nopt[1] = {1};
            fillICMP((struct nsolicit *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN), NEIGHBS, nopt, 1);
            if( write(ETHER0, (char *) packet, len) == SYSERR) {
                kprintf("THE WORLD IS BURNING\n");
                kill(getpid());
            }
            /*
            kprintf("OUTGOING PKT PRINTING\n");
            print6(packet);
            kprintf("OUTGOING PKT PRINTING\n");
            */
           freebuf((char *) packet);
           return 1;
        case NEIGHBA:
            totalOptLen = 10;
            kprintf("\n sending neighbor Advertisement \n");

            len = ETH_HDR_LEN + IPV6_HDR_LEN + NADSIZE + totalOptLen;
            packet = (struct netpacket *) getbuf(ipv6bufpool);
            memset((char*) packet, NULLCH, len);

            fillEthernet(packet, dest);
            fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN), link_local, allnIPmulti, NADSIZE + totalOptLen);
            uint8 nadops[1] = {1};
            fillICMP((struct nadvert  *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN), NEIGHBA, nadops, 1);
            if( write(ETHER0, (char *) packet, len) == SYSERR) {
                kprintf("THE WORLD IS BURNING\n");
                kill(getpid());
            }
            /*
            kprintf("OUTGOING PKT PRINTING\n");
            print6(packet);
            kprintf("OUTGOING PKT PRINTING\n");
            */
            freebuf((char *) packet);
            return 1;


    }
    //reaching here is bad
    return 0;
}

void fillOptions(void * pkt, uint8* option_types, uint8 option_types_length) {
    uint8 i = 0;
    uint8 length = 0;
    while (i < option_types_length) {
        switch(option_types[i]) {
            case 1:
                length = fill_option_one(pkt);
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
/*
void create_packet(byte type, byte * etherdest,byte * ipdest, uint32 optlen, uint8 options){
    struct netpacket * packet;
    uint16 totalOptLen = optlen;
    uint16 tsize = 0;
    switch(type){
        case ROUTERS:
            tsize = RSOLSIZE;
            break;
        case ROUTERA:
            tsize = RADVERTSIZE;
            break;
        case NEIGHBA:
            tsize = NADSIZE;
            break;
        case NEIGHBS:
            tsize = NSOLSIZE;
            break;
        default:
            break;
    }
    kprintf("\n sending neighbor Solicitation \n");
    uint32 len = ETH_HDR_LEN + IPV6_HDR_LEN + tsize + totalOptLen;
    packet = (struct netpacket *) getbuf(ipv6bufpool);
    memset((char*) packet, NULLCH, len);

    fillEthernet(packet, etherdest);
    fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN), link_local, ipdest, tsize  + totalOptLen);

    uint8 nopt[optlen];
    memcpy(nopt, optarr, optlen);
    fillICMP((struct nsolicit *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN), type, options, optlen);
    if( write(ETHER0, (char *) packet, len) == SYSERR) {
        kprintf("THE WORLD IS BURNING\n");
        kill(getpid());
    }

    kprintf("OUTGOING PKT PRINTING\n");
    print6(packet);
    kprintf("OUTGOING PKT PRINTING\n");
    freebuf((char *) packet);
}
*/
uint8 fill_option_prefix(void * pkt) {
    memcpy(pkt, &option_prefix_default, sizeof(option_prefix));
    //change to the appropriate values based on interface
    return sizeof(struct option_prefix);
}

uint8 fill_option_MTU(void * pkt) {
    struct option_MTU * temp = pkt;
    memset(temp, NULLCH, sizeof(struct option_MTU));
    temp->type = 0x05;
    temp->length = 0x01;
    temp->payload = htons(MTU);
    return sizeof(struct option_MTU);
}

uint8 fill_option_one(void * pkt) {
    struct option_one* temp = pkt;
    temp->type = 0x01;
    temp->length = 0x01;
    memcpy((char*)temp + 2, if_tab[ifprime].if_macucast, ETH_ADDR_LEN);
    return sizeof(struct option_one); // equal to length(1) * 8 bytes
}

void    fillEthernet(struct netpacket * pkt, byte* dest) {
    memcpy(pkt->net_dst, dest, ETH_ADDR_LEN);
    memcpy(pkt->net_src, if_tab[ifprime].if_macucast, ETH_ADDR_LEN);
    pkt->net_type = htons(ETH_IPv6);
}

void    fillIPdatagram(struct base_header * pkt, byte* src_ip, byte* dest_ip, uint16 length) {
    pkt->info[0] = 0x60;
    pkt->info[1] = 0x00;
    pkt->info[2] = 0x00;
    pkt->info[3] = 0x00;

    pkt->payload_len = htons(length);
    pkt->next_header = IPV6_ICMP;
    pkt->hop_limit = 255;

    memcpy(pkt->src, src_ip, IPV6_ASIZE);
    memcpy(pkt->dest, dest_ip, IPV6_ASIZE);
}
