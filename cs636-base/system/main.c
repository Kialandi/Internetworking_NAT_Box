/*  main.c  - main */

#include <xinu.h>
#include <string.h>

process	main(void)
{

	/* Run the Xinu shell */

	recvclr();

    kprintf("in print6\n");
    kprintf("netpacket size: %d\n", sizeof(struct netpacket));
    kprintf("net_dst: \n");
    kprintf("net_src: \n");
    //kprintf("net_type: %d\n", pkt->net_type);
    //kprintf("net_ethcrc: \n", pkt->net_ethcrc);
    //kprintf("net_iface: \n", pkt->net_iface);
    
    kprintf("Printing IPv6 base header\n");

    //struct base_header * ip_datagram = (base_header *)pkt->net_payload;
    
    kprintf("info: \n");
    kprintf("payload_len: \n");
    kprintf("next_header: \n");
    kprintf("hop_limit: \n");
    kprintf("ip_src: \n");
    kprintf("ip_dest: \n");

	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}
