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
                kprintf("radvert invalid, dropping pkt\n");
                return;
            }
            //just process if host
            radvert_handler((struct radvert *) msg, ip_payload_len);


            if (!host) {//if nat box, send out updated advert too
                //consider adding a check to see if prefix changed
                //send to all interfaces

                //TODO: change this to specific prefix for each interface
                kprintf("Broadcasting to iface 1 and 2\n");
                sendipv6pkt(ROUTERA, if_tab[1].if_macbcast);
                sendipv6pkt(ROUTERA, if_tab[2].if_macbcast);
            }
            break;

        case ROUTERS:
            if (host) {
                kprintf("Dropping rsol silently...\n");
                return;
            }

            kprintf("icmpv6_in: RSolicit received\n");

            if (!rsolicit_valid(ipdatagram)) {
                kprintf("rsolicit invalid, dropping pkt\n");
                return;
            }

            rsolicit_handler(pkt);

            break;
        case NEIGHBS:
            print6(pkt);
            // kprintf("\n\n received solicitation from neighbor \n\n");
            nsolicit_handler(pkt);
            break;
        case NEIGHBA:
            kprintf("\n\n nads received \n\n ");
            //print6(pkt);
            break;
        default:
            kprintf("Unknown ICMP type: %02X\n", msg->type);
            break;
    }
}
