#include "xinu.h"
byte router_link_addr[ETH_ADDR_LEN];
uint32  MTU;
byte prefix_default[64];
uint16 get_router_link_addr(char*);
uint16 get_MTU(char* option);
struct option_prefix option_prefix_default;
uint16 get_prefix_default(char* option) ;
struct radvert radvert_from_router;

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
     
     memcpy(&radvert_from_router, ad, sizeof(struct radvert));   
     kprintf("test for advert_from_router\n");
     payload_dump((char*) ad, sizeof(struct radvert));
    //TODO: handle options, consider using a loop?   
    uint32 options_len = ip_payload_len - sizeof(struct radvert);
    byte* options = (byte *) ( (char*)ad + sizeof(struct radvert));
    kprintf("options_len: %d\n", options_len);
    int i = 0; 
    uint16 curr_option_len = 0;
    while(i < options_len) {
	// read first byte to determine which option		
	uint16 option_type = (byte) options[i];
	kprintf("option_type: %d\n", option_type);

	switch(option_type) {
		case 1:	
			curr_option_len = get_router_link_addr(options + i);
			break;
		case 3:
			curr_option_len = get_prefix_default(options + i);
			//curr_option_len = 32;
			break;
		case 5:	
			curr_option_len = get_MTU(options + i);
			break;

		default:
			curr_option_len = options_len;   // get out of while loop
			break;
       	} 
//	kprintf("curr_option_len: %d\n", curr_option_len);
	i = i + curr_option_len;
//	break;
    }
	    
}

uint16 get_router_link_addr(char* option) {
	// get current option length
	uint16 curr_option_len_octets = *(option + 1);  // curr_option_len_octets is in unit of 8 octets.
	//kprintf("curr_option_len_octets: %d\n", curr_option_len_octets);
	uint16 option_payload_len = curr_option_len_octets * 8 - 2;
	
	memcpy(router_link_addr, option + 2, option_payload_len);	
	kprintf("src link addr:");
	print_mac_addr(router_link_addr);
       return curr_option_len_octets * 8;
}

uint16 get_MTU(char* option) {
	
	uint16 curr_option_len_octets = *(option + 1);  // curr_option_len_octets is in unit of 8 octets.
	//kprintf("curr_option_len_octets: %d\n", curr_option_len_octets);
	//uint16 option_payload_len = curr_option_len_octets * 8 - 2;
	uint32 * ptr = option + 4;
	MTU = ntohl(*(ptr));
	kprintf("MTU: %d\n", MTU);
       return curr_option_len_octets * 8;

}

uint16 get_prefix_default(char* option) {
		
	uint16 curr_option_len_octets = *(option + 1);  // curr_option_len_octets is in unit of 8 octets.
	//kprintf("curr_option_len_octets: %d\n", curr_option_len_octets);
	//uint16 option_payload_len = curr_option_len_octets * 8 - 2;
	memcpy(&option_prefix_default, option, sizeof(struct option_prefix));
	byte * ptr = option + 2;
	uint16 prefix_length = *ptr;
	kprintf("prefix_length: %d\n", prefix_length);
	kprintf("advertised prefix:");
	payload_hexdump(&(option_prefix_default.payload), 16);        
       return curr_option_len_octets * 8;
}

bool8 radvert_valid(struct base_header * ipdatagram) {
    //TODO: validate radvert
    return TRUE;
}
