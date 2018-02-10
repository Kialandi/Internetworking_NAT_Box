#include <xinu.h>

void printPacket2(struct netpacket * );
//int32 charToHex(byte *, char *);

#define PSEUDOLEN  sizeof(pseudoHdr) + ICMPSIZE 

void    fillEthernet(struct netpacket *);
void    fillIPdatagram(struct base_header *);


void *  makePseudoHdr(struct rsolicit * pkt) {
    struct pseudoHdr * pseudo = (struct pseudoHdr *) getmem(PSEUDOLEN);
    memset((char *) pseudo, NULLCH, PSEUDOLEN);
    memcpy(&pseudo->src, link_local, IPV6_ASIZE);

    pseudo->dest[0] = 0xff;
    pseudo->dest[1] = 0x02;
    pseudo->dest[15] = 0x02;
    pseudo->len = htonl(ICMPSIZE); //not sure about this, ask dylan
    pseudo->next_header = IPV6_ICMP;
    kprintf("pseudo: len: %d\n", ntohl(pseudo->len));
    kprintf("pseudo: next: 0x%X\n", pseudo->next_header);
    void * ptr = (void *) ((char *) pseudo + sizeof(pseudoHdr));
    memcpy(ptr, pkt, ICMPSIZE);
    return pseudo;
}

void    fillICMP(struct rsolicit * pkt) {
    kprintf("Filling icmp hdr\n");
    pkt->type = ROUTERS;
    //assumed code, checksum, and reserved were all set to 0 before coming in
    //but settting it again just in case
    pkt->code = 0;
    pkt->checksum = 0;
    pkt->reserved = 0;
    void * pseudo = makePseudoHdr(pkt);
    uint16 sum = checksumv6(pseudo, PSEUDOLEN);
    pkt->checksum = htons(sum);
    freemem(pseudo, PSEUDOLEN);
}

status  sendipv6pkt() {//byte[] destination, uint16 message) {
    struct netpacket * packet;
    if (ipv6bootstrap) {
        kprintf("bootstrapping for ipv6\n");

        uint32 len = ETH_HDR_LEN + IPV6_HDR_LEN + ICMPSIZE;
        packet = (struct netpacket *) getbuf(PACKLEN);//change this 
        memset((char *) packet, NULLCH, PACKLEN);

        fillEthernet(packet);
        fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN));
        fillICMP((struct rsolicit *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN));

        if( write(ETHER0, (char *) packet, PACKLEN) == SYSERR) {
            kprintf("THE WORLD IS BURNING\n");
        }
        printPacket2(packet);
        ipv6bootstrap = 0;
        //TODO: figure out if packet buffer has to be freed
        //freebuf((char *) packet);
    }
    else {
        //normal packet sending
        packet = (struct netpacket *) getbuf(PACKLEN);
    }
    //printPacket(packet);
    return 0;
}

void    fillEthernet(struct netpacket * pkt) {
    kprintf("Filling ether hdr\n");
    
    pkt->net_dst[0] = 0x33;
    pkt->net_dst[1] = 0x33;
    pkt->net_dst[2] = 0x00;
    pkt->net_dst[3] = 0x00;
    pkt->net_dst[4] = 0x00;
    pkt->net_dst[5] = 0x02;

    memcpy(&pkt->net_src, if_tab[ifprime].if_macucast, ETH_ADDR_LEN);
    pkt->net_type = htons(ETH_IPv6);
    //pkt->net_ethcrc = 0;
    //pkt->net_iface = 0;

}

void    fillIPdatagram(struct base_header * pkt) {
    kprintf("Filling ip hdr\n");
    pkt->info[0] = 0x60;
    pkt->info[1] = 0x00;
    pkt->info[2] = 0x00;
    pkt->info[3] = 0x00;
    pkt->payload_len = htons(ICMPSIZE);
    pkt->next_header = IPV6_ICMP;
    pkt->hop_limit = 255;
    memcpy(&pkt->src, link_local, IPV6_ASIZE);
    pkt->dest[0] = 0xff;
    pkt->dest[1] = 0x02;
    pkt->dest[15] = 0x02;

}


/*
   int32 charToHex(byte * buf, char * string) {

   while (*string != 0) {
   printf("char: %c\n", *string);
   if (*string == ':') {
   string++;
   continue;
   }
   if (*string >= '0' && *string <= '9')
 *buf = (*string - '0') << 4;
 else if (*string >= 'A' && *string <= 'Z')
 *buf = (*string - 'A' + 10) << 4;
 else if (*string >= 'a' && *string <= 'z')
 *buf = (*string - 'a' + 10) << 4;
 else
 return 1;

 string++;

 if (*string >= '0' && *string <= '9')
 *buf |= *string - '0';
 else if (*string >= 'A' && *string <= 'Z')
 *buf |= *string - 'A' + 10;
 else if (*string >= 'a' && *string <= 'z')
 *buf |= *string - 'a' + 10;
 else
 return 1;

 string++;
 buf++;
 }
 return 0;
 }
 */
void printPacket2(struct netpacket * packet) {
    printf("Printing packet...\n");
    printf("Dest: %02x:%02x:%02x:%02x:%02x:%02x\n",
            packet->net_dst[0], 0xff & packet->net_dst[1], 0xff & packet->net_dst[2],
            0xff & packet->net_dst[3], 0xff & packet->net_dst[4], 0xff & packet->net_dst[5]);
    printf("Source: %02x:%02x:%02x:%02x:%02x:%02x\n",
            packet->net_src[0], 0xff & packet->net_src[1], 0xff & packet->net_src[2],
            0xff & packet->net_src[3], 0xff & packet->net_src[4], 0xff & packet->net_src[5]);
    printf("Type: 0x%04x\n", ntohs(packet->net_type));
    printf("Payload: %s\n", packet->net_payload);
}
