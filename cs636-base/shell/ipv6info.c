
#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd ipv6info(int nargs, char *args[]) {

	/* Check argument count */

	if (nargs > 1) {
		fprintf(stderr, "%s: no arguments allowed\n", args[0]);
		return 1;
	}

    print_ipv6_info();

    return 0;
}
