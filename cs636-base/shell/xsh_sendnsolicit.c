#include <xinu.h>
#include <stdio.h>

shellcmd sendnsolicit(int nargs, char * args[]) {
    kprintf("\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendnsolicit\n");
        return 0;
    }
    
    sendipv6pkt(NEIGHBS, router_link_local_mac, router_link_local);
       
    return 1;
}
