#include <xinu.h>
#include <stdio.h>

shellcmd sendnsolicit(int nargs, char * args[]) {
    kprintf("\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendnsolicit\n");
        return 0;
    }
  
    byte bufmac[ETH_ADDR_LEN];
    bufmac[0] = 0x33;
    bufmac[1] = 0x33;
    bufmac[2] = 0xfe;
    bufmac[3] = 0xdc;
    bufmac[4] = 0xcb;
    bufmac[5] = 0xc0;

    byte bufip[IPV6_ASIZE];
    memset(bufip, NULLCH, IPV6_ASIZE);
    bufip[0] = 0xfe;
    bufip[1] = 0x80;
    
    bufip[8] = 0x02;
    bufip[9] = 0x24;
    bufip[10] = 0xc4;
    bufip[11] = 0xff;
    
    bufip[12] = 0xfe;
    bufip[13] = 0xdc;
    bufip[14] = 0xcb;
    bufip[15] = 0xc0;

    sendipv6pkt(NEIGHBS, bufmac, bufip);
    /*
    sendipv6pkt(NEIGHBS, mac_snm, NULL);
        sendipv6pkt(NEIGHBS, if_tab[1].if_macbcast, NULL);
        sendipv6pkt(NEIGHBS, if_tab[2].if_macbcast, NULL);
*/

 /*
  *
    kprintf("NAT: Multicasting to both interfaces\n");
     //broadcast to both interfaces
    sendipv6pkt(NEIGHBS, if_tab[1].if_macbcast);
    sendipv6pkt(NEIGHBA, if_tab[2].if_macbcast);
    */
    return 1;
}
