#include "xinu.h"


void sendEchoResp(byte * ip_src, byte * mac_dest, byte * ip_dest, uint16 identifier, uint16 seqNumber) {
    struct netpacket * packet = (struct netpacket *) getbuf(ipv6bufpool);
    memset((char *) packet, NULLCH, PACKLEN);

    uint32 len = ETH_HDR_LEN + IPV6_HDR_LEN + ECHOREQSIZE;
    
    fillEthernet(packet, mac_dest, ifprime);
    fillIPdatagram(packet, ip_src, ip_dest, ECHOREQSIZE, IPV6_ICMP);

    struct base_header * pkt_ip = (struct base_header *) ((char *) packet + ETH_HDR_LEN);
    struct icmpv6echoreq * req = (struct icmpv6echoreq *) ((char *) packet + ETH_HDR_LEN + IPV6_HDR_LEN);

    uint32 pseudoSize = 0;
    byte src[IPV6_ASIZE]; 
    byte dest[IPV6_ASIZE];
    memcpy(src, pkt_ip->src, IPV6_ASIZE);
    memcpy(dest, pkt_ip->dest, IPV6_ASIZE);

    req->type = ECHORESP;
    req->code = 0;
    req->checksum = 0;
    req->identifier = htons(identifier);
    req->seqNumber = htons(seqNumber);
    
    pseudoSize = PSEUDOLEN + ECHOREQSIZE;
    
    //set up pseudo header for checksum
    void * pseudo = (void *) getmem(pseudoSize);
    makePseudoHdr((struct pseudoHdr *) pseudo, src, dest, (void *) req, pseudoSize - PSEUDOLEN);
    uint16 sum = checksumv6(pseudo, pseudoSize);
    req->checksum = htons(sum);
    freemem(pseudo, pseudoSize);

    //fire off packet
    if( write(ETHER0, (char *) packet, len) == SYSERR) {
        kprintf("THE WORLD IS BURNING\n");
        kill(getpid());
    }   

    kprintf("OUTGOING PKT PRINTING\n");
    print6(packet);
    kprintf("OUTGOING PKT DONE PRINTING\n");

    freebuf((char *) packet);

}


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
            if (!host) {
                byte buf[ETH_ADDR_LEN];
                buf[0] = 0x00;
                buf[1] = 0x24;
                buf[2] = 0xC4;
                buf[3] = 0xDC;
                buf[4] = 0xCB;
                buf[5] = 0xC0;
                if(!match(pkt->net_src, buf, ETH_ADDR_LEN)) {
                    kprintf("Not from default router, dropping\n");
                    return;
                }
            }
            print6(pkt);
            //just process if host
            radvert_handler((struct radvert *) msg, ip_payload_len);

            //create the default router's global ip 
            memcpy(router_ip_addr, prefix_ipv6_default.ipv6, IPV6_ASIZE);
            memcpy(router_ip_addr + 8, &(ipdatagram->src[8]), IPV6_ASIZE);

            //create default router's link local
            memset(router_link_local, NULLCH, IPV6_ASIZE);
            router_link_local[0] = 0xFE;
            router_link_local[1] = 0x80;
            memcpy(router_link_local + 8, &(ipdatagram->src[8]), IPV6_ASIZE);

            //set default router's SNM address
            byte res[IPV6_ASIZE];
            memset(res, NULLCH, IPV6_ASIZE);
            res[0] = 0xFF;
            res[1] = 0x02;
            res[11] = 0x01;
            res[12] = 0xFF;
            // last 24 bit of unicast addr
            memcpy(res + 13, router_link_local + 13, 3);
            memcpy(router_snm_addr, res, IPV6_ASIZE);

            //set default router's MAC SNM
            memset(res, 0, 6);
            res[0] = 0x33;
            res[1] = 0x33;
            memcpy(res + 2, router_snm_addr + 12, 4);
            memcpy(router_mac_snm, res, ETH_ADDR_LEN);

            //set default router's transformed link local
            memcpy(res + 2, router_link_local + 12, 4);
            memcpy(router_link_local_mac, res, ETH_ADDR_LEN);

            if (!host) {//if nat box, send out updated advert too
                //consider adding a check to see if prefix changed
                //send to all interfaces

                //TODO: change this to specific prefix for each interface
                kprintf("Broadcasting to iface 1 and 2\n");
                print_mac_addr(if_tab[1].if_macbcast);
                kprintf("iface2 broadcast addr:\n");
                print_mac_addr(if_tab[2].if_macbcast);
                sendipv6pkt(ROUTERA, if_tab[1].if_macbcast, NULL, 1);
                sendipv6pkt(ROUTERA, if_tab[2].if_macbcast, NULL, 2);
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
            kprintf("icmpv6_in: Neighbor Sol received\n");
            if (!nsolicit_valid(ipdatagram)) {
                kprintf("neigh sol invalid, dropping pkt\n");
                return;
            }

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
            struct icmpv6echoreq * req = (struct icmpv6echoreq *) msg;
            //send a response back
            print6(pkt);
            sendEchoResp(ipdatagram->dest, pkt->net_src, ipdatagram->src, ntohs(req->identifier), ntohs(req->seqNumber));
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
