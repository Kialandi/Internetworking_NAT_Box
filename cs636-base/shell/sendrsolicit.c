#include <xinu.h>
#include <stdio.h>

shellcmd sendrsolicit(int nargs, char * args[]) {
    kprintf("\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendrsolicit\n");
        return 0;
    }

    if (!host) //nat goes to all routers
        sendipv6pkt(ROUTERS, allrMACmulti, NULL, ifprime);
    else //hosts go to your own bcast
        sendipv6pkt(ROUTERS, if_tab[ifprime].if_macbcast, NULL, ifprime);

    return 1;
}
