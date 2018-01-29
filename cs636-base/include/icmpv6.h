#include <xinu.h>
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


#define MAX_PAYLOAD (64)*(1000)
#define DESTUNREACH 1
#define PKT_BIG     2
#define TIME_EX     3
#define PVT1        100
#define PVT2        100


/*solicitation*/

#define ROUTERS     133
#define ROUTERA     134
#define NEIGHBS     135
#define NEIGHBA     136

/*informational messages */
#define ECHOREQ     128
#define ECHOREP     129
#define PVT3        200
#define PVT4        201

typedef struct{
                         /*header*/
    uint32 type;
    uint32 code;
    uint32 checksum;
                         /*begin payload*/
    byte payload[MAX_PAYLOAD];
}icmpv6;

