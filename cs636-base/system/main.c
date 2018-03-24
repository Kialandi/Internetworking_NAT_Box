/*  main.c  - main */

#include <xinu.h>
#include <string.h>

process	main(void)
{
	/* Run the Xinu shell */

	recvclr();
    sleep(1);
    ipv6_init();
    sleep(1);
    
    kprintf("Number of interfaces: %d\n", NIFACES);
    int i =0;
    for (i = 0; i < NIFACES; i++) {
        kprintf("IFACE name; %s, state: %d\n", if_tab[i].if_name, if_tab[i].if_state);
    }
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
