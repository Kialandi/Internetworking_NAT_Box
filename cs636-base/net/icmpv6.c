#include "xinu.h"

void icmpv6_in(struct icmpv6general * msg) {
    //TODO: figure out what type of msg it is call the appropriate function
    //validate the advertisement, as per RFC 4861: 6.1.2
    switch (msg->type) {

        case ROUTERA:
            kprintf("ROUTERA found\n");
            if (!radvert_valid((struct radvert *) msg)) {
                //consider making this return something
                kprintf("radvert invalid\n");
                return;
            }
            radvert_handler((struct radvert *) msg);
            break;

        case ROUTERS:
            kprintf("R Solicit received\n");
            if (!rsolicit_valid((struct rsolicit *) msg)) {
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

