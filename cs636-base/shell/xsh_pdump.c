/* xsh_pdump.c - xsh_pdump */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

extern	int32	packetdump;

/*------------------------------------------------------------------------
 * xsh_pdump - shell command to control packet dump
 *------------------------------------------------------------------------
 */
shellcmd xsh_pdump(int nargs, char *args[])
{
	/* For argument '--help', emit help about the 'pdump' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s  level (level is off|on|all\n\n", args[0]);
		printf("Description:\n");
		printf("\tUse pdump to control the level of packet dump\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs != 2) {
		fprintf(stderr, "%s: invalid arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if (strcmp(args[1], "off") == 0) {
		packetdump = 0;
		return 0;
	} else if (strcmp(args[1], "on") == 0) {
		packetdump = 1;
		return 0;
	} else if (strcmp(args[1], "all") == 0) {
		packetdump = 2;
		return 0;
	}
	fprintf(stderr, "%s: argument %s is invalid\n", args[0], args[1]);
	fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
	return 1;
}
