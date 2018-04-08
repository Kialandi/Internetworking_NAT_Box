
#define MAXFWDTABENTRIES    50

//fwd table in charge of figuring out what next hop is
struct	fwdTabEntry {			/* entry in the forwarding table	*/
    
    int16 ttl;      //time to live, -1 means permanent
    uint8 iface;
    byte ipAddr[IPV6_ASIZE];//max of 128 bits for ipv6 addresses
    byte nextHop[ETH_ADDR_LEN]; //maybe change depending on iface
    struct fwdTabEntry * next;

} fwdTabEntry;

//used for neighbour discovery, layer 2 stuff, mac/ethernet
struct NDCacheEntry {
    uint8 ttl;
    byte destIP[IPV6_ASIZE];
    byte destMAC[ETH_ADDR_LEN]; 

} NDCacheEntry;

//nat table is in charge of packet rewriting
struct natEntry {
    //ip src, dest, icmp ID, interface
   
    byte    src[IPV6_ASIZE];
    byte    dest[IPV6_ASIZE];
    uint8   iface;
    uint16  srcID;
    uint16  transID;
} natEntry;


extern	struct fwdTabEntry *    fwdTab[];	/* forwarding table	*/
extern	struct NDCacheEntry *   NDTab[];	/* forwarding table	*/
extern	struct fwdTabEntry *    natTab[];	/* forwarding table	*/
