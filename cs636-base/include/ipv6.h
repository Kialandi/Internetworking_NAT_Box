/* ipv6.h  -  Constants related to Internet Protocol version 6 (IPv6) */

#define	IPV6_ICMP		0x3A		/* ICMP protocol type for IP 	*/
#define	IPV6_UDP		0x11		/* UDP protocol type for IP 	*/
#define IPV6_TCP        0x06
#define IPV6_FRAG       0x2C

#define IPV6_ADDRBITS   128
#define	IPV6_ASIZE	    16		/* Bytes in an IP address	*/
#define	IPV6_HDR_LEN	40		/* Bytes in an IP header	*/
#define IPV6_VH		    0x45 	/* IP version and hdr length 	*/

#define IPV6OUTNBUFS    20      /* number of buffers */

struct base_header {
    //4bit    version;
    //byte    traffic;
    //20bit   flowLabel;
    byte    info[4];
    uint16  payload_len;
    byte    next_header;
    uint8   hop_limit;
    byte    src[IPV6_ASIZE];
    byte    dest[IPV6_ASIZE];
    byte    payload[0];//see if this breaks things
};

extern uint8    hasIPv6Addr;
extern byte     ipv6_addr[];
extern byte     link_local[];
extern byte     snm_addr[];
extern byte     mac_snm[];
extern byte     allrMACmulti[];//All routers MAC
extern byte     allnMACmulti[];//All nodes MAC
extern byte     allrIPmulti[];//All routers IP
extern byte     allnIPmulti[];//All nodes IP
extern bpid32   ipv6bufpool;
extern byte 	router_link_addr[];//router's MAC address?
extern uint32	MTU;
extern byte	prefix_default[];

struct prefix_ipv6 {
    byte ipv6[IPV6_ASIZE];
    uint8 prefix_length;
} prefix_ipv6;

extern struct prefix_ipv6 prefix_ipv6_default;
