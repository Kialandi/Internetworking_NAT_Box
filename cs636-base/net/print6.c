#include <xinu.h>

void print6(struct netpacket * pkt) {
    kprintf("in print6\n");
    
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
    
    kprintf("net_type: 0x%X\n", pkt->net_type);
    //kprintf("net_ethcrc: %X\n", pkt->net_ethcrc);//TODO: how do i print this??
    kprintf("net_iface: %X\n", pkt->net_iface);
    
    kprintf("Printing IPv6 base header\n");

    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    
    kprintf("info: 0x%x\n", ipdatagram->info);
    kprintf("payload_len: %d\n", ntohs(ipdatagram->payload_len));//includes extension header
    kprintf("next_header: %d\n", ipdatagram->next_header);
    kprintf("hop_limit: %d\n", ipdatagram->hop_limit);
    
    kprintf("ip_src: ");
    print_addr(ipdatagram->src, IPV6_ASIZE);
   
    kprintf("ip_dest: ");
    print_addr(ipdatagram->dest, IPV6_ASIZE);
   
    return;
}
