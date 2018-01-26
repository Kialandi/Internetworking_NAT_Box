/* net.h */

#define NETSTK		8192 		/* Stack size for network setup */
#define NETPRIO		500    		/* Network startup priority 	*/

/* Constants used in the networking code */

#define	ETH_ARP     0x0806		/* Ethernet type for ARP	*/
#define	ETH_IP      0x0800		/* Ethernet type for IP		*/
#define	ETH_IPv6    0x86DD		/* Ethernet type for IPv6	*/

#define	OTH_ADDR_LEN	ETH_ADDR_LEN	/* Othernet address size	*/

/* Format of an Ethernet or Othernet packet */

#pragma pack(2)
struct	netpacket	{
	byte	net_dst[ETH_ADDR_LEN];	/* Destination MAC address	*/
	byte	net_src[ETH_ADDR_LEN];	/* Source MAC address		*/
	uint16	net_type;		/* Layer 2 type field		*/
	byte	net_payload[1500];	/* Packet payload		*/
	uint32	net_ethcrc;		/* Ethernet CRC			*/
	int16	net_iface;		/* Interface over which the	*/
					/*   packet arrived (placed	*/
					/*   after the actual packet so	*/
					/*   we can pass the packet	*/
					/*   address directly to write	*/
};
#pragma pack()

#define	PACKLEN	sizeof(struct netpacket)

extern	bpid32	netbufpool;		/* ID of net packet buffer pool	*/



/* Definintions for network interfaces (used by IP) */

#define	NIFACES	3		/* Number of interfaces -- one for the	*/
				/*   Ethernet plus two "othernets"	*/

/* Interface state definitions */

#define	IF_UP		1	/* Interface is currently on line	*/
#define	IF_DOWN		0	/* Interface is currently offline	*/

#define	IF_QUEUESIZE	20	/* Size of the incoming packet queue	*/
				/*   for each interface			*/

#define	IF_NLEN		32	/* Max characters in an interface name	*/

/* Network interface structure */

struct	ifentry	{
	char	if_name[IF_NLEN];/* Name for the interface		*/
	bool8	if_state;	/* Interface is either up or down	*/
	did32	if_dev;		/* Device ID of the layer2 device used	*/
				/*    with this interface		*/
	byte	if_macucast[ETH_ADDR_LEN]; /* MAC unicast address	*/
	byte	if_macbcast[ETH_ADDR_LEN]; /* MAC broadcast address	*/
	sid32	if_sem;		/* semaphore counts incoming packets	*/
				/*    in the queue			*/
	struct	netpacket *if_queue[IF_QUEUESIZE]; /* queue to hold	*/
				/*  incoming packets for this interface	*/
	int32	if_head;	/* next entry in packet queue to remove	*/
	int32	if_tail;	/* next slot in packet queue to insert	*/

	/* *** NOTE ADDITIONAL IPv4/IPv6/ARP/ND fields go here *** */
};

extern	struct	ifentry	if_tab[];

extern	int32	ifprime;	/* Primary interface.  For a host, the	*/
				/*   only interface that's up, for a	*/
				/*   nat box, interface 0.  -1 means	*/
				/*   no network is active.		*/
extern	bool8	host;		/* TRUE if this node is running as a	*/
				/*   host; FALSE if acting as a NAT box	*/
extern	int32	bingid;		/* User's bing ID			*/
