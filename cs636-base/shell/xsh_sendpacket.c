#include <xinu.h>
#include <stdio.h>
#include <string.h>

shellcmd xsh_sendpacket(int nargs, char * args[]) {
	// we have to wait DNS works to enable ipv6 address typed by user
	if (nargs < 1 || (strcmp(args[1], "0") != 0 && strcmp(args[1], "1") != 0))  {
		kprintf("\nUsage: sendpacket <time out flag> :\ntime out flag : 1 , sleep to time out; otherwise 0\n");
		return 0;
	} 

	kprintf("send to .......");
	byte buffer[2000];
	memcpy((char*)buffer, "I am data\0", 10);
	bool8 timeout_flag; 
	if (strcmp(args[1], "0") == 0) {
		timeout_flag = FALSE;
	} else {

		timeout_flag = TRUE;
	}

	timeout_flag = ((strcmp(args[1], "0") == 0)? FALSE:TRUE);
	kprintf("timeout_flag: %u\n", timeout_flag);
	sendto(allrIPmulti, IPV6_UDP, buffer, 2000, timeout_flag);
	return 0;
}
