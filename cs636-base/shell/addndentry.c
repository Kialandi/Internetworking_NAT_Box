#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

shellcmd addNDEntry(int nargs, char * args[]) {
    printf("\nAdding ND entry...\n");
    if (nargs != 3) {
        //print usage
        printf("USAGE: addnd <ipAddr> <mac address>\n");
        return 0;
    }
 
    struct NDCacheEntry * entry = getAvailNDEntry();
 

    byte ipAddr[IPV6_ASIZE];
    byte macAddr[ETH_ADDR_LEN];
    

    if (charToHex(ipAddr, args[1])) {
        kprintf("Unknown characters in destination MAC\n");
        return 1;
    }

    if (charToHex(macAddr, args[2])) {
        kprintf("Unknown characters in destination MAC\n");
        return 1;
    }

    entry->ttl = MAXNDTTL;
    memcpy(entry->ipAddr, ipAddr, IPV6_ASIZE);
    memcpy(entry->macAddr, macAddr, ETH_ADDR_LEN);
    entry->state = NDREACHABLE;

   
    return 0;
}

