#include "xinu.h"

//forwarding table
struct fwdTabEntry  fwdTab[MAXFWDTABENTRIES];
struct fwdTabEntry  defaultRouterEntry;

//returns next hop's entry
int16 getNextHop(byte * ipdest) {
    //assume no TTL for now
    int i;
    for (i = 0; i < MAXFWDTABENTRIES; i++) {
        //longest to shortest
        if (match(ipdest, fwdTab[i].ipAddr, fwdTab[i].prefixLen)) {
            //entry found
            //TODO: eventually add check here for ttl
            return i;
        }
    }
    //getting here means no entry found
    return -1;
}

uint8 match (byte * a, byte * b, uint32 len) {
    uint32 i;
    for (i = 0; i < len; i++) {
        if (a[i] != b[i]) 
            return 0;
    }
    return 1;
}

int16  getNextAvailFwdIndex() {
    int i;
    for (i = 0; i < MAXFWDTABENTRIES; i++) {
        if (fwdTab[i].ttl == 0)
            return i;
    }
    //no available spots
    return -1;
}

void fwdIPDatagram(struct netpacket * pkt, uint32 len) {
    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);

    int16 index = getNextHop(ipdatagram->dest);
    
    if (index == -1) {
        //default router
        memcpy(pkt->net_dst, defaultRouterEntry.nextHop, ETH_ADDR_LEN);
        memcpy(pkt->net_src, if_tab[defaultRouterEntry.iface].if_macucast, ETH_ADDR_LEN);
        pkt->net_type = htons(ETH_IPv6);
    }
    else {
        //next hop is at entry index
        //wrap with ethernet frame and send it out the next hop
        memcpy(pkt->net_dst, fwdTab[index].nextHop, ETH_ADDR_LEN);
        memcpy(pkt->net_src, if_tab[fwdTab[index].iface].if_macucast, ETH_ADDR_LEN);
        pkt->net_type = htons(ETH_IPv6);
    }

    //fire off the packet
    if( write(ETHER0, (char *) pkt, len) == SYSERR) {
        kprintf("THE WORLD IS BURNING\n");
        kill(getpid());
    }
}

int16 insertNewFwdTabEntry(byte * ipAddr, uint8 prefixLen, uint8 interface, byte * nextHop) {
    //find an empty spot
    //then add in the info
    //assumes the caller already checked if the entry already exists
    //TODO: make sure things are from longest prefix to shortest

    int16 index = getNextAvailFwdIndex();
    if (index == -1) {
        kprintf("Forwarding table full. doing nothing...\n");
        return -1;
    }

    fwdTab[index].ttl = FWDMAXTTL;
    fwdTab[index].iface = interface;
    fwdTab[index].prefixLen = prefixLen;
    memcpy(fwdTab[index].ipAddr, ipAddr, IPV6_ASIZE);
    memcpy(fwdTab[index].nextHop, nextHop, ETH_ADDR_LEN);

    return 1;
}


void fwdipv6_init() {
    int i;

    for (i = 0; i < MAXFWDTABENTRIES; i++) {
        //fwdTab[i] = (fwdTabEntry *) getmem(sizeof(fwdTabEntry));
        fwdTab[i].ttl = 0;
        fwdTab[i].iface = 0;
        memset(fwdTab[i].ipAddr, NULLCH, IPV6_ASIZE);
        memset(fwdTab[i].nextHop, NULLCH, ETH_ADDR_LEN);
    }

    //TODO: maybe loop here trying to init if hasIPv6Addr flag isn't set
    //fill in default router stuff

    defaultRouterEntry.ttl = FWDPERMANENT;
    defaultRouterEntry.iface = ifprime;
    //set to 0 since it should never be matched
    defaultRouterEntry.prefixLen = 0;
    memcpy(defaultRouterEntry.ipAddr, router_ip_addr, IPV6_ASIZE);
    memcpy(defaultRouterEntry.nextHop, router_link_addr, ETH_ADDR_LEN);
}

