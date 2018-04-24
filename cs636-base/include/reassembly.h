// net.h, ipv6.h should be defined before this file

#define REASSEMBLY_TABLE_MAX_SIZE 16

struct frag_desc {
	uint16 offset;
	byte* payload; // the payload following after fragment header
	uint16 payload_len; // real payload length excluding the fragment header
	struct frag_desc * prev;
	struct frag_desc * next;
};

struct reassembly_entry {
	byte src_addr[IPV6_ASIZE]; // IVP6 src addr
	byte dest_addr[IPV6_ASIZE]; // ipv6 dest addr
	uint32 identif;  // identification in ipv6 header	
	byte perfrag_headers[80];  // perfragment headers max length, only copy the first header
	uint16 headers_len; // perfragment headers real length
	struct frag_desc * frag_list;  // a list of frag_desc 
	byte next_header_in_fragment_header;  // save next header in fragment header to replace fragment header value 44 (or 0x2c) in perfrag_headers
	uint32 timestamp;  // time stamp in seconds of the first fragment coming in
};


extern struct reassembly_entry  reassembly_table[];
extern uint8 reassembly_tab_size; 
