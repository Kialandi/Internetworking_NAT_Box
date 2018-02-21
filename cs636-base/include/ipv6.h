/* ipv6.h  -  Constants related to Internet Protocol version 6 (IPv6) */


#define	IPV6_ICMP		0x3A		/* ICMP protocol type for IP 	*/
#define	IPV6_UDP		17		/* UDP protocol type for IP 	*/

#define	IPV6_ASIZE	    16		/* Bytes in an IP address	*/
#define	IPV6_HDR_LEN	40		/* Bytes in an IP header	*/
#define IPV6_VH		    0x45 	/* IP version and hdr length 	*/

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
    //byte    payload[1];
};

extern byte link_local[];
extern byte snm_addr[];
extern byte mac_snm[];
extern uint8 ipv6bootstrap;
extern byte allrmulti[];
extern byte allnmulti[];
