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
//    printf("CHECKSUM: %d\n", (uint16) cksum);
	return (uint16) cksum;
}

bool8 cksum_valid(void * phead, void * pkt, uint32 len, uint32 pheadlen) {
    uint16 a = checksumv6(pkt, len);
    uint16 b = checksumv6(phead, pheadlen);
    if (!(0xffff & ~(a + b))){
        //kprintf("cksum_valid: returning true\n");
        return TRUE;
    }
    else{
        //kprintf("cksum_valid: returning false\n");
        return FALSE;
    }
}
    //TODO: validate checksum
