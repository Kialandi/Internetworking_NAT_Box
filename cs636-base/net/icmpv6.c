#include "xinu.h"

//validate the advertisement, as per RFC 4861: 6.1.2
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
                kprintf("radvert invalid\n");
                return;
            }
            radvert_handler((struct radvert *) msg, ip_payload_len);
            
            if (!host) //if nat box, send out updated advert
                //consider adding a check to see if prefix changed
                //send to all interfaces
                sendipv6pkt(ROUTERA, NULL);
            break;

        case ROUTERS:
            kprintf("icmpv6_in: RSolicit received\n");
            if (!rsolicit_valid(ipdatagram)) {
                kprintf("rsolicit invalid\n");
                return;
            }
            rsolicit_handler(pkt);
            break;
        default:
            kprintf("Unknown ICMP type: %02X\n", msg->type);
            break;
    }
}
