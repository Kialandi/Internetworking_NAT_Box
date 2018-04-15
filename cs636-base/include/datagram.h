
#define DATAGRAM_SIZE	98304  /* DATAGRAM_SIZE = PAYLOAD_SIZE + HEADERS_SIZE */
#define PAYLOAD_SIZE	65536
#define HEADERS_SIZE	32768 

/*
struct Datagram_info{
//	byte* headers_start;
	uint16 headers_len;
//	byte* payload_start;
	uint16 payload_len;
};
*/
struct Datagram	{
	uint16 headers_len;
	uint16 payload_len;
	byte headers[HEADERS_SIZE];
	byte payload[PAYLOAD_SIZE];
};

//extern struct Datagram datagram;
//extern struct Datagram_info datagram_info;

