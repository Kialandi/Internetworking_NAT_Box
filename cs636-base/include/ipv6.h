/* ipv6.h  -  Constants related to Internet Protocol version 6 (IPv6) */

#define	IPV6_ICMP		0x3A		/* ICMP protocol type for IP 	*/
#define	IPV6_UDP		0x11		/* UDP protocol type for IP 	*/
#define IPV6_TCP        0x06
#define TCP_HDR_LEN	20
#define IPV6_FRAG       0x2C
#define NEXT_HEADER_FRAGMENT 0X2C  /*44 : next header value of ipv6 header to indicate next header is fragment header*/
#define NEXT_HEADER_NONE 0x9E /* indicate real payload is right after this header, no next header any more */

#define IPV6_ADDRBITS   128
#define	IPV6_ASIZE	    16		/* Bytes in an IP address	*/
#define	IPV6_HDR_LEN	40		/* Bytes in an IP header	*/
#define IPV6_VH		    0x45 	/* IP version and hdr length 	*/

#define IPV6OUTNBUFS    40      /* number of buffers */
#define DATAGRAM_ASIZE  98312   
#define DATAGRAMNBUFS   10	/*number of buffers */ 

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
extern byte     ipv6_addr[];//your own global ip address
extern byte     link_local[];//your own link local IP address: FE80::\64 + EUI64(mac)
extern byte     link_local_mac[];//mac version of link local, 3333:<link local last 32>
extern byte     snm_addr[];//multicast group ff02::1:ff<last 24 from link local>
extern byte     mac_snm[];//3333:<mac last 32>
extern byte     allrMACmulti[];//All routers MAC
extern byte     allnMACmulti[];//All nodes MAC
extern byte     allrIPmulti[];//All routers IP
extern byte     allnIPmulti[];//All nodes IP
extern bpid32   ipv6bufpool;
extern bpid32	datagram_buf_pool;
extern byte     router_link_local[];//default router link local
extern byte     router_link_local_mac[];//default router transformed mac from link local
extern byte     router_ip_addr[];//default router global unicast
extern byte 	router_mac_addr[];//default router's MAC address
extern byte 	router_snm_addr[];//default router's SNM address
extern byte 	router_mac_snm[];//default router's MAC SNM address

extern uint32	MTU;
extern byte	prefix_default[];

struct prefix_ipv6 {
    byte ipv6[IPV6_ASIZE];
    uint8 prefix_length;
} prefix_ipv6;

extern struct prefix_ipv6 prefix_ipv6_default;

struct fragment_header {
	byte next_header;
	byte reserved;
	//uint13 frag_offset;
	uint16 offset; // 13 bits for frag_offset, 2 bites for res, 1 bit for M flag
	//byte M_flag;
	uint32 identif;		
};

struct udp_header{
	uint16 src_port;
	uint16 dest_port;
	uint16 udp_len;
	uint16 udp_checksum;

};

