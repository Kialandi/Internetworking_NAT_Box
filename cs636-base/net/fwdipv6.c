#include "xinu.h"

uint8 match (byte * a, byte * b) {
    uint32 i;
    for (i = 0; i < IPV6_ASIZE; i++) {
        if (a[i] != b[i]) 
            return 0;
    }
    return 1;
}

struct fwdTabEntry * fwdTab[IPV6_ADDRBITS + 1];

struct fwdTabEntry * insertNewFwdTabEntry(uint8 index, uint8 interface, byte * nextHop) {
    struct fwdTabEntry * newEntry = (struct fwdTabEntry *) getmem(sizeof(fwdTabEntry));
    newEntry->iface = interface;
    memcpy(newEntry->nextHop, nextHop, ETH_ADDR_LEN);
    //replaces the current head on that index
    newEntry->next = fwdTab[index];
    fwdTab[index] = newEntry;
    
    return newEntry;
}

struct fwdTabEntry * getEntry(uint8 len, byte * dest) {
    //get first entry at the index
    struct fwdTabEntry * ptr = fwdTab[len];

    while(ptr != NULL) {
        if (match(dest, ptr->prefix)) {
            return ptr;
        }
        ptr = ptr->next;
    }

    return NULL;
}

void fwdipv6_init() {
    int i;
    //set up array of linked lists
    for (i = 0; i < IPV6_ADDRBITS + 1; i++) {
        fwdTab[i]->iface = 0;
        memset(fwdTab[i]->nextHop, NULLCH, ETH_ADDR_LEN);
        fwdTab[i]->next = NULL;
    }
}
    
void fwdipv6(struct netpacket * pkt, uint32 len) {
    //works for hosts too, default is our NAT box
    
    struct base_header * ipdatagram = (struct base_header *) pkt->net_payload;
    struct fwdTabEntry * entry = NULL;

    //Optimization: change to sorted trees
    //0 means we hit default router
    int i;
    
    byte * dest_ip = ipdatagram->dest;

    for(i = IPV6_ADDRBITS; i >= 0; i--) {
        entry = getEntry(i, dest_ip);

        if (entry != NULL) {
            //found match
            break;
        }
    }
    
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

}
