#include <xinu.h>

void  get_link_local(byte* mac) {
	byte res[16];
	memset(res, 0, 8); // for 64 bits prefix
	res[0] = 0xFE;
	res[1] = 0x80;
	// insert 0xFFEE in the middle of mac addr
	byte  temp[8];
	memset(temp, NULLCH, 8);
	memcpy(temp, mac, 3);
	temp[3] = 0xFF;
	temp[4] = 0xEE;
	memcpy(temp + 5, mac + 3, 3);
	//printf("temp before flip: 0x%x", temp);
	printf("before flip:\n");
	print_addr(temp, 8);
	// flip the 7th bit
	temp[0] = temp[0] ^ 0x02;
	printf("temp after flip:\n");
	print_addr(temp, 8);
	printf("test\n");
	memcpy(res + 8, temp, 8);

	printf("res test:\n");
	print_addr(res, 16);
	memcpy(link_local, res, 16);
}

void get_snm_addr(byte* unicast){
	byte res[16];
	memset(res, 0, 16);
	// 104 bit prefix :FF02:0::01:FF
	res[0] = 0xFF;
	res[1] = 0x02;
	res[11] = 0x01;
	res[12] = 0xFF;
	// last 24 bit of unicast addr
	memcpy(res + 13, unicast + 13, 3);
	printf("res in get_snm_addr:\n");
	print_addr(res, 16);
	memcpy(snm_addr, res, 16);
}

void get_mac_snm(byte* snm_addr, byte* mac_snm) {
	byte res[6];
	memset(res, 0, 6);
	res[0] = 0x33;
	res[1] = 0x33;
	memcpy(res + 2, snm_addr + 12, 4);
	printf("res in get_mac_snm:\n");
	print_addr(res, 6);
	memcpy(mac_snm, res, 6);
}

void print_addr(byte* addr, int32 len){
	int i;
	for (i = 0; i < len; i++) {
		printf("0x%02x:", addr[i]);
	}
	printf("\n");
}
void    ipv6_in (
		struct ipv6datagram * packet
		)
{
	//    print6();
	return;
}

