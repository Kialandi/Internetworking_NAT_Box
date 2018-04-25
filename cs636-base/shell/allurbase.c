#include <xinu.h>
#include <stdio.h>

shellcmd allurbase(int nargs, char * args[]) {
    kprintf("\n bye bye \n");
    }
    if (!host) //nat goes to all routers
        for(int32 i = 0; i<50; i++)
            sendipv6pkt(ROUTERS, allrMACmulti, NULL);
    else //hosts go to your own bcast
        for(int32 i = 0; i<50; i++)
            sendipv6pkt(ROUTERS, if_tab[ifprime].if_macbcast, NULL);

    return 1;
}
