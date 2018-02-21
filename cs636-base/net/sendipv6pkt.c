#include <xinu.h>

void    fillEthernet(struct netpacket *);
void    fillIPdatagram(struct base_header *);

void    fillICMP(struct rsolicit * pkt) {
    pkt->type = ROUTERS;
    
    //assumed code, checksum, and reserved were all set to 0 before coming in
    
    //void * pseudo = makePseudoHdr(pkt);
    
    uint16 sum = checksumv6(pkt, PSEUDOLEN);
    pkt->checksum = htons(sum);
    //freemem(pseudo, PSEUDOLEN);
}

status  sendipv6pkt() {//byte[] destination, uint16 message) {
    struct netpacket * packet;
    if (ipv6bootstrap) {
        kprintf("bootstrapping for ipv6\n");

        uint32 len = ETH_HDR_LEN + IPV6_HDR_LEN + ICMPSIZE;
        packet = (struct netpacket *) getmem(len);//change this 
        memset((char *) packet, NULLCH, len);

        fillEthernet(packet);

        fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN));

        fillICMP((struct rsolicit *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN));


        if( write(ETHER0, (char *) packet, len) == SYSERR) {
            kprintf("THE WORLD IS BURNING\n");
            return 0;
        }
        ipv6bootstrap = 0;
        
        //TODO: figure out if packet buffer has to be freed
        //freebuf((char *) packet);
    }
    else {
        //normal packet sending
        packet = (struct netpacket *) getbuf(PACKLEN);
    }
    //printPacket(packet);
    return 1;
}

void    fillEthernet(struct netpacket * pkt) {
    pkt->net_dst[0] = 0x33;
    pkt->net_dst[1] = 0x33;
    pkt->net_dst[2] = 0x00;
    pkt->net_dst[3] = 0x00;
    pkt->net_dst[4] = 0x00;
    pkt->net_dst[5] = 0x02;

    memcpy(&pkt->net_src, if_tab[ifprime].if_macucast, ETH_ADDR_LEN);
    pkt->net_type = htons(ETH_IPv6);
}

void    fillIPdatagram(struct base_header * pkt) {
    pkt->info[0] = 0x60;
    pkt->info[1] = 0x00;
    pkt->info[2] = 0x00;
    pkt->info[3] = 0x00;
    
    pkt->payload_len = htons(ICMPSIZE);
    pkt->next_header = IPV6_ICMP;
    pkt->hop_limit = 255;
    
    //set IP dest addr, assumes src is non-specified (all 0s)
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

