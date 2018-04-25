#include <xinu.h>
#include <stdio.h>

shellcmd sendnsolicit(int nargs, char * args[]) {
    kprintf("\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendnsolicit\n");
        return 0;
    }
/*
        sendipv6pkt(NEIGHBS, allnMACmulti, NULL);
        sleep(1);
        sendipv6pkt(NEIGHBS, allrMACmulti, NULL);
*/

        sendipv6pkt(NEIGHBS, router_snm, NULL);

        /*
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
