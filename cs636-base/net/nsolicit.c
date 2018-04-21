#include "xinu.h"

int nsolicit_handler(struct netpacket * pkt){
    kprintf("\n sending advertisement \n");
    sendipv6pkt(NEIGHBA, pkt->net_src);
    return 1;
}
