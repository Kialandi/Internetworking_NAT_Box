/* icmpv6.h - definintions for the Internet Control Message Protocol */

/*
 * must be preceded by an ipv6 extension header
 *
 * The structure for an ICMP message:
 *
 * The IPv6 header contains source and destination
 * information as well as information for an extension
 * header. This extension header contains the code, 58.
 * The IP header functions as the pseudo header for the
 * ICMPv6 header. The header for ICMPv6 contains the type,
 * code, and checksum. The payload then follows.
 *
 * Source: RFC 4443. Pg 3.
 * RFC 4861 pg 18-18.
 *
 * */

// check icmpsize
// make correct size
// network byte order
//
#define  IPLEN 16

#define PSEUDOLEN  sizeof(pseudoHdr) 
#define ICMPSIZE    sizeof(rsolicit)
#define MAX_PAYLOAD 1472
#define DESTUNREACH 1
#define PKT_BIG     2
#define TIME_EX     3
#define PVT1        100
#define PVT2        100


/*solicitation*/

#define ROUTERS     0x85    //translates to 133
#define ROUTERA     134
#define NEIGHBS     135
#define NEIGHBA     136

/*informational messages */
#define ECHOREQ     128
#define ECHOREP     129
#define PVT3        200
#define PVT4        201

typedef struct idk{//TODO: Ask dylan what to name this
    uint32 type;
    uint32 length;
    uint32 prefixlength;
    uint32 L:1;
    uint32 A:1;
    uint32 res:6;
    uint32 validlifetime;
    uint32 preferredlifetime;
    byte res2[8];
    byte prefix[16];
}idk;

typedef struct lladdress{
    uint32 type;
    uint32 length;
    byte address[16];
}lladdress;

typedef struct icmpopt{
    uint32 type;
    uint32 length;
    byte icmpopt[16];//length];TODO: what's length here dylan?
}icmpopt;

typedef struct icmpv6general{
    uint32 type;
    uint32 code;
    uint16 checksum;
    byte reserved[16];//TODO: what's length here dylan?
    byte payload[MAX_PAYLOAD];
    icmpopt opt;
}icmpv6general;

typedef struct rsolicit{
    byte type;
    byte code;
    uint16 checksum;
    uint32 reserved;
    //icmpopt opt;
}rsolicit;

typedef struct radvertisement{
    uint32 type;
    uint32 code;
    uint16 checksum;
    uint32  curhoplim;
    uint32 M: 1;
    uint32 O: 1;
    uint32 reserved: 6;
    uint32 routerlifetime;
    uint32 reachabletime;
    uint32 retranstimer;
    icmpopt opt;
}radvertisement;


typedef struct nsolicit{
    uint32 type;
    uint32 code;
    uint16 checksum;
    byte reserved[4];
    byte ipaddr[16];
    icmpopt opt;
}nsolicit;

typedef struct nadvert{
    uint32 type;
    uint32 code;
    uint16 checksum;
    uint32 R: 1;
    uint32 S: 1;
    uint32 O: 1;
    uint32 res: 29;
    byte ipaddr[16];
    icmpopt opt;
}nadvert;


typedef struct pseudoHdr{
    byte    src[IPV6_ASIZE];
    byte    dest[IPV6_ASIZE];
    uint32  len;
    byte    zero[3];
    byte    next_header;
    byte    icmppayload[ICMPSIZE];
}pseudoHdr;


