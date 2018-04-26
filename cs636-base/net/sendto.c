#include<xinu.h>
#include<stdlib.h>

uint32 getIdentif();
uint16 checkLength(uint16 payload_len);
void writeToInterface(char*  pkt);
void fillPayload(char* payload, char * buffer, uint32 len);
void fillPerFragmentHeader(struct Datagram * datagram, char* pkt, uint16 payload_len_ipv6_header);
//void fragmentDatagram();
void fragmentDatagram(struct Datagram * datagram, bool8 timeout_flag);
char* fillFragmentHeader ( struct fragment_header * fragment_header ,uint32 identif, byte next_header, uint16 frag_offset, bool8 M_flag) ;
void sendFragment(struct Datagram * datagram, uint16 payload_len_ipv6_header, uint16 payload_len, uint32 identif, uint16 frag_offset, byte next_header_in_fragment_header, bool8 M_flag);

char* fillUDPHeader(struct udp_header * udp_header, uint16 src_port, uint16 dest_port, uint16 udp_len, uint16 udp_checksum) ;
char*  fillPreFragmentHeader(char* pkt, byte* dest_ipv6, byte next_header);


status sendto(byte* dest_ipv6, byte next_header, byte * buffer, uint16 buf_len, bool8 timeout_flag) {

//	kprintf("I am in sendto.......\n");
// The blow is for considering UDP/TCP, but it should be heandled in buffer before comming into this function.
//        uint8 pro_hdr_len = 0;
//	switch(next_header) {
//		case IPV6_TCP:
//			pro_hdr_len = TCP_HDR_LEN;
//			break;
//		case IPV6_UDP:
//			pro_hdr_len = UDP_HDR_LEN;
//			break;	
//	}
	kprintf("MTU: %d\n", MTU);

	struct netpacket netpkt;
	struct base_header * ipv6_header = (struct base_header *) netpkt.net_payload;
	fillIPdatagram(&netpkt, link_local, dest_ipv6, buf_len, next_header);  // assuming buffer already includes upper layer header 
	kprintf("after filling ipv6 header before fragment:\n");
	payload_hexdump((char *) ipv6_header, IPV6_HDR_LEN);

	//struct Datagram * datagram = (struct Datagram *) getbuf( datagram_buf_pool);
	struct Datagram * datagram = (struct Datagram *) getmem(sizeof(struct Datagram));
	fillDatagram(datagram, (byte *) ipv6_header, IPV6_HDR_LEN, buffer, buf_len);

        fragmentDatagram(datagram, timeout_flag);
	//if (MTU > 0 && buf_len > MTU - IPV6_HDR_LEN - pro_hdr_len) { // 20 is IP header len
	/*
	if (TRUE) {
	        MTU = 1500; 				
		// send first fragment packet
		kprintf("I am in sending first fragment\n");
		uint32 identif = getIdentif();		
	 	uint16 udp_len = MTU - IPV6_HDR_LEN - sizeof(struct fragment_header);
		uint16 first_payload_len = checkLength(udp_len - UDP_HDR_LEN);
		sendFragment(dest_ipv6, buffer, first_payload_len, identif, 0, IPV6_UDP, TRUE);
                kprintf("send subsequence fragment packet\n");
	
		uint16 remain_len = buf_len - first_payload_len;
		uint16 max_payload = MTU - IPV6_HDR_LEN - sizeof(struct fragment_header); // max payload for one packet
		uint16 frag_offset = first_payload_len;
		bool8 M_flag = TRUE;
		while(remain_len > 0) {
			uint16 payload_len = (remain_len > max_payload) ? max_payload : remain_len;		
			payload_len = checkLength(payload_len);
			remain_len -= payload_len;
			if (remain_len == 0) {
				M_flag = FALSE;
				kprintf("M_flag is FALSE\n");
			} 

			if (M_flag)	kprintf("M_flag is TRUE\n");
						
			//sendPacket(dest_ipv6, next_header, buffer, payload_len, TRUE);
			sendFragment(dest_ipv6, buffer, payload_len, identif, frag_offset, NEXT_HEADER_NONE, M_flag);
			frag_offset += payload_len;
		}	
		
	


		//sendPacket(dest_ipv6, next_header, buffer, buf_len, FALSE);
	} 
 */ 
    freemem((char*)datagram, sizeof(struct Datagram));
    return OK;

} 

