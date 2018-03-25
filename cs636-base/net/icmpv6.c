#include "xinu.h"

//TODO: figure out what type of msg it is call the appropriate function
//validate the advertisement, as per RFC 4861: 6.1.2
//void icmpv6_in(struct icmpv6general * msg, uint32 ip_payload_len) {
void icmpv6_in(struct netpacket * pkt) {
    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    struct icmpv6general * msg = (struct icmpv6general *) ((char *) ipdatagram + IPV6_HDR_LEN);
    uint32 ip_payload_len = ntohs(ipdatagram->payload_len);
    
    //kprintf("ip_payload_len: %d\n", ip_payload_len);
    //figure out what type of msg it is and call the appropriate function
    switch (msg->type) {
        case ROUTERA:
            kprintf("icmpv6_in: RAdvert received\n");
            if (!radvert_valid(ipdatagram)) {
                //consider making this return something
                kprintf("radvert invalid\n");
                return;
            }
            radvert_handler((struct radvert *) msg, ip_payload_len);
            //probably uncomment later when it all works
            //sendipv6pkt(ROUTERA, NULL); // just for testing purpose
            break;

        case ROUTERS:
            kprintf("icmpv6_in: RSolicit received\n");
            if (!rsolicit_valid(ipdatagram)) {
                //consider making this return something
                kprintf("rsolicit invalid\n");
                return;
            }
            //TODO: send an advertisement back
            rsolicit_handler(pkt);
            //sendipv6pkt(ROUTERA, ALLNODES);
            break;
        default:
            kprintf("Unknown ICMP type: %02X\n", msg->type);
            break;
    }
}
