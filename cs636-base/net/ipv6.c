#include <xinu.h>

byte    link_local[IPV6_ASIZE];
byte    snm_addr[IPV6_ASIZE];
byte    mac_snm[ETH_ADDR_LEN];
byte    allrmulti[IPV6_ASIZE];
byte    allnmulti[IPV6_ASIZE];
bpid32  ipv6bufpool;

void get_link_local(byte *);
void get_snm_addr(byte *);
void get_mac_snm(byte *);

void    ipv6_in (
        struct netpacket * pkt
        )
{
    print6(pkt);
    
    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    //void * payload = (void *) ((char *) ipdatagram + IPV6_HDR_LEN);
    //uint32 payload_len = ntohs(ipdatagram->payload_len);

    switch (ipdatagram->next_header) {
        //TODO: add other cases such as UDP and fragments
        case IPV6_ICMP:
            kprintf("ipv6icmp found\n");
            //payload_hexdump((char *) ad, 64);
            //icmpv6_in((struct icmpv6general *) payload, payload_len);
            icmpv6_in(ipdatagram);
            break;

        default:
            kprintf("Unhandled next header type: 0x%02X\n", ipdatagram->next_header);
            break;
    }

    freebuf((char *) pkt);
    return;
}


syscall ipv6_init() {
    ipv6bufpool = mkbufpool(PACKLEN, IPV6OUTNBUFS);
    if (ipv6bufpool == SYSERR) {
        kprintf("ipv6_init cannot allocate network buffer pool\n");
        kill(getpid());
    }

    //initialize link-local unicast, SNM, and MAC SNM
    get_link_local(if_tab[ifprime].if_macucast);
    get_snm_addr(link_local);
    get_mac_snm(snm_addr);

    kprintf("\n======================= IPv6 addresses =======================\n\n");
    kprintf("Link local address : ");
    print_ipv6_addr(link_local);
    kprintf("SNM address        : ");
    print_ipv6_addr(snm_addr);
    kprintf("MAC SNM address    : ");
    print_mac_addr(mac_snm);
    kprintf("\n==============================================================\n");
    //tell hardware to listen to MAC SNM
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)mac_snm, 0);

    //listen to all router and all node multicast
    byte buf[6];
    buf[0] = 0x33;
    buf[1] = 0x33;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x01;

    memcpy(allnmulti, buf, IPV6_ASIZE);
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)allnmulti, 0);

    buf[5] = 0x02;
    memcpy(allrmulti, buf, IPV6_ASIZE); 
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)allrmulti, 0);

     sendipv6pkt(ROUTERS, NULL);
    //TODO; Send solicitation message
    /*
    while(1) {
        kprintf("Press enter to send another router solicitation\n");
        read(CONSOLE, NULL, 5);
        sendipv6pkt(ROUTERS, NULL);
    }
    */
    return OK;
}

void  get_link_local(byte* mac) {
    byte res[IPV6_ASIZE];

    memset(res, NULLCH, 8); // for 64 bits prefix
    res[0] = 0xFE;
    res[1] = 0x80;

    // insert 0xFFEE in the middle of mac addr
    byte  temp[8];
    memset(temp, NULLCH, 8);
    memcpy(temp, mac, 3);
    temp[3] = 0xFF;
    temp[4] = 0xEE;
    memcpy(temp + 5, mac + 3, 3);

    // flip the 7th bit
    temp[0] = temp[0] ^ 0x02;
    memcpy(res + 8, temp, 8);

    memcpy(link_local, res, IPV6_ASIZE);
}

void get_snm_addr(byte* unicast){
    byte res[IPV6_ASIZE];
    memset(res, NULLCH, IPV6_ASIZE);

    // 104 bit prefix :FF02:0::01:FF
    res[0] = 0xFF;
    res[1] = 0x02;
    res[11] = 0x01;
    res[12] = 0xFF;

    // last 24 bit of unicast addr
    memcpy(res + 13, unicast + 13, 3);

    memcpy(snm_addr, res, IPV6_ASIZE);
}

void get_mac_snm(byte* snm_addr) {
    byte res[ETH_ADDR_LEN];
    memset(res, 0, 6);
    res[0] = 0x33;
    res[1] = 0x33;
    memcpy(res + 2, snm_addr + 12, 4);
    memcpy(mac_snm, res, ETH_ADDR_LEN);
}

