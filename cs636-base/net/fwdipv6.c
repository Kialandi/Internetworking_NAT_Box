#include "xinu.h"

//forwarding table
struct fwdTabEntry * fwdTab[IPV6_ADDRBITS + 1];

//returns next hop's MAC address or link local
byte * getNextHop(byte * ipdest) {
    //look up forwarding table
    //if exists, pick up the ip address and return it
    //otherwise, return the default router's ip address

    //assume no TTL for now

    struct fwdTabEntry * ptr;
    int i;

    for(i = IPV6_ADDRBITS; i > 0; i--) {
        //longest to shortest
        ptr = fwdTab[i];
        while(ptr != NULL) {
            if (match(ipdest, ptr->ipAddr, IPV6_ASIZE)) {
                //entry found
                return ptr->nextHop;
            }
            ptr = ptr->next;
        }
    }
    //getting here means no entry found
    return router_link_addr;
}

uint8 match (byte * a, byte * b, uint32 len) {
    uint32 i;
    for (i = 0; i < len; i++) {
        if (a[i] != b[i]) 
            return 0;
    }
    return 1;
}


struct fwdTabEntry * insertNewFwdTabEntry(uint8 prefixLen, uint8 interface, byte * nextHop) {
    //TODO: add a limit to # of entries, maybe do a finite array
    struct fwdTabEntry * newEntry = (struct fwdTabEntry *) getmem(sizeof(fwdTabEntry));
    newEntry->iface = interface;
    memcpy(newEntry->nextHop, nextHop, ETH_ADDR_LEN);
    //replaces the current head on that index
    newEntry->next = fwdTab[prefixLen];
    fwdTab[prefixLen] = newEntry;

    return newEntry;
}


void fwdipv6_init() {
    int i;
    //set up array of linked lists
    for (i = 0; i < IPV6_ADDRBITS + 1; i++) {
        fwdTab[i]->ttl = 0;
        fwdTab[i]->iface = 0;
        memset(fwdTab[i]->ipAddr, NULLCH, IPV6_ASIZE);
        memset(fwdTab[i]->nextHop, NULLCH, ETH_ADDR_LEN);
        fwdTab[i]->next = NULL;
    }
}
/*
void fwdipv6(struct netpacket * pkt, uint32 len) {

    struct base_header * ipdatagram = (struct base_header *) pkt->net_payload;
    byte * nextHop = NULL;

    //Optimization: change to sorted trees
    //0 means we hit default router
    int i;

    byte * dest_ip = ipdatagram->dest;

    //rewrite destination to next hop
    memcpy(pkt->net_dst, entry->nextHop, ETH_ADDR_LEN);
    //TODO: Figure out if we need to rewrite net_src

    //fire off packet to the right interface
    if (entry->iface == 0) {
        if( write(ETHER0, (char *) pkt, len) == SYSERR) {
            kprintf("THE WORLD IS BURNING\n");
            kill(getpid());
        }
    }
    else if (entry->iface == 1) {
        if( write(ETHER0, (char *) pkt, len) == SYSERR) {
            kprintf("THE WORLD IS BURNING\n");
            kill(getpid());
        }
    }
    else {
        kprintf("fwipv6: FATAL ERROR!!\n");
    }

}*/
