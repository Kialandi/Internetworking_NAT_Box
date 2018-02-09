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
    char* datagram = (char*) message;
    if (messagelength % 2 != 0) { kprintf("odd len\n"); return 0; }
    
    uint32 checksum = 0;
    uint32 segment1;
    for (int32 i = 0; i + 1 < messagelength; i += 2){

        memcpy(&segment1, datagram + i, 2);
        checksum += ntohs(segment1); // network to host
        // carry over
        // greater than 2^16
    }
    //TODO: account for odd length

    //TODO: overflow
    checksum += (checksum >> 16);
    checksum = 0xffff & checksum;

    return (uint16) (0xffff & ~checksum); //host to network
}

