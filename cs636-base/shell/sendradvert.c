#include <xinu.h>
#include <stdio.h>

shellcmd sendradvert(int nargs, char * args[]) {
    printf("Sending radvert...\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendradvert\n");
        return 0;
    }
    
    if (!host) {
        //broadcast to both interfaces
        sendipv6pkt(ROUTERA, if_tab[1].if_macbcast);
        sendipv6pkt(ROUTERA, if_tab[2].if_macbcast);
    }
    else 
        kprintf("Not a NAT box, can't send RADVERTS\n");
    
    return 1;
}
