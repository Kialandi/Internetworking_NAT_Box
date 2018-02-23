#include <xinu.h>

/*
 * ptr increment in 1 byte  = 16 bits. each increment
 * provides one  bit segment.
 *
 * additionally, the one's complement sum is commutative
 * as per RFC 1624
 *
 */

uint32 checksumv6(void * pkt, uint32 messagelength){
    uint16 i;
    uint32 cksum = 0;
    uint16 * ptr16 = (uint16 *) pkt;
    uint16 ck;

	for(i = 0; i < messagelength; i = i + 2) {
        ck = (uint32) *ptr16;
        cksum += ntohs(ck);
		ptr16++;
	}

    cksum = cksum + (cksum >> 16);
    cksum = 0xffff & ~cksum;

	return (uint16) cksum;
}

bool8 cksum_valid(void * pkt, uint32 len) {
    //TODO: validate checksum
    return TRUE;
}
