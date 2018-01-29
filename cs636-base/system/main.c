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
    icmpv6 icmp;
    icmp.type = 0;
    icmp.code = 0;
    icmp.checksum = 0;
    char *testing = "Testing testing testing";
    memcpy(icmp.payload, testing, 24);
    uint32 val = checksumv6(&icmp, 1516);
    printf("\n checksum val : %d \n", val);


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
