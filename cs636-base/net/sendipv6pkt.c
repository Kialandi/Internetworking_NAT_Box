#include <xinu.h>

//void printPacket(struct netpacket * );
//int32 charToHex(byte *, char *);

void    fillEthernet(struct netpacket * pkt) {
    pkt->net_dst[0] = 0x33;
    pkt->net_dst[1] = 0x33;
    pkt->net_dst[2] = 0x00;
    pkt->net_dst[3] = 0x00;
    pkt->net_dst[4] = 0x00;
    pkt->net_dst[5] = 0x02;

    memcpy(&pkt->net_src, if_tab[ifprime].if_macucast, ETH_ADDR_LEN);
    pkt->net_type = htons(ETH_IPv6);

    //TODO: Not sure if we need to do anything with these
    pkt->net_ethcrc = 0;
    pkt->net_iface = 0;
}

void    fillIPdatagram(struct base_header * pkt) {
    pkt->info[0] = 0x60;
    pkt->info[1] = 0x00;
    pkt->info[2] = 0x00;
    pkt->info[3] = 0x00;
    pkt->payload_len = htons(IPV6_HDR_LEN + ICMPSIZE);
    pkt->next_header = IPV6_ICMP;
    pkt->hop_limit = 255; //TODO: not sure whether to use htons
    memcpy(&pkt->src, link_local, IPV6_ASIZE);
    pkt->dest[0] = 0xff;
    pkt->dest[1] = 0x02;
    pkt->dest[15] = 0x02;
}

void    fillICMP() {
    return;
}

status  sendipv6pkt() {//byte[] destination, uint16 message) {
    struct netpacket * packet;
    if (ipv6bootstrap) {
        kprintf("bootstrapping for ipv6\n");
        packet = (struct netpacket *) getmem(PACKLEN);
        memset((char *) packet, NULLCH, PACKLEN);

        fillEthernet(packet);
        fillIPdatagram((struct base_header *) ((char *) packet + ETH_HDR_LEN));
        fillICMP((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN);
        
        write(ETHER0, (char *) packet, PACKLEN);
        ipv6bootstrap = 0;
    }
    else {
        //normal packet sending
        packet = (struct netpacket *) getmem(PACKLEN);
    }
    //printPacket(packet);
    return 0;
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

void printPacket(struct netpacket * packet) {
    printf("Printing packet...\n");
    printf("Dest: %02x:%02x:%02x:%02x:%02x:%02x\n",
            packet->net_dst[0], 0xff & packet->net_dst[1], 0xff & packet->net_dst[2],
            0xff & packet->net_dst[3], 0xff & packet->net_dst[4], 0xff & packet->net_dst[5]);
    printf("Source: %02x:%02x:%02x:%02x:%02x:%02x\n",
            packet->net_src[0], 0xff & packet->net_src[1], 0xff & packet->net_src[2],
            0xff & packet->net_src[3], 0xff & packet->net_src[4], 0xff & packet->net_src[5]);
    printf("Type: 0x%04x\n", packet->net_type);
    printf("Payload: %s\n", packet->net_payload);
}*/
