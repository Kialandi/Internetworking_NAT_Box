#include<xinu.h>
uint32 getIdentif();
uint16 checkLength(uint16 payload_len);
void fillPayload(char* payload, char * buffer, uint32 len);
char* fillUDPHeader(struct udp_header * udp_header, uint16 src_port, uint16 dest_port, uint16 udp_len, uint16 udp_checksum) ;
char*  fillPreFragmentHeader(char* pkt, byte* dest_ipv6, byte next_header);
char* fillFragmentHeader ( struct fragment_header * fragment_header ,uint32 identif, byte next_header, uint16 frag_offset, bool8 M_flag) ;


status sendto(byte* dest_ipv6, byte next_header, char* buffer, uint16 buf_len) {

// TODO:construct original packet first.  I am not sure whether it is necessary to construct original packet. The only concern is checksum in UDP or TCP is the checksum of  the fragmented packet or the checksum of the original packet.
	// check buf_len to decide whether fragment or not 
	kprintf("I am in sendto.......\n");
        uint8 pro_hdr_len = 0;
	switch(next_header) {
		case IPV6_TCP:
			pro_hdr_len = TCP_HDR_LEN;
			break;
		case IPV6_UDP:
			pro_hdr_len = UDP_HDR_LEN;
			break;	
	}
	kprintf("MTU: %d\n", MTU);
	//if (MTU > 0 && buf_len > MTU - IPV6_HDR_LEN - pro_hdr_len) { // 20 is IP header len
	if (TRUE) {
	        MTU = 1500; 				
		// send first fragment packet
		kprintf("I am in sending first fragment\n");
		uint32 identif = getIdentif();		
	 	uint16 udp_len = MTU - IPV6_HDR_LEN - sizeof(struct fragment_header);
		uint16 first_payload_len = checkLength(udp_len - UDP_HDR_LEN);
		sendFragment(dest_ipv6, buffer, first_payload_len, identif, 0, IPV6_UDP, TRUE);
		/*
		struct netpacket* pkt = (struct netpacket *) getbuf(ipv6bufpool); 
		memset(pkt, NULLCH, sizeof(struct netpacket));

		struct fragment_header * frag_header = (struct fragment_header *) fillPreFragmentHeader((char*) pkt, dest_ipv6, NEXT_HEADER_FRAGMENT);
		uint32 identif = getIdentif();
		struct udp_header* udp_header = (struct udp_header *) fillFragmentHeader(frag_header, identif, IPV6_UDP, 0, TRUE);
	 	uint16 udp_len = MTU - IPV6_HDR_LEN - sizeof(struct fragment_header);
		// TODO : fill UDP real data
		char* payload = fillUDPHeader(udp_header, 0, 0, udp_len, 0);
		uint16 first_payload_len = udp_len - UDP_HDR_LEN;
		fillPayload(payload, buffer, first_payload_len);
		// fire off the packet
		if( write(ETHER0, (char *) pkt, sizeof(struct netpacket)) == SYSERR) {
			kprintf("THE WORLD IS BURNING\n");
			kill(getpid());
		}
		*/
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
		
	
	} else {


		//sendPacket(dest_ipv6, next_header, buffer, buf_len, FALSE);
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

void sendFragment(byte* dest_ipv6, char* buffer, uint16 payload_len, uint32 identif, uint16 frag_offset, byte next_header_in_fragment_header, bool8 M_flag){
	
	// send first fragment packet
	struct netpacket* pkt = (struct netpacket *) getbuf(ipv6bufpool); 
	memset(pkt, NULLCH, sizeof(struct netpacket));

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
	/*
	srand(time(0));
	int32 res = rand();
	*/
	uint32 res = (uint32) rand();
	kprintf("identif: %d\n", res);
	return res;
}

// prefragment header includes Ethernet header and IPV6 header.
char*  fillPreFragmentHeader(char * pkt, byte* dest_ipv6, byte next_header) {
				
        // get mac of dest_ipv6

       // fill Ethenet header	
 	//fillEthernet((char *)pkt, if_tab[1].if_macbcast);
 	fillEthernet((char *)pkt, allrMACmulti);
       // fill IP header
        fillIPdatagram((char*)pkt + ETH_HDR_LEN, link_local, dest_ipv6, MTU - ETH_HDR_LEN -IPV6_HDR_LEN, next_header);
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
	fragment_header->frag = htons(frag_offset);
	kprintf("printing fragment header: \n");
	payload_hexdump((char*) fragment_header, 8); // for testing
	return (char*) fragment_header + sizeof(struct fragment_header);
		
}
