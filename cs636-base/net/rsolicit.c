#include "xinu.h"

void rsolicit_handler(struct rsolicit * ad) {
    kprintf("Printing router solicitation payload...\n");
    
    kprintf("type: 0x%X\n", ad->type);
    kprintf("code: 0x%X\n", ad->code);
    kprintf("checksum: %d\n", ntohs(ad->checksum));
    
    //TODO: handle options, consider using a loop?   

}

bool8 rsolicit_valid(struct rsolicit * msg) {
    //TODO: validate radvert
    //check ip hop limit
    //check icmp code = 0
    //icmp length is 8 or more
    //opt len > 0
    //if ip source unspecified, no source link layer addres option

    if (!cksum_valid(msg))
        return FALSE;

    typedef struct rsolicit{
    byte type;
    byte code;
    uint16 checksum;
    uint32 reserved;
    //icmpopt opt;
}rsolicit;

    

    return TRUE;
}
