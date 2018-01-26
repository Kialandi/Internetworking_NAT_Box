/* rdscomm.c - rdscomm */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdscomm  -  handle communication with a remote disk server (send a
 *		 request and receive a reply, including sequencing and
 *		 retries)
 *------------------------------------------------------------------------
 */
status	rdscomm (
	  struct rd_msg_hdr *msg,	/* Message to send		*/
	  int32		    mlen,	/* Message length		*/
	  struct rd_msg_hdr *reply,	/* Buffer for reply		*/
	  int32		    rlen,	/* Size of reply buffer		*/
	  struct rdscblk    *rdptr	/* Ptr to device control block	*/
	)
{

	return SYSERR;

//NEED IP4//	intmask		mask;		/* Saved interrupt mask		*/
//NEED IP4//	int32		i;		/* Counts retries		*/
//NEED IP4//	int32		retval;		/* Return value			*/
//NEED IP4//	int32		seq;		/* Sequence for this exchange	*/
//NEED IP4//	uint32		localip;	/* Local IP address		*/
//NEED IP4//	int16		rtype;		/* Reply type in host byte order*/
//NEED IP4//	bool8		xmit;		/* Should we transmit again?	*/
//NEED IP4//	int32		slot;		/* UDP slot			*/
//NEED IP4//
//NEED IP4//	/* Disable interrupts while testing status */
//NEED IP4//
//NEED IP4//	mask = disable();
//NEED IP4//
//NEED IP4//	/* Register the server port, if not registered */
//NEED IP4//
//NEED IP4//	if ( ! rdptr->rd_registered ) {
//NEED IP4//		slot = udp_register(0, rdptr->rd_ser_port,
//NEED IP4//						rdptr->rd_loc_port);
//NEED IP4//		if(slot == SYSERR) {
//NEED IP4//			restore(mask);
//NEED IP4//			return SYSERR;
//NEED IP4//		}
//NEED IP4//		rdptr->rd_udpslot = slot;
//NEED IP4//		rdptr->rd_registered = TRUE;
//NEED IP4//	}
//NEED IP4//
//NEED IP4//	/* Get the local IP address */
//NEED IP4//
//NEED IP4//	if ( NetData.ipvalid == FALSE ) {
//NEED IP4//		localip = getlocalip();
//NEED IP4//		if((int32)localip == SYSERR) {
//NEED IP4//			restore(mask);
//NEED IP4//			return SYSERR;
//NEED IP4//		}
//NEED IP4//	}
//NEED IP4//	restore(mask);
//NEED IP4//
//NEED IP4//	/* Retrieve the saved UDP slot number  */
//NEED IP4//
//NEED IP4//	slot = rdptr->rd_udpslot;
//NEED IP4//
//NEED IP4//	/* Assign message next sequence number */
//NEED IP4//
//NEED IP4//	seq = rdptr->rd_seq++;
//NEED IP4//	msg->rd_seq = htonl(seq);
//NEED IP4//
//NEED IP4//	/* Repeat RD_RETRIES times: send message and receive reply */
//NEED IP4//
//NEED IP4//	xmit = TRUE;
//NEED IP4//	for (i=0; i<RD_RETRIES; i++) {
//NEED IP4//	    if (xmit) {
//NEED IP4//
//NEED IP4//		/* Send a copy of the message */
//NEED IP4//
//NEED IP4//		retval = udp_sendto(slot, rdptr->rd_ser_ip, rdptr->rd_ser_port,
//NEED IP4//					(char *)msg, mlen);
//NEED IP4//		if (retval == SYSERR) {
//NEED IP4//			kprintf("Cannot send to remote disk server\n\r");
//NEED IP4//			return SYSERR;
//NEED IP4//		}
//NEED IP4//	    } else {
//NEED IP4//		xmit = TRUE;
//NEED IP4//	    }
//NEED IP4//
//NEED IP4//	    /* Receive a reply */
//NEED IP4//
//NEED IP4//	    retval = udp_recv(slot, (char *)reply, rlen,
//NEED IP4//						RD_TIMEOUT);
//NEED IP4//
//NEED IP4//	    if (retval == TIMEOUT) {
//NEED IP4//		continue;
//NEED IP4//	    } else if (retval == SYSERR) {
//NEED IP4//		kprintf("Error reading remote disk reply\n\r");
//NEED IP4//		return SYSERR;
//NEED IP4//	    }
//NEED IP4//
//NEED IP4//	    /* Verify that sequence in reply matches request */
//NEED IP4//
//NEED IP4//		
//NEED IP4//	    if (ntohl(reply->rd_seq) < seq) {
//NEED IP4//		xmit = FALSE;
//NEED IP4//	    } else if (ntohl(reply->rd_seq) != seq) {
//NEED IP4//			continue;
//NEED IP4//	    }
//NEED IP4//
//NEED IP4//	    /* Verify the type in the reply matches the request */
//NEED IP4//
//NEED IP4//	    rtype = ntohs(reply->rd_type);
//NEED IP4//	    if (rtype != ( ntohs(msg->rd_type) | RD_MSG_RESPONSE) ) {
//NEED IP4//		continue;
//NEED IP4//	    }
//NEED IP4//
//NEED IP4//	    /* Check the status */
//NEED IP4//
//NEED IP4//	    if (ntohs(reply->rd_status) != 0) {
//NEED IP4//		return SYSERR;
//NEED IP4//	    }
//NEED IP4//
//NEED IP4//	    return OK;
//NEED IP4//	}
//NEED IP4//
//NEED IP4//	/* Retries exhausted without success */
//NEED IP4//
//NEED IP4//	kprintf("Timeout on exchange with remote disk server\n\r");
//NEED IP4//	return TIMEOUT;
}
