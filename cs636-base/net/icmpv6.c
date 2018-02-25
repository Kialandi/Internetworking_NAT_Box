#include "xinu.h"

    //TODO: figure out what type of msg it is call the appropriate function
    //validate the advertisement, as per RFC 4861: 6.1.2
//void icmpv6_in(struct icmpv6general * msg, uint32 ip_payload_len) {
void icmpv6_in(struct base_header * ipdatagram) {
    struct icmpv6general * msg = (struct icmpv6general *) ((char *) ipdatagram + IPV6_HDR_LEN);

    //figure out what type of msg it is call the appropriate function
    switch (msg->type) {
        case ROUTERA:
            kprintf("ROUTERA found\n");
            if (!radvert_valid(ipdatagram)) {
                //consider making this return something
                kprintf("radvert invalid\n");
                return;
            }
            radvert_handler((struct radvert *) msg, ip_payload_len);
            break;

        case ROUTERS:
            kprintf("RSolicit received\n");
            if (!rsolicit_valid(ipdatagram)) {
                //consider making this return something
                kprintf("rsolicit invalid\n");
                return;
            }
            //TODO: send an advertisement back
            //sendipv6pkt(ROUTERA, ALLNODES);
            break;
        default:
            kprintf("Unknown ICMP type: %02X\n", msg->type);
            break;
    }
}

