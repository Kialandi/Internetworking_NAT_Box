#include <xinu.h>
#include <stdio.h>

shellcmd sendnsolicit(int nargs, char * args[]) {
    kprintf("\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendradvert\n");
        return 0;
    }
        sendipv6pkt(NEIGHBS, mac_snm);
        sendipv6pkt(NEIGHBS, if_tab[1].if_macbcast);
        sendipv6pkt(NEIGHBS, if_tab[2].if_macbcast);


 /*
  *
    kprintf("NAT: Multicasting to both interfaces\n");
     //broadcast to both interfaces
    sendipv6pkt(NEIGHBS, if_tab[1].if_macbcast);
    sendipv6pkt(NEIGHBA, if_tab[2].if_macbcast);
    */
    return 1;
}
