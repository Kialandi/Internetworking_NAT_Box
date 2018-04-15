#include <xinu.h>
#include <stdio.h>

shellcmd sendnadvert(int nargs, char * args[]) {
    kprintf("\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendradvert\n");
        return 0;
    }
     if (!host){ //nat goes to all routers
        //sendipv6pkt(NEIGHBA, allnMACmulti);
        sendipv6pkt(NEIGHBA, if_tab[1].if_macbcast);
        sendipv6pkt(NEIGHBA, if_tab[2].if_macbcast);

     }else //hosts go to your own bcast
        sendipv6pkt(NEIGHBA, mac_snm);
     //broadcast to both interfaces
    return 1;
}
