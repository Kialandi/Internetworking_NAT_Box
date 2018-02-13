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
    
    byte * ptr = (byte *) getmem(4);
    ptr[0] = 0x00;
    ptr[1] = 0xff;
    ptr[2] = 0x00;
    ptr[3] = 0xff;

    uint16 ret = checksumv6((void *) ptr, 4);
    kprintf("Ret = %d , supposed to be 65025\n", ret, ~ret);
    //sleep(1);
    //bootipv6();
    //print_addr(link_local, IPV6_ASIZE);
//    resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */
/*
    	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}*/
	return OK;

}
