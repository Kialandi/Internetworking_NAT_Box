
#define FWDPERMANENT        -1
#define FWDMAXTTL           900
#define MAXFWDTABENTRIES    30

//fwd table in charge of figuring out what next hop is
struct	fwdTabEntry {			/* entry in the forwarding table	*/
    
    int16 ttl;      //time to live, -1 means permanent
    uint8 iface;
    uint8 prefixLen;
    byte ipAddr[IPV6_ASIZE];//max of 128 bits for ipv6 addresses
    //TODO: this is actually an ip address
    byte nextHop[IPV6_ASIZE]; //maybe change depending on iface

} fwdTabEntry;

extern struct fwdTabEntry  defaultRouterEntry;
extern	struct fwdTabEntry     fwdTab[];	/* forwarding table	*/
extern	struct fwdTabEntry  *   fwdTabPTR[];	/* forwarding table	pointers to avoid copying */