void fragmentDatagram(struct Datagram * datagram, bool8 timeout_flag){
//	MTU = 1500;// change it later
	kprintf("MTU: %d\n", MTU);
	if (datagram->headers_len + datagram->payload_len > MTU) {
		// fragment the datagram into multiple packet
		 
		uint32 identif = getIdentif();		
		uint16 frag_offset = 0;
 		byte M_flag = TRUE;
		// get next_header value in fragment header
		struct base_header * ipv6_header = (struct base_header *) (datagram->headers);
		byte next_header_in_fragment_header = ipv6_header->next_header;
		kprintf("next header in fragment header: %x\n", next_header_in_fragment_header);
		
		uint16 remain_len = datagram->payload_len;
		uint16 max_payload_len = MTU - datagram->headers_len - sizeof(struct fragment_header);   // the length of fragement from paylaad of datagram
		max_payload_len = checkLength(max_payload_len); // check length in multiple of 8 octets.
		kprintf("datagram headers len: %d\n", datagram->headers_len);
		kprintf("datagram payload len:%d\n", datagram->payload_len);
		
		while(remain_len > 0) {
			uint16 payload_len = (remain_len > max_payload_len) ? max_payload_len : remain_len;		
			kprintf("payload_len: %d\n", payload_len);
			if (payload_len == remain_len) {  // if it is last packet, check length in multiple of 8 octet again.
				payload_len = checkLength(payload_len);
			}
			uint16 payload_len_ipv6_header = sizeof(struct fragment_header) + payload_len;		
			remain_len -= payload_len;
			if (remain_len == 0) {
				M_flag = FALSE;
				kprintf("M_flag is FALSE\n");
			} 

			if (M_flag)	kprintf("M_flag is TRUE\n");
						
			//sendPacket(dest_ipv6, next_header, buffer, payload_len, TRUE);
			sendFragment(datagram, payload_len_ipv6_header, payload_len, identif, frag_offset, next_header_in_fragment_header, M_flag);
			frag_offset += payload_len;
			if (timeout_flag == TRUE) {
				sleep(40);
			}
		}	

 	} else {

		// send the packet directly

		// copy datagram to packet

		// fill Ethernet
	
		// write to interface
	}
	



}

void writeToInterface(char*  pkt){

	kprintf("sending fragments\n");
	print6(pkt);
	if( write(ETHER0, (char *) pkt, sizeof(struct netpacket)) == SYSERR) {
		kprintf("THE WORLD IS BURNING\n");
		kill(getpid());
	}

}
// payload len has to be in the unit of 8 octets
uint16 checkLength(uint16 payload_len){
	/*
	if (payload_len > 8) {
		uint16 remainder = payload_len % 8;
		return payload_len - remainder;
        }else {
		return payload_len;
	}
	*/
	return (payload_len > 8) ? payload_len - (payload_len % 8) : payload_len;
}

