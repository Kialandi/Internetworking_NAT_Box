#include "xinu.h"

//forwarding table
struct NDCacheEntry * NDCache[MAXNDENTRY];

struct NDCacheEntry * lookupNDEntry(byte * ipAddr) {
    struct NDCacheEntry * ptr = NULL;
    int i;

    for(i = 0; i < MAXNDENTRY; i++) {
        if (match(ipAddr, NDCache[i]->ipAddr, IPV6_ASIZE)) {
            //found an existing entry
            //kprintf("lookupNDEntry: found matching entry\n");
            ptr = NDCache[i];
            break;
        }
    }

    return ptr;
}

void createEntry(uint32 ttl, byte * ip, byte * maddr, uint8 state){
    struct NDCacheEntry *nd = (struct NDCacheEntry *) NDCache;
    struct NDCacheEntry *nd2 = (struct NDCacheEntry *) NDCache;
        for(uint32 i=0; i<MAXNDENTRY; i++){
            if(nd->ipAddr != NULL){
                nd->ttl = ttl;
                memcpy(nd->ipAddr, ip, IPV6_ASIZE);
                memcpy(nd->macAddr, maddr, ETH_ADDR_LEN);
                nd->state = state;
                break;
            }
            /* if no entry found, replace oldest entry */

            if(i == MAXNDENTRY - 1){
                for(uint32 j = 0; j<MAXNDENTRY; j++){
                    if(ttl > nd2->ttl){
                        nd2->ttl = ttl;
                        memcpy(nd2->ipAddr, ip, IPV6_ASIZE);
                        memcpy(nd2->macAddr, maddr, ETH_ADDR_LEN);
                        nd2->state = state;
                    }
                }
            }
    }
}

struct NDCacheEntry * getAvailNDEntry() {
    //returns null if NDcache is full
    struct NDCacheEntry * ptr = NULL;
    int i;
    for (i = 0; i < MAXNDENTRY; i++) {
        if (NDCache[i]->state == NDNOENTRY) {
            //kprintf("getAvailNDEntry: found available entry\n");
            //found an available spot
            //zero out the entry just in case race condition
            //TODO: consider not having to do this if things are managed
            //properly
            //maybe have a process that cleans up entries every so often
            ptr = NDCache[i];
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
        NDCache[i] = (struct NDCacheEntry *) getmem(sizeof(NDCacheEntry));
        NDCache[i]->ttl = 0;
        memset(NDCache[i]->ipAddr, NULLCH, IPV6_ASIZE);
        memset(NDCache[i]->macAddr, NULLCH, ETH_ADDR_LEN);
        NDCache[i]->state = NDNOENTRY;
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
        if (NDCache[i]->state == NDNOENTRY) {
            //TODO: change this later to only print relevant entries
            //continue;
        }
        print_ipv6(NDCache[i]->ipAddr);
        kprintf("     ");
        print_mac(NDCache[i]->macAddr);
        kprintf("   ");
        print_state(NDCache[i]->state);
    }
}
