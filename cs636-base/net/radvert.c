#include "xinu.h"

byte router_link_local[IPV6_ASIZE];
byte router_link_local_mac[ETH_ADDR_LEN];
byte router_ip_addr[IPV6_ASIZE];
byte router_mac_addr[ETH_ADDR_LEN];
byte router_snm_addr[IPV6_ASIZE];
byte router_mac_snm[ETH_ADDR_LEN];

uint32  MTU;
byte prefix_default[64];
uint16 get_router_link_addr(char*);
uint16 get_MTU(char* option);
uint16 get_prefix_default(char* option) ;
struct option_prefix option_prefix_default;
struct option_prefix option_prefix_oth1;
struct option_prefix option_prefix_oth2;
struct radvert radvert_from_router;

//assigned ipv6 prefix
struct prefix_ipv6 prefix_ipv6_default;

void radvert_handler(struct radvert * ad, uint32 ip_payload_len) {
    
    memcpy(&radvert_from_router, (char*) ad, sizeof(struct radvert));
   
    uint32 options_len = ip_payload_len - sizeof(struct radvert);
    byte* options = (byte *) ( (char*)ad + sizeof(struct radvert));
    
    int i = 0;
    uint16 curr_option_len = 0;
    
    while(i < options_len) {
        // read first byte to determine which option
        uint16 option_type = (byte) options[i];
        //	kprintf("option_type: %d\n", option_type);

        switch(option_type) {
            case 1:
                curr_option_len = get_router_link_addr((char *) options + i);
                break;
            case 3:
                curr_option_len = get_prefix_default((char *) options + i);
                //curr_option_len = 32;
                break;
            case 5:
                curr_option_len = get_MTU((char *) options + i);
                break;

            default:
                curr_option_len = options_len;   // get out of while loop
                break;
        }
        i = i + curr_option_len;
    }
}

uint16 get_router_link_addr(char* option) {
    // get current option length
    uint16 curr_option_len_octets = *(option + 1);  // curr_option_len_octets is in unit of 8 octets.
    uint16 option_payload_len = curr_option_len_octets * 8 - 2;
    memcpy(router_mac_addr, option + 2, option_payload_len);
    return curr_option_len_octets * 8;
}

uint16 get_MTU(char* option) {

    uint16 curr_option_len_octets = *(option + 1);  // curr_option_len_octets is in unit of 8 octets.
    uint32 * ptr = (uint32 *) (option + 4);
    MTU = ntohl(*(ptr));
    return curr_option_len_octets * 8;

}

uint16 get_prefix_default(char* option) {
    uint16 curr_option_len_octets = *(option + 1);  // curr_option_len_octets is in unit of 8 octets.
    // save prefix and length to prefix_ipv6_default struct
    memset(&prefix_ipv6_default, NULLCH, IPV6_ASIZE) ;
    memcpy(&prefix_ipv6_default, option + 16, IPV6_ASIZE);
    prefix_ipv6_default.prefix_length = *(option + 2);
    // save the whole prefix option
    memcpy(&option_prefix_default, option, sizeof(struct option_prefix));

    //set up ipv6 unicast address
    //should be 64 bits of prefix
    memcpy(ipv6_addr, prefix_ipv6_default.ipv6, IPV6_ASIZE);

    //copy last 64 bits from link local
    memcpy(ipv6_addr + 8, &(link_local[8]), 8);
    //set flag
    hasIPv6Addr = 1;
    return curr_option_len_octets * 8;
}

bool8 radvert_valid(struct base_header * ipdatagram) {
    //TODO: validate radvert
    struct radvert  * msg = (struct radvert *) ((char *) ipdatagram + IPV6_HDR_LEN);
    uint32 pload = ntohs(ipdatagram->payload_len);
    char *start = (void *) msg + ipdatagram->payload_len; // mark start of pseudoheader
    char *sourcedest = (void *) ipdatagram + 8; // sourcedest
    char *pld = (void *) ipdatagram + 4; //payload
    char *code = (void *) ipdatagram + 6;
    memset(start, NULLCH, 40);
    memcpy(start , sourcedest, 32);
    memcpy(start + 34, pld, 2);
    memcpy(start + 39, code, 1);
    if (!cksum_valid(start, msg, pload, 40)) {
        return FALSE;
    }

    return TRUE;
}
