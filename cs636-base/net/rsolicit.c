#include "xinu.h"

void rsolicit_handler(struct netpacket * pkt) {
    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    struct rsolicit * rsol = (struct rsolicit *) ((char *) ipdatagram + IPV6_HDR_LEN);
    kprintf("Printing router solicitation payload...\n");

    kprintf("type: 0x%X\n", rsol->type);
    kprintf("code: 0x%X\n", rsol->code);
    kprintf("checksum: %d\n", ntohs(rsol->checksum));

    //TODO: only 1 valid option, source link layer address
    //check if length is greater than without
    int16 iface = pkt->net_iface;
    iface++;
}

bool8 rsolicit_valid(struct base_header * ipdatagram) {
    struct rsolicit * msg = (struct rsolicit *) ((char *) ipdatagram + IPV6_HDR_LEN);
    if (ipdatagram->hop_limit != 255) 
        return FALSE;
    if (msg->code != 0)
        return FALSE;
    if (ipdatagram->payload_len < 8) 
        return FALSE;

    //this is in bytes
    int16 opt_len = ntohs(ipdatagram->payload_len) - sizeof(struct rsolicit);
    int16 src_link_layer_opt = 0;
    if (opt_len > 0) {
        //options exist
        while (opt_len > 0) {
            struct icmpopt * opt = (struct icmpopt *) msg->opt;
            if (opt->type == 1) //flag for src link layer option
                src_link_layer_opt = 1;
            if (opt->length <= 0) 
                return FALSE;    
            //length is in units of 8 octets so multiply by 8 to get bytes
            opt_len -= opt->length * 8;
        }
    }

    //reset back to actual length
    opt_len = ntohs(ipdatagram->payload_len) - sizeof(struct rsolicit);
    byte buf[IPV6_ASIZE];
    memset(buf, NULLCH, IPV6_ASIZE);

    if (match(ipdatagram->src, buf, IPV6_ASIZE))
        if (src_link_layer_opt)
            return FALSE;

    //prepare for checksum validation
    uint32 pload = ntohs(ipdatagram->payload_len);
    char *start = (void *) msg + sizeof(msg); // mark start of pseudoheader
    char *sourcedest = (void *) ipdatagram + 8; // sourcedest
    char *pld = (void *) ipdatagram + 4; //payload
    char *code = (void *) ipdatagram + 6;
    memset(start, NULLCH, 40);
    memcpy(start , sourcedest, 32);
    memcpy(start + 34, pld, 2);
    memcpy(start + 39, code, 1);

    if (!cksum_valid(start, msg, pload, 40 ))
        return FALSE;

    return TRUE;
}
