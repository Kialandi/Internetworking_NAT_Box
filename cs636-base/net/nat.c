#include "xinu.h"

//forwarding table
//TODO: REFACTOR CODE TO USE OBJECTS INSTEAD OF PTRS
struct natEntry natTab[MAXNATENTRIES];
uint16 NATIDCOUNTER;

uint16 getNewID() {
    NATIDCOUNTER++;

    return NATIDCOUNTER - 1;
}

//incoming packet
//if entry exists in NAT table, rewrite
int32 incPktRW(struct netpacket * pkt) {
    //look up NAT table
    //if entry matches, rewrite the packet's ID
    //otherwise, return the default router's ip address

    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    struct icmpv6echoreq * msg = (struct icmpv6echoreq *) ((char *) ipdatagram + IPV6_HDR_LEN);
 
    struct natEntry * ptr;
    int i;
    uint16 msgID = ntohs(msg->identifier);
    
    //TODO: probably add a check here to see what type we are changing
    //if TCP/UDP, change the port number
    for(i = 0; i < MAXNATENTRIES; i++) {
        ptr = &natTab[i];
        
        //look into the packet and grab the id, compare with entry
        //TODO: probably have to add TTL check here eventually
        //perhaps check if dest is yourself

        if (ptr->transID == msgID && match(ptr->dest, ipdatagram->src, IPV6_ASIZE)) {
            //rewrite id;
            msg->identifier = htons(ptr->transID);
            memcpy(ipdatagram->src, ptr->src, IPV6_ASIZE);

            //recompute checksum
            msg->checksum = 0;
            void * pseudo = (void *) getmem(PSEUDOLEN + ECHOREQSIZE);
            makePseudoHdr((struct pseudoHdr *) pseudo, ipdatagram->src, ipdatagram->dest, (void *) msg, ECHOREQSIZE);

            uint16 sum = checksumv6(pseudo, PSEUDOLEN + ECHOREQSIZE);
            msg->checksum = htons(sum);
            freemem(pseudo, PSEUDOLEN + ECHOREQSIZE);
            
            //let caller handle sending it out
            return ptr->iface;
        }
    }

    //no rewrite happened, or maybe consider returning the interface
    return -1;
}

int32 outPktRW(uint8 interface, struct netpacket * pkt) {
    //for now this is assuming icmp echo
    struct base_header * ipdatagram = (struct base_header *) &(pkt->net_payload);
    struct icmpv6echoreq * msg = (struct icmpv6echoreq *) ((char *) ipdatagram + IPV6_HDR_LEN);

    struct natEntry * ptr;

    //get next available entry, if non, maybe block or return fail
    uint16 newID = getNewID();

    if (newID == -1) {
        kprintf("No valid entries in NAT table\n");
        return 1;
    }

    ptr = &natTab[newID];

    //insert into nat table
    memcpy(ptr->src, ipdatagram->src, IPV6_ASIZE);
    memcpy(ptr->dest, ipdatagram->dest, IPV6_ASIZE);
    ptr->iface = interface;
    ptr->srcID = msg->identifier;
    ptr->transID = newID;

    //rewrite the ID and src ip
    memcpy(ipdatagram->src, ipv6_addr, IPV6_ASIZE);
    msg->identifier = htons(newID);

    //recompute checksum
    msg->checksum = 0;
    void * pseudo = (void *) getmem(PSEUDOLEN + ECHOREQSIZE);
    makePseudoHdr((struct pseudoHdr *) pseudo, ipdatagram->src, ipdatagram->dest, (void *) msg, ECHOREQSIZE);

    uint16 sum = checksumv6(pseudo, PSEUDOLEN + ECHOREQSIZE);
    msg->checksum = htons(sum);
    freemem(pseudo, PSEUDOLEN + ECHOREQSIZE);

    return 0;
}

void natTab_init() {
    int i;
    //set up nat table
    for (i = 0; i < MAXNATENTRIES; i++) {
        natTab[i].srcID = 0;
        natTab[i].transID = 0;
        natTab[i].iface = 0;
        memset(natTab[i].src, NULLCH, IPV6_ASIZE);
        memset(natTab[i].dest, NULLCH, IPV6_ASIZE);
    }
    NATIDCOUNTER = 0;
}

void printNATTab() {
    kprintf("\nPrinting NAT Table\n\n");
    kprintf("Internal                                 ");
    kprintf("Internal   ");
    kprintf("Dest                                     ");
    kprintf("Trans  ");
    kprintf("Iface\n");
    kprintf("Address                                    ");
    kprintf(" ID      ");
    kprintf("Address                                  ");
    kprintf(" ID     \n");
    kprintf("---------------------------------------- ");
    kprintf("--------  ");
    kprintf("---------------------------------------- ");
    kprintf("------  ");
    kprintf("-----  \n");
    int i;

    for (i = 0; i < MAXFWDTABENTRIES; i++) {
        //done
        if (fwdTabPTR[i]->prefixLen == 0) return;
        print_ipv6(natTab[i].src);
        kprintf(" ");
        kprintf("  %3d  ", natTab[i].srcID);
        kprintf("    ");
        print_ipv6(natTab[i].dest);
        kprintf("  ");
        kprintf(" %3d  ", natTab[i].transID);
        kprintf(" ");
        kprintf(" %3d  \n", natTab[i].iface);
    }   
}

