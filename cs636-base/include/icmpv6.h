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

//
//
// general structures and icmpv6 messages
//
// 


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

// reserved is the first 32 bits of message body
// after the first 32 bits, depending on the icmp type,
// additional information is inserted into the payload as the
// message body is continued.
//
// Depending on the type of message, the reserved portion is
// the only message body and payload contains options which
// may or may not be utilized.
//
// e.g. echo requests:
// reserved:
// 16 bits Identifier | 16 bits sequence number
// payload of arbitrary data
//
// uint16 *resptr = &reserved
// resptr = (uint16*) identifying value
// resptr++;
// resptr = (uint16*) sequencenumber
//
// -?
typedef struct icmpv6general{
    byte type;
    byte code;
    uint16 checksum;
    byte reserved[4];
    byte payload[MAX_PAYLOAD];
    icmpopt opt;
}icmpv6general;


//
//
// router solicitation and advertisement
//
// 
typedef struct rsolicit{
    byte type;
    byte code;
    uint16 checksum;
    uint32 reserved;
    //icmpopt opt;
}rsolicit;

typedef struct radvertisement{
    byte type; // 8 bits
    byte code; // 8 bits
    uint16 checksum; // 16 bits
    uint8 curhoplim; // 8 bit unsigned int
    uint16 M: 1; // one bit flag.
    uint16 O: 1; // one bit, flag .
    uint16 reserved: 6; // 6 bits initialized to zeros 
    uint16 routerlifetime; // 16  bit unsigned int
    uint32 reachabletime; // 32 bit unsigned int
    uint32 retranstimer; // 32 bit unsigned int
    // icmpopt opt;
}radvertisement;

//
//
//neighbor solicitation and advertisement
//
//

typedef struct nsolicit{
    byte type;
    byte code;
    uint16 checksum;
    uint32 reserved
    byte ipaddr[16];
    //icmpopt opt;
}nsolicit;

typedef struct nadvert{
    byte type;
    byte code;
    uint16 checksum;
    uint16 R: 1;
    uint16 S: 1;
    uint16 O: 1;
    uint16 res: 29;
    byte ipaddr[16];
    // icmpopt opt;
}nadvert;

//
//
//pseudoheaders
//
//

typedef struct pseudoHdr{
    byte    src[IPV6_ASIZE];
    byte    dest[IPV6_ASIZE];
    uint32  len;
    byte    zero[3];
    byte    next_header;
    byte    icmppayload[ICMPSIZE];
}pseudoHdr;


