#include <xinu.h>

/*
 * ptr increment in  16 bits. each increment
 * provides one  bit segment.
 *
 * additionally, the one's complement sum is commutative
 * as per RFC 1624
 *
 */


// check to see if host or netbyte order is backward

uint32 checksumv6(struct rsolicit * pkt, uint32 messagelength){
    struct pseudoHdr pseudo;
    memset(&pseudo, NULLCH, PSEUDOLEN);
    memcpy(&pseudo.src, link_local, IPV6_ASIZE);

    pseudo.dest[0] = 0xff;
    pseudo.dest[1] = 0x02;
    pseudo.dest[15] = 0x02;
    pseudo.len = htonl(ICMPSIZE); //not sure about this, ask dylan
    pseudo.next_header = IPV6_ICMP;

    memcpy(&pseudo.icmppayload, pkt, ICMPSIZE);
    hexdump(&pseudo, PSEUDOLEN);

    if (messagelength % 2 != 0) { kprintf("odd len\n"); return 0; }
    uint16 i;
    uint32 cksum = 0;
    uint16 * ptr16 = (uint16 *) &pseudo;
    //uint16 * ptr16 = (uint16 *) pkt;
    uint16 ck;

	/* First add all shorts in the pseudo header */
    for(i = 0; i < messagelength; i = i + 2) {
	    ck = (uint32) *ptr16;
	    cksum += ntohs(ck);
	    ptr16++;
    }
    cksum = cksum + (cksum >> 16);
    cksum = 0xffff & ~cksum;
    return (uint16) (0xffff & ~cksum);
}

