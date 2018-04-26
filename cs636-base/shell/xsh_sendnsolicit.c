#include <xinu.h>
#include <stdio.h>

shellcmd sendnsolicit(int nargs, char * args[]) {
    
    kprintf("\n");
    if (nargs != 2) {
        //print usage
        kprintf("USAGE: sendnsolicit <destination>\n");
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

    byte buf[ETH_ADDR_LEN];
    buf[0] = 0x33;
    buf[1] = 0x33;
    memcpy(buf + 2, dest + 12, 4);
    
    sendNSOL(link_local, buf, dest, dest);
       
    return 1;
}
