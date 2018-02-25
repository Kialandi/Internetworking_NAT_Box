#include "xinu.h"

void icmpv6_in(struct icmpv6general * msg, uint32 ip_payload_len) {
    //TODO: figure out what type of msg it is call the appropriate function
    //validate the advertisement, as per RFC 4861: 6.1.2
    switch (msg->type) {
        case ROUTERA:
            if (!radvert_valid((struct radvert *) msg)) {
                //consider making this return something
                return;
            }
            radvert_handler((struct radvert *) msg, ip_payload_len);
            break;
        case ROUTERS:
            break;
        default:
            kprintf("Unknown ICMP type: %02X\n", msg->type);
            break;
    }
}

