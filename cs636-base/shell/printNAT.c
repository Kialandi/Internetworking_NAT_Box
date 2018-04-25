#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd printNAT(int nargs, char *args[]) {
	/* Check argument count */
	if (nargs != 1) {
		fprintf(stderr, "Usage: printnat\n");
		return 1;
	}

    printNATTab();

    return 0;
}
