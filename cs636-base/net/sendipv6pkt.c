#include <xinu.h>
uint8 fill_option_prefix(void * pkt);
uint8 fill_option_MTU(void * pkt) ;
uint8 fill_option_one(void * pkt) ;
void fillOptions(void * pkt, uint8* option_types, uint8 option_types_length);

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
        kprintf("Filling router advert icmp\n");
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
    }

}

status  sendipv6pkt(byte type, byte * dest) {//byte[] destination, uint16 message) {
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

            fillEthernet(packet, allrMACmulti);

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

            //kprintf("OUTGOING PKT PRINTING\n");
            //print6(packet);
            //kprintf("OUTGOING PKT PRINTING\n");

            freebuf((char *) packet);
            return 1;

        case ROUTERA:
            totalOptLen = 48;
            kprintf("\nSending Router Advertisement...\n");

            //entire packet length
            len = ETH_HDR_LEN + IPV6_HDR_LEN + RADVERTSIZE + totalOptLen;
            packet = (struct netpacket *) getbuf(ipv6bufpool);
            memset((char*) packet, NULLCH, len);

            fillEthernet(packet, allnMACmulti);

            fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN), link_local, allnIPmulti, RADVERTSIZE + totalOptLen);

            uint8 option_types[3] = {1, 5, 3};
            fillICMP((struct radvert *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN), ROUTERA, option_types, 3);

            if( write(ETHER0, (char *) packet, len) == SYSERR) {
                kprintf("THE WORLD IS BURNING\n");
                kill(getpid());
            }

            kprintf("OUTGOING PKT PRINTING\n");
            print6(packet);
            kprintf("OUTGOING PKT PRINTING\n");

            freebuf((char *) packet);
            return 1;
    }
    //TODO: figure out the correct status to send back, reaching here is bad
    return 0;
}

void fillOptions(void * pkt, uint8* option_types, uint8 option_types_length) {
    uint8 i = 0;
    uint8 length = 0;
    while (i < option_types_length) {
        switch(option_types[i]) {
            case 1:
                length = fill_option_one(pkt);
                kprintf("option_one:\n");
                payload_hexdump(pkt, length);
                break;
            case 5:
                length = fill_option_MTU(pkt);
                kprintf("option_MTU:\n");
                payload_hexdump(pkt, length);
                break;
            case 3:
                length = fill_option_prefix(pkt);
                kprintf("option_prefix:\n");
                payload_hexdump(pkt, length);
                break;
            default:
                break;

        }
        pkt = pkt + length;
        i++;
    }
}

uint8 fill_option_prefix(void * pkt) {
    memcpy(pkt, &option_prefix_default, sizeof(option_prefix));
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
    //kprintf("pkt src:\n");
    //print_ipv6_addr(&(pkt->src));
    //kprintf("pkt dest:\n");
    //print_ipv6_addr(&(pkt->dest));
    /*
    //set IP dest addr, assumes src is non-specified (all 0s)
    pkt->dest[0] = 0xff;
    pkt->dest[1] = 0x02;
    pkt->dest[15] = 0x02;
    */
}
