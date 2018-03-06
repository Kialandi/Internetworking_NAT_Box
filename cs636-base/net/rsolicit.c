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
    char *start = (void *) msg + sizeof(msg); // mark start of pseudoheader
    char *sourcedest = (void *) ipdatagram + 8; // sourcedest
    char *pld = (void *) ipdatagram + 4; //payload
    char *code = (void *) ipdatagram + 6;
    memset(start, NULLCH, 40);
    memcpy(start , sourcedest, 32);
    memcpy(start + 34, pld, 2);
    memcpy(start + 39, code, 1);
    if (!cksum_valid(msg, sizeof(msg)+40))
        return FALSE;
    if (msg->code != 0)
        return FALSE;
    return TRUE;

}
