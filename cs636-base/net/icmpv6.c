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
            print6(pkt);
            //just process if host
            radvert_handler((struct radvert *) msg, ip_payload_len);


            if (!host) {//if nat box, send out updated advert too
                //consider adding a check to see if prefix changed
                //send to all interfaces

                //TODO: change this to specific prefix for each interface
                kprintf("Broadcasting to iface 1 and 2\n");
                print_mac_addr(if_tab[1].if_macbcast);
                kprintf("iface2 broadcast addr:\n");
                print_mac_addr(if_tab[2].if_macbcast);
                sendipv6pkt(ROUTERA, if_tab[1].if_macbcast, NULL);
                sendipv6pkt(ROUTERA, if_tab[2].if_macbcast, NULL);
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

            kprintf("icmpv6_in: Neighbor Sol received\n");
            if (!nsolicit_valid(ipdatagram)) {
                kprintf("neigh sol invalid, dropping pkt\n");
                return;
            }
            print6(pkt);
            nsolicit_handler(pkt);
            break;
        
        case NEIGHBA:
            kprintf("icmpv6_in: Neighbor Ad received\n");

            if (!nadvert_valid(ipdatagram)) {
                kprintf("neigh ad invalid, dropping pkt\n");
                return;
            }
            print6(pkt);
            nadvert_handler(pkt);
            break;
        
        case ECHOREQ:
            kprintf("icmpv6_in: Echo Request received\n");
            print6(pkt);
            break;
        
        case ECHORESP:
            kprintf("icmpv6_in: Echo Response received\n");
            print6(pkt);
            break;

        default:
            kprintf("Unknown ICMP type: 0x%02X, printing packet\n", msg->type);
            print6(pkt);
            break;
    }
}
