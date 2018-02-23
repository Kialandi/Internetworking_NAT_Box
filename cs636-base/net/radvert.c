#include "xinu.h"

void radvert_handler(struct radvert * ad) {
    kprintf("Printing router advertisement payload...\n");
    
    kprintf("type: 0x%X\n", ad->type);
    kprintf("code: 0x%X\n", ad->code);
    kprintf("checksum: %d\n", ntohs(ad->checksum));
    kprintf("curhoplim: %d\n", ad->curhoplim);
    kprintf("m_o_res: 0x%X\n", ad->m_o_res);

    kprintf("routerlifetime: %d\n", ntohs(ad->routerlifetime));
    kprintf("reachabletime: %d\n", ntohl(ad->reachabletime));
    kprintf("retranstimer: %d\n", ntohl(ad->retranstimer));
    //TODO: handle options, consider using a loop?   
}

bool8 radvert_valid(struct base_header * ipdatagram) {
    //TODO: validate radvert
    return TRUE;
}
