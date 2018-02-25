#include "xinu.h"
byte router_link_addr[ETH_ADDR_LEN];
int32  MTU;
byte default_prefix[64];
uint16 get_router_link_addr(char*);
void radvert_handler(struct radvert * ad, uint32 ip_payload_len) {
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
    uint32 options_len = ip_payload_len - sizeof(struct radvert);
    char* options = (char*) (ad + sizeof(struct radvert));
    kprintf("options_len: %d\n", options_len);
    int i = 0;
    while(i < options_len) {
	// read first byte to determine which option
	uint16 option_type = ntohs(*(options + i));
	kprintf("option_type: %d\n", option_type);
	uint16 curr_option_len = 0;
	switch(option_type) {
		case 1:	
			curr_option_len = get_router_link_addr(options + i);
			break;
		case 3:
			//curr_option_len = get_default_prefix(options + i);
			break;
		case 5:	
			//curr_option_len = get_MTU(options + i);
			break;


       	} 
	i = i + curr_option_len;
    }
	    
}

uint16 get_router_link_addr(char* option) {
	// get current option length
	uint16 curr_option_len = ntohs(*(option + 1));
	uint16 option_payload_len = curr_option_len * 8 - 2;
	memcpy(router_link_addr, option + 2, option_payload_len);	
	kprintf("src link addr: \n");
	kprintf("0x", router_link_addr);
}
bool8 radvert_valid(struct base_header * ipdatagram) {
    //TODO: validate radvert
    return TRUE;
}
