
#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd ipv6info(int nargs, char *args[]) {

	/* Check argument count */

	if (nargs > 1) {
		fprintf(stderr, "%s: no arguments allowed\n", args[0]);
		return 1;
	}

	/* Print ipv6 info */

	printf("\n");
    kprintf("Printing prefix from router\n");
    print_ipv6_addr(prefix_ipv6_default.ipv6);
    kprintf("Prefix len: %d\n", prefix_ipv6_default.prefix_length);
    kprintf("Default interface: %d\n", ifprime);

	return 0;
}
