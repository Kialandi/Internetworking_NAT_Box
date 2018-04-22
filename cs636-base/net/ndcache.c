#include "xinu.h"

//forwarding table
struct NDCacheEntry * NDTab[MAXNDENTRY];

struct NDCacheEntry * lookupNDEntry(byte * ipAddr) {
    struct NDCacheEntry * ptr = NULL;
    int i;

    for(i = 0; i < MAXNDENTRY; i++) {
        if (match(ipAddr, NDTab[i]->ipAddr, IPV6_ASIZE)) {
            //found an existing entry
            ptr = NDTab[i];
        }
    }

    return ptr;
}

struct NDCacheEntry * getAvailNDEntry() {
    //returns null if NDcache is full
    struct NDCacheEntry * ptr = NULL;
    int i;
    for (i = 0; i < MAXNDENTRY; i++) {
        if (NDTab[i]->state == NDNOENTRY) {
            //found an available spot
            //zero out the entry just in case race condition
            //TODO: consider not having to do this if things are managed
            //properly
            //maybe have a process that cleans up entries every so often
            ptr = NDTab[i];
            ptr->state = NDPROCESSING;
            break;
        }
    }

    return ptr;
}

void NDCache_init() {
    int i;
    //initialize all the entries
    for (i = 0; i < MAXNDENTRY; i++) {
        NDTab[i]->ttl = 0;
        memset(NDTab[i]->ipAddr, NULLCH, IPV6_ASIZE);
        memset(NDTab[i]->macAddr, NULLCH, ETH_ADDR_LEN);
        NDTab[i]->state = NDNOENTRY;
    }
}

void print_mac(byte* addr) {
    //doesnt include newline so formatting on table is better
    int32 i;

    for (i = 0; i < ETH_ADDR_LEN - 2; i += 2) {
        kprintf("%02x%02x.", addr[i], addr[i + 1]);
    }   
    kprintf("%02x%02x", addr[i], addr[i + 1]);

}

void print_ipv6(byte* addr){
    //doesnt include newline so formatting on table is better
    int32 i;

    //TODO: implement the prefix len stuff
    for (i = 0; i < IPV6_ASIZE - 2; i += 2) {
        kprintf("%02x%02x:", addr[i], addr[i + 1]);
    }   
    kprintf("%02x%02x", addr[i], addr[i + 1]);

}

void print_state(uint8 state) {
    switch(state) {
        case NDNOENTRY:
            kprintf("No Entry\n");
            break;

        case NDINCOMPLETE:
            kprintf("INCOMPLETE\n");
            break;

        case NDREACHABLE:
            kprintf("REACHABLE\n");
            break;
        
        case NDSTALE:
            kprintf("STALE\n");
            break;
        
        case NDDELAY:
            kprintf("DELAY\n");
            break;
        
        case NDPROBE:
            kprintf("PROBE\n");
            break;

    }
}

void printNDTab() {
    kprintf("\nPrinting Neighbor Discovery Cache\n");
    kprintf("IPv6 Address                                ");
    kprintf("MAC Address      ");
    kprintf("State        \n");
    kprintf("-----------------------------------------  ");
    kprintf("---------------  ");
    kprintf("-----------  \n");
    int i;

    for (i = 0; i < MAXNDENTRY; i++) {
        if (NDTab[i]->state == NDNOENTRY) {
            //TODO: change this later to only print relevant entries
            //continue;
        }
        print_ipv6(NDTab[i]->ipAddr);
        kprintf("     ");
        print_mac(NDTab[i]->macAddr);
        kprintf("   ");
        print_state(NDTab[i]->state);
    }
}
