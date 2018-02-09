#include <xinu.h>

/*
 * ptr increment in 1 byte  = 16 bits. each increment
 * provides one  bit segment.
 *
 * additionally, the one's complement sum is commutative
 * as per RFC 1624
 *
 */


// check to see if host or netbyte order is backward

uint32 checksumv6(void * message, uint32 messagelength){
    kprintf("len in checksumv6: %d\n", messagelength);
    char* datagram = (char*) message;
    uint32 checksum = 0;
    for (int32 i = 0; i+1<messagelength; i+=2){
        uint32 segment1;
        uint32 segment2;
        memcpy(&segment1, datagram+i, 2);
        memcpy(&segment2, datagram+(i+1), 2);
        checksum = ntohs(segment1+segment2); // network to host
        // carry over
        // greater than 2^16
        if (checksum > 0xffff)
            checksum -=0xffff;
    }

    if (messagelength % 16 != 0){
        uint32 remainder;
        memcpy(&remainder, datagram-messagelength, 1);
        checksum += ntohs(remainder);
        if (checksum >0xffff)
            checksum -=0xffff;
        }

    return ~checksum; //host to network
}

