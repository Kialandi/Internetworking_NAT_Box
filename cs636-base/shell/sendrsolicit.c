#include <xinu.h>
#include <stdio.h>

shellcmd sendrsolicit(int nargs, char * args[]) {
    printf("Sending rsolicit...\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendrsolicit\n");
        return 0;
    }
    
    sendipv6pkt(ROUTERS, if_tab[ifprime].if_macbcast);
    
    return 1;
}
