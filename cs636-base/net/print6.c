#include <xinu.h>

void print6(struct netpacket * pkt) {
    kprintf("Printing incoming packet...\n");
    kprintf("=========== Printing Ethernet header ===========\n");
    
    kprintf("net_dst: ");
    print_mac_addr(pkt->net_dst);
    kprintf("net_src: ");
    print_mac_addr(pkt->net_src);
    
    kprintf("net_type: 0x%X\n", pkt->net_type);
    
    kprintf("========== Printing IPv6 base header ===========\n");

    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    
    kprintf("info: 0x%x\n", ipdatagram->info);
    kprintf("payload_len: %d\n", ntohs(ipdatagram->payload_len));
    kprintf("next_header: %d\n", ipdatagram->next_header);
    kprintf("hop_limit: %d\n", ipdatagram->hop_limit);
    
    kprintf("ip_src: ");
    print_ipv6_addr(ipdatagram->src);
   
    kprintf("ip_dest: ");
    print_ipv6_addr(ipdatagram->dest);
   
    char * payload = (char *) ipdatagram + IPV6_HDR_LEN;
    kprintf("=============== Printing Payload ===============\n");
    payload_hexdump(payload, ntohs(ipdatagram->payload_len));
    
    return;
}

void payload_hexdump(char * p, int32 len) {
    int32 i;
    for (i = 0; i < len; i++) {
        if (i == 0) {
            kprintf("%02x", 0xff & *p++);
            continue;
        }
        if ((i % 16) == 0) {
            kprintf("\n");
        }
        else if ((i % 4) == 0) {
            kprintf(" ");
        }
        kprintf("%02x", 0xff & *p++);
    }
    kprintf("\n");
}

void print_mac_addr(byte* addr) {
    int32 i;

    for (i = 0; i < ETH_ADDR_LEN - 2; i += 2) {
        kprintf("%02x%02x.", addr[i], addr[i + 1]);
    }
    kprintf("%02x%02x\n", addr[i], addr[i + 1]);

}

void print_ipv6_addr(byte* addr){
    int32 i;

    //TODO: implement the prefix len stuff
    for (i = 0; i < IPV6_ASIZE - 2; i += 2) {
        kprintf("%02x%02x:", addr[i], addr[i + 1]);
    }
    kprintf("%02x%02x\n", addr[i], addr[i + 1]);
    
}
