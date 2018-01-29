#include <xinu.h>

byte link_local[IPV6_ASIZE];
byte link_local[IPV6_ASIZE];
byte mac_snm[ETH_ADDR_LEN];
uint8 ipv6bootstrap = 0;

void    ipv6_in (
        struct netpacket * pkt
    )
{
    print6(pkt);
    freebuf((char *) pkt);
    return;
}

