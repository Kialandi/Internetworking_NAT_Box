#include <xinu.h>
#include <stdio.h>

shellcmd sendradvert(int nargs, char * args[]) {
    printf("Sending radvert...\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendradvert\n");
        return 0;
    }
    
    if (!host)
        sendipv6pkt(ROUTERA, if_tab[ifprime].if_macbcast);
    else 
        kprintf("Not a NAT box, can't send RADVERTS\n");
    
    return 1;
}
