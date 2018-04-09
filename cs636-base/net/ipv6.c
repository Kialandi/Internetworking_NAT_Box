#include <xinu.h>

uint8   hasIPv6Addr;
byte    ipv6_addr[IPV6_ASIZE];
byte    link_local[IPV6_ASIZE];
byte    snm_addr[IPV6_ASIZE];
byte    mac_snm[ETH_ADDR_LEN];
byte    allrMACmulti[ETH_ADDR_LEN];
byte    allnMACmulti[ETH_ADDR_LEN];
byte    allrIPmulti[IPV6_ASIZE];
byte    allnIPmulti[IPV6_ASIZE];
bpid32  ipv6bufpool;

void get_link_local(byte *);
void get_snm_addr(byte *);
void get_mac_snm(byte *);

void    ipv6_in (
        struct netpacket * pkt
        )
{
    if (match(pkt->net_src, if_tab[ifprime].if_macucast, ETH_ADDR_LEN)) {
        //if source is yourself
        kprintf("src is yourself, drop\n");
        freebuf((char *) pkt);
        return;
    }
    //kprintf("Printing incoming ipv6 pkt\n");
    //print6(pkt);

    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    //kprintf("received ipv6 pkt from iface: %d\n", pkt->net_iface);

    switch (ipdatagram->next_header) {
        //TODO: add other cases such as UDP and fragments
        case IPV6_ICMP:
            //kprintf("ipv6icmp found\n");
            //payload_hexdump((char *) ad, 64);
            icmpv6_in(pkt);
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

    //set prefix stuff to 0
    memset(&option_prefix_default, NULLCH, sizeof(option_prefix));

    //initialize link-local unicast, SNM, and MAC SNM
    get_link_local(if_tab[ifprime].if_macucast);
    get_snm_addr(link_local);
    get_mac_snm(snm_addr);

    //no ipv6 address yet
    hasIPv6Addr = 0;
    print_ipv6_info(); 
    
    //tell hardware to listen to MAC SNM
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)mac_snm, 0);

    //listen to all router and all node multicast MAC address
    byte buf[IPV6_ASIZE];
    memset(buf, NULLCH, IPV6_ASIZE);
    buf[0] = 0x33;
    buf[1] = 0x33;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x01;

    memcpy(allnMACmulti, buf, ETH_ADDR_LEN);
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)allnMACmulti, 0);

    buf[5] = 0x02;
    memcpy(allrMACmulti, buf, ETH_ADDR_LEN); 

    //this doesn't solve the problem because the default router is
    //sending a router advertisement to all nodes
    if (!host) //only listen if you're not a host
        control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)allrMACmulti, 0);

    //set up global all node/router multicast IP address
    memset(buf, NULLCH, IPV6_ASIZE);
    buf[0] = 0xff;
    buf[1] = 0x02;
    buf[15] = 0x01;
    //all nodes
    memcpy(allnIPmulti, buf, IPV6_ASIZE);

    //all routers
    buf[15] = 0x02;
    memcpy(allrIPmulti, buf, IPV6_ASIZE);

    if (!host) {
        kprintf("NAT Box autoconfiguration...\nSending solicitation...\n");
        /*
           char ch;
           while(1) {
           kprintf("Press enter to send a RSOLICIT, c to continue\n");
           read(CONSOLE, &ch, 5);
           if (ch == 'c') break;
           sendipv6pkt(ROUTERS, allrMACmulti);
           }*/
        sendipv6pkt(ROUTERS, allrMACmulti);
    }
    else {
        kprintf("Host online... Sending solicitation...\n");
        sendipv6pkt(ROUTERS, allrMACmulti);
        //sendipv6pkt(ROUTERS, if_tab[ifprime].if_macbcast);
    }
    return OK;
}

void  get_link_local(byte* mac) {
    byte res[IPV6_ASIZE];

    memset(res, NULLCH, 8); // for 64 bits prefix
    res[0] = 0xFE;
    res[1] = 0x80;

    // insert 0xFFFE in the middle of mac addr
    byte  temp[8];
    memset(temp, NULLCH, 8);
    memcpy(temp, mac, 3);
    temp[3] = 0xFF;
    temp[4] = 0xFE;
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

