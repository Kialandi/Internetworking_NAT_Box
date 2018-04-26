#include "xinu.h"

//forwarding table
struct fwdTabEntry  fwdTab[MAXFWDTABENTRIES];

//use this as actual fwding table
struct fwdTabEntry * fwdTabPTR[MAXFWDTABENTRIES];
struct fwdTabEntry  defaultRouterEntry;

//returns next hop's entry
int16 getNextHop(byte * ipdest) {
    //assume no TTL for now
    int i;
    for (i = 0; i < MAXFWDTABENTRIES; i++) {

                //assume that it's sorted and if it's 0 then no need to check
        if (fwdTabPTR[i]->prefixLen == 0)
            break;

        //longest to shortest
        if (match(ipdest, fwdTabPTR[i]->ipAddr, fwdTabPTR[i]->prefixLen / 8)) {
            kprintf("found nexthop\n");
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
        if (a[i] != b[i]) { 
            return 0;
        }
    }
    return 1;
}

int16  getNextAvailFwdIndex() {
    int i;
    for (i = 0; i < MAXFWDTABENTRIES; i++) {
        if (fwdTabPTR[i]->ttl == 0)
            return i;
    }
    //no available spots
    return -1;
}

//feed it a packet, figures out where to send it next and fires it off
void fwdIPDatagram(struct netpacket * pkt, uint32 len) {
    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);

    int16 index = getNextHop(ipdatagram->dest);
    byte * nextHopIP = NULL;
    uint8 iface;

    if (index == -1) {
        kprintf("fwdipdatagram: Next hop is default router\n");
        //default router
        //nextHopIP = defaultRouterEntry.nextHop;
        //iface = defaultRouterEntry.iface;
        nextHopIP = router_ip_addr;
        iface = ifprime;
    }
    else {
        kprintf("entry exists already, forward that way\n");
        //next hop is at entry index
        nextHopIP = fwdTabPTR[index]->nextHop;
        iface = fwdTabPTR[index]->iface;
    }

    //get the MAC address of the next hop
    struct NDCacheEntry * ndentry = lookupNDEntry(nextHopIP);
    
    kprintf("nextHopIP: ");
    print_ipv6_addr(nextHopIP);
    kprintf("nextHopIP's MAC: ");
    print_mac_addr(ndentry->macAddr);

    if (ndentry == NULL) {
        kprintf("no entry in neighbor cache found\n");
        return;
    }

    //rewrite ethernet frame
    memcpy(pkt->net_dst, ndentry->macAddr, ETH_ADDR_LEN);
    memcpy(pkt->net_src, if_tab[iface].if_macucast, ETH_ADDR_LEN);
    pkt->net_type = htons(ETH_IPv6);

    //fire off the packet
    if( write(ETHER0, (char *) pkt, len) == SYSERR) {
        kprintf("THE WORLD IS BURNING\n");
        kill(getpid());
    }
    kprintf("FORWARDED OUTGOING PKT PRINTING\n");
    print6(pkt);
    kprintf("FORWARDED OUTGOING PKT DONE PRINTING\n");

}

void sortFwdTab() {
    int i,j;

    //bubble sort haha!
    for (i = 0; i < MAXFWDTABENTRIES - 1; i++) {
        for (j = 0; j < MAXFWDTABENTRIES - i - 1; j++) {
            //sort by prefix length
            if (fwdTabPTR[j]->prefixLen < fwdTabPTR[j + 1]->prefixLen) {
                struct fwdTabEntry * temp = fwdTabPTR[j];
                fwdTabPTR[j] = fwdTabPTR[j + 1];
                fwdTabPTR[j + 1] = temp;
            }
        }
    }
}


int16 insertNewFwdTabEntry(byte * ipAddr, uint8 prefixLen, byte * nextHop, uint8 interface) {
    //find an empty spot
    //then add in the info
    //assumes the caller already checked if the entry already exists
    //sort to make sure things are from longest prefix to shortest
    //TODO: dont add duplicates into table


    int16 index = getNextAvailFwdIndex();
    if (index == -1) {
        kprintf("Forwarding table full. doing nothing...\n");
        return -1;
    }

    fwdTabPTR[index]->ttl = FWDMAXTTL;
    fwdTabPTR[index]->iface = interface;
    fwdTabPTR[index]->prefixLen = prefixLen;
    memcpy(fwdTabPTR[index]->ipAddr, ipAddr, IPV6_ASIZE);
    memcpy(fwdTabPTR[index]->nextHop, nextHop, IPV6_ASIZE);

    sortFwdTab();
    return 1;
}


void fwdipv6_init() {
    int i;

    for (i = 0; i < MAXFWDTABENTRIES; i++) {
        fwdTab[i].ttl = 0;
        fwdTab[i].iface = 0;
        memset(fwdTab[i].ipAddr, NULLCH, IPV6_ASIZE);
        memset(fwdTab[i].nextHop, NULLCH, IPV6_ASIZE);
        //set up pointers to the actual entries
        fwdTabPTR[i] = &fwdTab[i];
    }

    //TODO: maybe loop here trying to init if hasIPv6Addr flag isn't set
    //fill in default router stuff

    defaultRouterEntry.ttl = FWDPERMANENT;
    defaultRouterEntry.iface = ifprime;
    //set to 0 since it should never be matched and should be last entry
    defaultRouterEntry.prefixLen = 0;
    memcpy(defaultRouterEntry.ipAddr, router_ip_addr, IPV6_ASIZE);
    memcpy(defaultRouterEntry.nextHop, router_ip_addr, IPV6_ASIZE);
}

void printFWDTab() {
    kprintf("\nPrinting Forwarding Table\n");
    kprintf("IPv6 Address                              ");
    kprintf("Len   ");
    kprintf("Next Hop IPv6 Address                    ");
    kprintf("Iface\n");
    kprintf("----------------------------------------  ");
    kprintf("---  ");
    kprintf("----------------------------------------  ");
    kprintf("------  \n");
    int i;

    for (i = 0; i < MAXFWDTABENTRIES; i++) {
        //done
        if (fwdTabPTR[i]->prefixLen == 0) return;
        print_ipv6(fwdTabPTR[i]->ipAddr);
        kprintf("  ");
        kprintf(" %3d ", fwdTabPTR[i]->prefixLen);
        kprintf("  ");
        print_ipv6(fwdTabPTR[i]->nextHop);
        kprintf("  ");
        kprintf("  %2d  \n", fwdTabPTR[i]->iface);
    }   
}

