/* xsh_netinfo.c - xsh_netinfo */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_netinfo - obtain and print the IP address, subnet mask, default
 *			router address and other network information
 *------------------------------------------------------------------------
 */

shellcmd xsh_netinfo(int nargs, char *args[]) {

	struct	ifentry	*ifptr;		/* Ptr to interface entry	*/
	int32	iface;			/* Iterate through interfaces	*/

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: no arguments allowed\n", args[0]);
		return 1;
	}

	/* Print interface info */

	printf("\n");

	for (iface=0; iface<NIFACES; iface++) {
		ifptr = &if_tab[iface];
		printf("Interface %s:  unicast= ", ifptr->if_name);
		printf("%02x:%02x:%02x:%02x:%02x:%02x  bcast= ",
			ifptr->if_macucast[0] &0xff, ifptr->if_macucast[1] &0xff,
			ifptr->if_macucast[2] &0xff, ifptr->if_macucast[3] &0xff, 
			ifptr->if_macucast[4] &0xff, ifptr->if_macucast[5] &0xff);
		printf("%02x:%02x:%02x:%02x:%02x:%02x\n\n",
			ifptr->if_macbcast[0] &0xff, ifptr->if_macbcast[1] &0xff,
			ifptr->if_macbcast[2] &0xff, ifptr->if_macbcast[3] &0xff, 
			ifptr->if_macbcast[4] &0xff, ifptr->if_macbcast[5] &0xff);
	}
	printf("\n");
	return 0;
}
