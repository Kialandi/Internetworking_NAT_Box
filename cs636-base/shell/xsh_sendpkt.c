#include <xinu.h>
#include <stdio.h>

void printPacket(struct netpacket * );
int32 charToHex(byte *, char *);

shellcmd xsh_sendpkt(int nargs, char * args[]) {
    printf("Sending packet...\n");
    if (nargs != 3) {
        //print usage
        printf("USAGE: sendpkt <destination> <message>\n");
        return 0;
    }
    
    //TODO: sanity check for input
    
    uint32 strlength, maclength = 0;
    char * string = args[2];
    //printf("msg: %s\n", string);
    
    while(*string != 0) {
        string++;
        strlength++;
        if (strlength > 1500) {
            printf("Payload too large! Only 1500 octets allowed!\n");
            return 1;
        }
    }
    
    string = args[1];
    
    while(*string != 0) {
        string++;
        maclength++;
        
    }
    if (maclength != 17) {
        printf("Invalid destination MAC length\n");
        return 1;
    }

    struct netpacket * packet = (struct netpacket *) getmem(PACKLEN);
    memset((char *) packet, NULLCH, PACKLEN);

    if (charToHex(packet->net_dst, args[1])) {
        printf("Unknown characters in destination MAC\n");
        return 1;
    }

    printf("ETH_ADDR_LEN: %d\n", ETH_ADDR_LEN);
    //memcpy(&packet->net_dst, args[1], ETH_ADDR_LEN);

    memcpy(&packet->net_src, if_tab[0].if_macucast, ETH_ADDR_LEN);
    packet->net_type = ETH_IP;
    //printf("ETH_IP: %x\n", ETH_IP);
    memcpy(&packet->net_payload, args[2], strlength);

    //printf("packet size = %d\n", PACKLEN);
    write(ETHER0, (char *) packet, PACKLEN);
    printPacket(packet);

    return 0;
}

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
}
