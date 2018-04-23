
//states for entries
#define NDNOENTRY         0
#define NDINCOMPLETE      1
#define NDREACHABLE       2
#define NDSTALE           3
#define NDDELAY           4
#define NDPROBE           5

//my own flag to keep away race conditions
#define NDPROCESSING      10

#define MAXNDENTRY      30
#define MAXNDTTL        900 //900 seconds to live

//used for neighbour discovery, layer 2 stuff, mac/ethernet
struct NDCacheEntry {
    uint32 ttl;
    byte ipAddr[IPV6_ASIZE];
    byte macAddr[ETH_ADDR_LEN]; 
    uint8 state;
} NDCacheEntry;

extern	struct  NDCacheEntry *   NDCache[];	/* forwarding table	*/
