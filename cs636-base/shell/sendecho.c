#include <xinu.h>
#include <stdio.h>


shellcmd sendecho(int nargs, char * args[]) {
    kprintf("\n");
    if (nargs != 2) {
        //print usage
        kprintf("USAGE: sendecho <destination>\n");
        return 0;
    }
    
    char * ptr = args[1];
    uint32 len = 0;

    while(*ptr != 0) {
        ptr++;
        len++;
    }

    if (len != 39) {
        kprintf("Invalid IPv6 dest, please try with XXXX:XXXX: ... format\n");
        return 1;
    }

    byte dest[IPV6_ASIZE];
    memset(dest, NULLCH, IPV6_ASIZE);

    if (charToHex(dest, args[1])) {
        kprintf("Unknown characters in destination address\n");
        return 1;
    }
    //TODO: add the routing table lookup here, given an ip address, return a mac
    sendipv6pkt(ECHOREQ, router_mac_addr, dest);
    
    return 1;
}
