#include <xinu.h>

void get_link_local(byte *);
void get_snm_addr(byte *);
void get_mac_snm(byte *);

syscall bootipv6() {
    printf("MAC ADDR:\n");
    //print_addr(if_tab[ifprime].if_macucast, ETH_ADDR_LEN);
    
    //initialize link-local unicast, SNM, and MAC SNM
    get_link_local(if_tab[ifprime].if_macucast);
    get_snm_addr(link_local);
    get_mac_snm(snm_addr);

    //TODO: sanity check for the output of functions
    //if (bad) return SYSERR;

    //tell hardware to listen to MAC SNM
    
    control(ETHER0, ETH_CTRL_ADD_MCAST, (int32)mac_snm, 0);

    //TODO; Send solicitation message
    ipv6bootstrap = 1;
    sendipv6pkt();
    
    //TODO: receive router advertisement -- probably in netin
    //TODO: parse router advertisement -- probably in netin
    //maybe add a wait call here or a timer 
    
    //TODO: accept router offer -- only if offer received
    
    //TODO: remember to use hton for integers going to the network

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
    //printf("temp before flip: 0x%x", temp);
    //printf("before flip:\n");
    //print_addr(temp, 8);
    // flip the 7th bit
    temp[0] = temp[0] ^ 0x02;
    //printf("temp after flip:\n");
    //print_addr(temp, 8);
    //printf("test\n");
    memcpy(res + 8, temp, 8);

    //printf("res test:\n");
    //print_addr(res, IPV6_ASIZE);
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
    //printf("res in get_snm_addr:\n");
    //print_addr(res, IPV6_ASIZE);
    memcpy(snm_addr, res, IPV6_ASIZE);
}

void get_mac_snm(byte* snm_addr) {
    byte res[ETH_ADDR_LEN];
    memset(res, 0, 6);
    res[0] = 0x33;
    res[1] = 0x33;
    memcpy(res + 2, snm_addr + 12, 4);
    //printf("res in get_mac_snm:\n");
    //print_addr(res, ETH_ADDR_LEN);
    memcpy(mac_snm, res, ETH_ADDR_LEN);
}

void print_addr(byte* addr, int32 len){
    int i;

    if (len == ETH_ADDR_LEN) {
        for (i = 0; i < len; i += 2) {
            printf("%02x%02x.", addr[i], addr[i + 1]);
        }
    } else if (len == IPV6_ASIZE) {
        for (i = 0; i < len; i += 2) {
            printf("%02x%02x:", addr[i], addr[i + 1]);
        }
    }
    else {
        kprintf("print_addr: different length found\n");
    }
    printf("\n");
}

