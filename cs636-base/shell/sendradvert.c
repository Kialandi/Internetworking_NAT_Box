#include <xinu.h>
#include <stdio.h>

shellcmd sendradvert(int nargs, char * args[]) {
    printf("Sending radvert...\n");
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendradvert\n");
        return 0;
    }
    
    sendipv6pkt(ROUTERA, NULL);
}
