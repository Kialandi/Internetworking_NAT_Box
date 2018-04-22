#include <xinu.h>
#include <stdio.h>


shellcmd xsh_sendpacket(int nargs, char * args[]) {
	// we have to wait DNS works to enable ipv6 address typed by user
	kprintf("send to .......");
	byte buffer[2000];
	memcpy((char*)buffer, "I am data\0", 10);
	sendto(allrIPmulti, IPV6_UDP, buffer, 2000);
	return 0;
}
