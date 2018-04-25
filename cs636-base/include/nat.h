#define MAXNATENTRIES       30

//nat table is in charge of packet rewriting
struct natEntry {
    //ip src, dest, icmp ID, interface

    byte    src[IPV6_ASIZE];//internal ip address
    byte    dest[IPV6_ASIZE];//outside internet destination so we can check when reply comes
    uint8   iface;//iface it came from
    uint16  srcID;//original ID
    uint16  transID;//translated ID
    

} natEntry;


extern	struct natEntry        natTab[];	/* forwarding table	*/

extern uint16   NATIDCOUNTER;
