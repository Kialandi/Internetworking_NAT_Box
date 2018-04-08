#include <xinu.h>
#include <stdio.h>


shellcmd xsh_sendpacket(int nargs, char * args[]) {
	kprintf("send to .......");
	char buffer[2000];
	sendto(allrIPmulti, IPV6_UDP, buffer, 2000);
	return 0;
}
