#include <xinu.h>
#include <stdio.h>
#include <string.h>

struct Datagram datagram;
struct Datagram_info datagram_info;

void  fillDatagram(byte* headers, uint16 headers_len, byte*  payload, uint16 payload_len) {
	memcpy(&datagram.headers, headers, headers_len);
	memcpy(&datagram.payload, payload, payload_len);	
	datagram_info.headers_len = headers_len;	
	datagram_info.payload_len = payload_len;
	kprintf("in fill Datagram:\n");
	payload_hexdump((char*) &datagram.headers, datagram_info.headers_len);
	kprintf("headers len: %d\n", datagram_info.headers_len);
	kprintf("payload len: %d\n", datagram_info.payload_len);
	kprintf("ending fill Datagram\n");
//	return datagram;
}

