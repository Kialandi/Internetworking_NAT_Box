#define NOENTRY         1
#define INCOMPLETE      2
#define REACHABLE       3
#define STALE           4
#define DELAY           5
#define PROBE           6

//used for neighbour discovery, layer 2 stuff, mac/ethernet
struct NDCacheEntry {
    uint8 ttl;
    byte destIP[IPV6_ASIZE];
    byte destMAC[ETH_ADDR_LEN]; 
    uint8 state;

} NDCacheEntry;


extern	struct NDCacheEntry *   NDTab[];	/* forwarding table	*/
