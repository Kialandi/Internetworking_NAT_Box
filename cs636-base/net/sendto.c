#include<xinu.h>
uint32 getIdentif();
void fillPayload(char* payload, char * buffer, uint32 len);
char* fillUDPHeader(struct udp_header * udp_header, uint16 src_port, uint16 dest_port, uint16 udp_len, uint16 udp_checksum) ;
char*  fillPreFragmentHeader(char* pkt, byte* dest_ipv6, byte next_header);
char* fillFragmentHeader ( struct fragment_header * fragment_header ,uint32 identif, byte next_header, uint16 frag_offset, bool8 M_flag) ;


status sendto(byte* dest_ipv6, byte next_header, char* buffer, uint32 buf_len) {

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
		// send first fragment packet
		kprintf("I am in sending first fragment\n");
		struct netpacket* pkt = (struct netpacket *) getbuf(ipv6bufpool); 
		memset(pkt, NULLCH, sizeof(struct netpacket));

		struct fragment_header * frag_header = (struct fragment_header *) fillPreFragmentHeader((char*) pkt, dest_ipv6, NEXT_HEADER_FRAGMENT);
		int32 identif = getIdentif();
		struct udp_header* udp_header = (struct udp_header *) fillFragmentHeader(frag_header, identif, IPV6_UDP, 0, TRUE);
	 	uint32 udp_len = MTU - IPV6_HDR_LEN - sizeof(struct fragment_header);
		// TODO : fill UDP real data
		char* payload = fillUDPHeader(udp_header, 0, 0, udp_len, 0);
		fillPayload(payload, buffer, udp_len - UDP_HDR_LEN);
		// fire off the packet
		if( write(ETHER0, (char *) pkt, sizeof(struct netpacket)) == SYSERR) {
			kprintf("THE WORLD IS BURNING\n");
			kill(getpid());
		}
                // send subsequence fragment packet
		/*	
		uint32 temp_len = buf_len;
		uint32 max_payload = MTU - IPV6_HDR_LEN - pro_hdr_len; // max payload for one packet
		while(temp_len > 0) {
			uint32 payload_len = (temp_len > max_payload) ? max_payload : temp_len;
			sendPacket(dest_ipv6, next_header, buffer, payload_len, TRUE);
			temp_len -= payload_len;
		}	
		*/	

	} else {


		//sendPacket(dest_ipv6, next_header, buffer, buf_len, FALSE);
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
	uint32 res = 1800;
	kprintf("identif: %d\n", res);
	return res;
}


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
