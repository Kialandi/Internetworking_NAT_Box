#include "xinu.h"

int nsolicit_handler(struct netpacket * pkt){
    void * retarget = 0;
    void  * payload = (void *) pkt->net_payload;
    memcpy(retarget, payload + 8, 6);
    kprintf("\n sending advertisement \n");
    for(int i = 0; i<6; i++){
        char * tg = retarget;
        kprintf("%d", tg);
    }
    sendipv6pkt(NEIGHBA, if_tab[1].if_macbcast);
    sendipv6pkt(NEIGHBA, if_tab[2].if_macbcast);

    return 1;
 }
