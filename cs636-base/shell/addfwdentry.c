#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

shellcmd addFwdEntry(int nargs, char * args[]) {
    printf("\nAdding forwarding entry...\n");
    if (nargs != 5) {
        //print usage
        printf("USAGE: addfwd <ipAddr> <prefixLen> <next hop> <iface>\n");
        return 0;
    }
 
    byte ipAddr[IPV6_ASIZE];
    uint8 len = atoi(args[2]);
    byte nexthop[IPV6_ASIZE];
    uint8 iface = atoi(args[4]);
    
    //sanity checks for entries

    if (len < 1 || len > 128) {
        kprintf("Invalid prefix len, has to be 1 - 128\n");
        return 1;
    }

    if (charToHex(ipAddr, args[1])) {
        kprintf("Unknown characters in destination MAC\n");
        return 1;
    }

    if (charToHex(nexthop, args[3])) {
        kprintf("Unknown characters in destination MAC\n");
        return 1;
    }

    insertNewFwdTabEntry(ipAddr, len, nexthop, iface);
    return 0;
}

