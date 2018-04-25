#include <xinu.h>
#include <stdlib.h>
uint8   hasIPv6Addr;
byte    ipv6_addr[IPV6_ASIZE];
byte    link_local[IPV6_ASIZE];
byte    link_local_mac[ETH_ADDR_LEN];
byte    snm_addr[IPV6_ASIZE];
byte    mac_snm[ETH_ADDR_LEN];
byte    allrMACmulti[ETH_ADDR_LEN];
byte    allnMACmulti[ETH_ADDR_LEN];
byte    allrIPmulti[IPV6_ASIZE];
byte    allnIPmulti[IPV6_ASIZE];
bpid32  ipv6bufpool;
bpid32 	datagram_buf_pool;

struct reassembly_entry  reassembly_table[REASSEMBLY_TABLE_MAX_SIZE];
uint8 reassembly_tab_size = 0;  // check later

//TODO: make this general with negotiation
//band aid
void solicitrouter();
//band aid
void get_link_local(byte *);
void get_snm_addr(byte *);
void get_mac_snm(byte *);
//void removeReaEntry(struct reassembly_entry * entry);
void check_reassembly_time_out_task();
void check_reassembly_time_out();

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
        case NEXT_HEADER_FRAGMENT:
            kprintf("======A fragment packet received========\n");
            hexdump((char *) pkt, ETH_HDR_LEN + IPV6_HDR_LEN + 8 + 8 + 16);
            reassembly(pkt);
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
    // make reassembly fragments descriptor buffer pool
    // frag_desc_pool = mkbufpool(sizeof(struct frag_desc), )

    // make the datagram buffer pool
    /*
       datagram_buf_pool = mkbufpool(DATAGRAM_ASIZE, DATAGRAMNBUFS);
       */
    // initialize frag_list of each reassembly entry to be NULL
    uint8 i;
    for (i = 0; i < REASSEMBLY_TABLE_MAX_SIZE; i++) {
        reassembly_table[i].frag_list = NULL;
        reassembly_table[i].timestamp = 0;
    }
    // make a thread check reassembly table every REASSEMBLY_MAC_TIME * 2
    resume(create((void*) check_reassembly_time_out_task, 2048, 30, "reassembly_time_out_task", 0));

    //set prefix stuff to 0
    memset(&option_prefix_default, NULLCH, sizeof(option_prefix));

    //initialize link-local unicast, SNM, and MAC SNM
    get_link_local(if_tab[ifprime].if_macucast);
    get_snm_addr(link_local);
    get_mac_snm(snm_addr);

    //no ipv6 address yet
    hasIPv6Addr = 0;
    print_ipv6_info();

    //tell hardware to listen to your own MAC SNM
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)mac_snm, 0);
    
    //tell hardware to listen to your link local transformed MAC
    link_local_mac[0] = 0x33;
    link_local_mac[1] = 0x33;
    //copy the last 32 bits
    memcpy(link_local_mac + 2, link_local + 12, 4);
    
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)link_local_mac, 0);

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
        
        sendipv6pkt(ROUTERS, allrMACmulti, NULL);
    }
    else {
        kprintf("Host online... Sending solicitation...\n");
        sendipv6pkt(ROUTERS, allrMACmulti, NULL);
        kprintf("in ipv6_init ifacer macbcast");
        print_mac_addr(if_tab[ifprime].if_macbcast);
        //sendipv6pkt(ROUTERS, if_tab[ifprime].if_macbcast, NULL);
    }

    NDCache_init();
    fwdipv6_init();

    if(!host) {
        //only initialize nat table if you're a NAT box
        natTab_init();
    }

    //have to make sure that ipv6 information is successfull before we can send
    //neighbor stuff
    while(!hasIPv6Addr) {
        sleep(1);
    }
    
    sendipv6pkt(NEIGHBS, router_link_local_mac, router_link_local);
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

void check_reassembly_time_out_task(){
    while(1) {
        //kprintf("checking time out....");
        check_reassembly_time_out();
        sleep(REASSEMBLY_MAX_TIME);
    }

}

void check_reassembly_time_out(){
    //kprintf("I am in check_reassembly_time_out.\n");
    uint8 i;
    for (i = 0; i < REASSEMBLY_TABLE_MAX_SIZE; i++) {
        struct reassembly_entry * entry = reassembly_table + i;
        if (entry->frag_list != NULL && entry->timestamp != 0) {
            kprintf("clktime now: %u\n", clktime);
            kprintf("entry timestamp: %u\n", entry->timestamp);
            if (clktime - entry->timestamp >= REASSEMBLY_MAX_TIME) {
                removeReaEntry(entry);
            }
        }
    }
}

void removeReaEntry(struct reassembly_entry * entry) {
    // we will deal with the hole later, currently remove the fragment list
    struct frag_desc * frag_list = entry->frag_list;
    if (frag_list != NULL) {
        struct frag_desc * node = frag_list-> next;
        while (node != NULL && node->payload != NULL) {
            struct frag_desc * next = node -> next;
            freemem((char*) (node->payload), (uint32) node->payload_len);
            freemem( (char*)node, sizeof(struct frag_desc));
            node = next;
        }
        // free the dummy sentinal node
        if (node != NULL) {
            freemem( (char*)node, sizeof(struct frag_desc));
        }

    }
    entry->frag_list = NULL;
    entry->timestamp = 0;
    reassembly_tab_size--;
}

