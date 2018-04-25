#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd printFWD(int nargs, char *args[]) {
	/* Check argument count */
	if (nargs != 1) {
		fprintf(stderr, "Usage: printfwd\n");
		return 1;
	}

    printFWDTab();

    return 0;
}
