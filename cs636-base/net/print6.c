#include <xinu.h>

void print6(struct netpacket * pkt) {
    kprintf("in print6\n");
    kprintf("ETH_ADDR_LEN: %d\n", ETH_ADDR_LEN);
    
    int32 i;
    kprintf("net_dst: ");
    for(i = 0; i < 6; i++) {
        kprintf("%02x.", pkt->net_dst[i]);
    }
    kprintf("\n");
    kprintf("net_src: ");
    for(i = 0; i < 6; i++) {
        kprintf("%02x.", pkt->net_src[i]);
    }
    kprintf("\n");
    
    kprintf("net_type: %d\n", pkt->net_type);
    kprintf("net_ethcrc: %d\n", pkt->net_ethcrc);
    kprintf("net_iface: %d\n", pkt->net_iface);
    
    kprintf("Printing IPv6 base header\n");

    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    
    kprintf("info: \n");
    kprintf("payload_len: %d\n", ipdatagram->payload_len);//includes extension header
    kprintf("next_header: %d\n", ipdatagram->next_header);
    kprintf("hop_limit: %d\n", ipdatagram->hop_limit);
    
    kprintf("ip_src: ");
    for(i = 0; i < 6; i++) {
        kprintf("%02x:", ipdatagram->src[i]);
    }
    kprintf("\n");
    
    kprintf("ip_dest: ");
    for(i = 0; i < 6; i++) {
        kprintf("%02x:", ipdatagram->dest[i]);
    }
    kprintf("\n");

    return;
}
