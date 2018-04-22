#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd printND(int nargs, char *args[]) {
	/* Check argument count */
	if (nargs != 1) {
		fprintf(stderr, "Usage: printndcache\n");
		return 1;
	}

    printNDTab();

    return 0;
}
