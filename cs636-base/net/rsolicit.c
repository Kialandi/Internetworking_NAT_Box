#include "xinu.h"

void rsolicit_handler(struct rsolicit * ad) {
    kprintf("Printing router solicitation payload...\n");
    
    kprintf("type: 0x%X\n", ad->type);
    kprintf("code: 0x%X\n", ad->code);
    kprintf("checksum: %d\n", ntohs(ad->checksum));
    
    //TODO: handle options, consider using a loop?   

}

bool8 rsolicit_valid(struct base_header * ipdatagram) {
    struct rsolicit * msg = (struct rsolicit *) ((char *) ipdatagram + IPV6_HDR_LEN);
    //TODO: validate radvert
    //check icmp code = 0
    //icmp length is 8 or more
    //opt len > 0
    //if ip source unspecified, no source link layer addres option

    if (!cksum_valid(msg, ipdatagram->payload_len))
        return FALSE;
    if (msg->code != 0) 
        return FALSE;

    

    return TRUE;
}