void sendFragment(struct Datagram * datagram, uint16 payload_len_ipv6_header, uint16 payload_len, uint32 identif, uint16 frag_offset, byte next_header_in_fragment_header, bool8 M_flag){
	
	// send first fragment packet
	struct netpacket* pkt = (struct netpacket *) getbuf(ipv6bufpool); 
	memset((char*) pkt, NULLCH, sizeof(struct netpacket));


	// copy headers since currently we only have ipv6 base header
	fillPerFragmentHeader(datagram, (char *)pkt + ETH_HDR_LEN, payload_len_ipv6_header);

	// insert fragment head	
	struct fragment_header * frag_header = (struct fragment_header *) ( (char*) pkt + ETH_HDR_LEN + datagram->headers_len); 
	char * payload = fillFragmentHeader(frag_header, identif, next_header_in_fragment_header, frag_offset, M_flag);

	// copy payload
	fillPayload(payload, (char * ) datagram->payload + frag_offset, payload_len);
	
        kprintf("in sendto : print payload: \n");
	payload_hexdump(payload, 32);
        //TODO: get mac of dest_ipv6	
	struct base_header* ipv6_header = (struct base_header *) (datagram->headers);
	kprintf("printing dest address\n");
	payload_hexdump(ipv6_header->dest, IPV6_ASIZE);
    	struct NDCacheEntry * entry = lookupNDEntry(ipv6_header->dest);
	if (entry == NULL) {
		kprintf("lookupNDEntry return NULL\n");
		sendnsolicit(ipv6_header->src);
		entry = lookupNDEntry(ipv6_header->src);
	} 
	if (entry == NULL) {
		kprintf("sendnsolicit does not work. no mac addr in neighbor cache table, so not sending anything.\n");
		return;
	}	
	// fillEthnet header
  	
	fillEthernet(pkt, entry->macAddr, ETH_HDR_LEN); // send to netbox for now
	kprintf("filling Ethernet header:\n");
	payload_hexdump((char*)pkt, ETH_HDR_LEN);
	// write to interface
	writeToInterface((char * )pkt);

/*
	struct fragment_header * frag_header = (struct fragment_header *) fillPreFragmentHeader((char*) pkt, dest_ipv6, NEXT_HEADER_FRAGMENT);
	
	char * temp = fillFragmentHeader(frag_header, identif, next_header_in_fragment_header, frag_offset, M_flag);
	char * payload = temp; // later on change temp to payload 
	if (next_header_in_fragment_header == IPV6_UDP) {
		struct udp_header * udp_header = (struct udp_header *) temp; 
		uint16 udp_len = MTU - IPV6_HDR_LEN - sizeof(struct fragment_header);
		// TODO : fill UDP real data
		payload = fillUDPHeader(udp_header, 0, 0, udp_len, 0);
	}

	//uint16 first_payload_len = udp_len - UDP_HDR_LEN;
	fillPayload(payload, buffer + frag_offset, payload_len);
	// fire off the packet
	if( write(ETHER0, (char *) pkt, sizeof(struct netpacket)) == SYSERR) {
		kprintf("THE WORLD IS BURNING\n");
		kill(getpid());
	}
*/

}
void fillPerFragmentHeader(struct Datagram * datagram, char* pkt, uint16 payload_len_ipv6_header) {


	memcpy(pkt, datagram->headers, datagram->headers_len);
	// update payload lengh field and next header field
	struct base_header * ipv6_header = (struct base_header *) pkt;
	ipv6_header->payload_len = htons(payload_len_ipv6_header);
	ipv6_header->next_header = NEXT_HEADER_FRAGMENT;
	kprintf("filling perfregment header:\n");
	payload_hexdump(pkt, IPV6_HDR_LEN);

}

void fillPayload(char* payload, char * buffer, uint32 len) {

	memcpy(payload, buffer, len);

}

char* fillUDPHeader(struct udp_header * udp_header, uint16 src_port, uint16 dest_port, uint16 udp_len, uint16 udp_checksum) {
	udp_header->src_port = src_port;
	udp_header->dest_port = dest_port;
	udp_header->udp_len = udp_len;
	udp_header->udp_checksum = udp_checksum;
	return (char*) udp_header + sizeof(struct udp_header);
}

uint32 getIdentif(){
	
	srand(getticks());
	int32 res = rand();
	
	/*
	uint32 res = (uint32) rand();
	*/
	kprintf("identif: %u\n", (uint32) res);
	return (uint32) res;
}

// prefragment header includes Ethernet header and IPV6 header.
char*  fillPreFragmentHeader(char * pkt, byte* dest_ipv6, byte next_header) {
				
        // get mac of dest_ipv6

       // fill Ethenet header	
 	//fillEthernet((char *)pkt, if_tab[1].if_macbcast);
	fillEthernet((struct netpacket *) pkt, allrMACmulti, ifprime);
       // fill IP header
        fillIPdatagram((struct netpacket *) pkt, link_local, dest_ipv6, MTU - ETH_HDR_LEN -IPV6_HDR_LEN, next_header);
	kprintf("======printing preFragment  header: =========\n");
	payload_hexdump((char*) pkt, ETH_HDR_LEN + IPV6_HDR_LEN);
	return (char*) pkt + ETH_HDR_LEN + IPV6_HDR_LEN;	
}

char*  fillFragmentHeader ( struct fragment_header * fragment_header ,uint32 identif, byte next_header, uint16 frag_offset, bool8 M_flag) {

	fragment_header->next_header = next_header;
//	fragment_header->frag_offset = frag_offset;
	fragment_header->identif = htonl(identif);
	// modify last 3 bits of frag_offset
	kprintf("frag_offset:%x\n", frag_offset);
	frag_offset = frag_offset & 0xFFF8;
	if (M_flag) {
		// set the last bit to be 1
		frag_offset = frag_offset | 0x0001;	
	}
	kprintf("frag_offset:%x\n", frag_offset);
	fragment_header->offset = htons(frag_offset);
	kprintf("printing fragment header: \n");
	payload_hexdump((char*) fragment_header, 8); // for testing
	return (char*) fragment_header + sizeof(struct fragment_header);
		
}
