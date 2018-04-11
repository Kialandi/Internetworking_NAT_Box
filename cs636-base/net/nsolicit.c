#include "xinu.h"

int nsolicit_handler(struct netpacket * pkt){
    void  * payload = (void *) pkt->net_payload;
    void * retarget = 0;
    memcpy(retarget, payload + 26, 6);
    kprintf("\n sending advertisement \n");
        sendipv6pkt(NEIGHBA, retarget);
        sendipv6pkt(NEIGHBA, if_tab[1].if_macbcast);
        sendipv6pkt(NEIGHBA, if_tab[2].if_macbcast);
    return 1;
}
