/*  main.c  - main */

#include <xinu.h>
#include <string.h>

process	main(void)
{

	/* Run the Xinu shell */

	recvclr();
    bootipv6();
    print_addr(link_local, IPV6_ASIZE);
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
