


struct	fwdTabEntry {			/* entry in the forwarding table	*/
    uint8 iface;
    byte prefix[16];//max of 128 bits for ipv6 addresses
    byte nextHop[6];//[ETH_ADDR_LEN]; //maybe change depending on iface
    struct fwdTabEntry * next;
} fwdTabEntry;

extern	struct fwdTabEntry * fwdTab[];	/* forwarding table	*/
