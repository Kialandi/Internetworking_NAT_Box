#include <xinu.h>
#include <stdio.h>

shellcmd sendnadvert(int nargs, char * args[]) {
    kprintf("\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendnadvert\n");
        return 0;
    }
    byte * buf = (byte *) getmem(ETH_ADDR_LEN);
    byte * buf2 = (byte *) getmem(IPV6_ASIZE);
    buf[0] = 0x33;
    buf[1] = 0x33;
    buf[2] = 0xFF;
    buf[3] = 0xDC;
    buf[4] = 0xCB;
    buf[5] = 0xC0;

    memset(buf2, NULLCH, IPV6_ASIZE);
    buf2[0] = 0xFF;
    buf2[1] = 0x02;
    buf2[8] = 0x00;
    buf2[9] = 0x00;
    buf2[10] = 0x00;
    buf2[11] = 0x01;
    buf2[12] = 0xFF;
    buf2[13] = 0xDC;
    buf2[14] = 0xCB;
    buf2[15] = 0xC0;

    sendipv6pkt(NEIGHBA, buf, buf2, NULL);
   freemem((char *) buf, ETH_ADDR_LEN);
   freemem((char *) buf2, IPV6_ASIZE);
    /*
    if (!host){ //nat goes to all routers
        //sendipv6pkt(NEIGHBA, allnMACmulti);
        sendipv6pkt(NEIGHBA, if_tab[1].if_macbcast, NULL);
        sendipv6pkt(NEIGHBA, if_tab[2].if_macbcast, NULL);

     }else //hosts go to your own bcast
        sendipv6pkt(NEIGHBA, allnMACmulti, allnIPmulti);
    */
     return 1;
}
