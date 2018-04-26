#include <xinu.h>
#include <stdio.h>

shellcmd sendradvert(int nargs, char * args[]) {
    kprintf("\n");
    if (!hasIPv6Addr) {
        kprintf("No prefix found. Please solicit first!\n");
        return 0;
    }
    if (nargs != 1) {
        //print usage
        printf("USAGE: sendradvert\n");
        return 0;
    }

    if (!host) {
        kprintf("NAT: Multicasting to both interfaces\n");
        //broadcast to both interfaces
        sendipv6pkt(ROUTERA, if_tab[1].if_macbcast, NULL, NULL);
        sendipv6pkt(ROUTERA, if_tab[2].if_macbcast, NULL, NULL);
    }
    else
        kprintf("Not a NAT box, can't send RADVERTS\n");

    return 1;
}
