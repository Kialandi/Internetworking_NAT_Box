/*  main.c  - main */

#include <xinu.h>
#include <string.h>

typedef struct{
                         /*header*/
    uint32 type;
    uint32 code;
    uint32 checksum;
                         /*begin payload*/
    byte payload[1500];
}icmpv6;

process	main(void)
{

	/* Run the Xinu shell */

	recvclr();

    byte * ptr = (byte *) getmem(8);
    memset(ptr, NULLCH, 8);
    ptr[0] = 0x86;
    ptr[1] = 0x5E;
    ptr[2] = 0xAC;
    ptr[3] = 0x60;
    ptr[4] = 0x71;
    ptr[5] = 0x2A;
    ptr[6] = 0x81;
    ptr[7] = 0xB5;
    uint16 ans = checksumv6(ptr, 8);
    kprintf("checksum: %d\n", ans);
    freemem((char *)ptr, 8);
    //bootipv6();
    //print_addr(link_local, IPV6_ASIZE);
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
