/* xsh_sp.c - xsh_sp */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_sp - shell command to send one packet
 *
 *  use:     sp [interface_number]
 *------------------------------------------------------------------------
 */
shellcmd xsh_sp(int nargs, char *args[])
{
	char	ch;			/* Character form of args[1]	*/
	int32	iface;			/* Interface to use		*/
	struct	netpacket pkt;		/* Packet to send		*/

	/* For argument '--help', emit help about the 'sp' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s [iface [dst_mac]]\n\n", args[0]);
		printf("Description:\n");
		printf("\tsp sends on packet, either on the specified\n");
		printf("\t    interface or the primary interface, if\n");
		printf("\t    non is specified.  The optional dst\n");
		printf("\t    is a destination MAC address; the interface\n");
		printf("\t    broadcast address is used if no destination\n");
		printf("\t    is specified\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if ( nargs > 2 ) {
		fprintf(stderr, "%s: invalid number of arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if (nargs == 2) {

		/* Parse argument */

		ch = args[1][0];
		if ((strlen(args[1]) != 1) || (ch < '0') || (ch > '2')) {
		    fprintf(stderr, "%s:  %s", args[0], args[1]);
		    fprintf(stderr, "is not a valid interface number\n");
		    return 1;
		}
		iface = ch - '0';
	} else {
		/* Use primary interface */
		iface = ifprime;
	}

	/* Handcraft and send a packet that has				*/
	/*	Ethernet type	= 0x0885				*/
	/*	destination	= Interface's MAC broadcast address	*/
	/*	source		= Interface's MAC unicast address	*/

	memset((char *)&pkt, 0, 1500);
	strcpy((char *)&pkt.net_payload, " Please ignore this packet");
	pkt.net_type = htons(0x0885);
	memcpy(pkt.net_dst, if_tab[iface].if_macbcast, ETH_ADDR_LEN);
	memcpy(pkt.net_src, if_tab[iface].if_macucast, ETH_ADDR_LEN);

	printf("Sending packet:\n");
	printf("   type        %04x\n", ntohs(pkt.net_type));
	printf("   source      %02x:%02x:%02x:%02x:%02x:%02x\n", 
		0xff&pkt.net_src[0], 0xff&pkt.net_src[1], 0xff&pkt.net_src[2],
		0xff&pkt.net_src[3], 0xff&pkt.net_src[4], 0xff&pkt.net_src[5]);
	printf("   destination %02x:%02x:%02x:%02x:%02x:%02x\n", 
		0xff&pkt.net_dst[0], 0xff&pkt.net_dst[1], 0xff&pkt.net_dst[2],
		0xff&pkt.net_dst[3], 0xff&pkt.net_dst[4], 0xff&pkt.net_dst[5]);

	write(ETHER0, (char *)&pkt, 1500);

	return 0;
}
