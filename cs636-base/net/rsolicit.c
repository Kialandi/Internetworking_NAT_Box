#include "xinu.h"

void rsolicit_handler(struct rsolicit * ad) {
    kprintf("Printing router solicitation payload...\n");
    
    kprintf("type: 0x%X\n", ad->type);
    kprintf("code: 0x%X\n", ad->code);
    kprintf("checksum: %d\n", ntohs(ad->checksum));
    
    //TODO: handle options, consider using a loop?   

}

bool8 rsolicit_valid(struct rsolicit * ad) {
    //TODO: validate radvert
    return TRUE;
}
