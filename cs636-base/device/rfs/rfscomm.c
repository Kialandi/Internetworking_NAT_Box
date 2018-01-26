/* rfscomm.c - rfscomm */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rfscomm  -  Handle communication with RFS server (send request and
 *		receive a reply, including sequencing and retries)
 *------------------------------------------------------------------------
 */
int32	rfscomm (
	 struct	rf_msg_hdr *msg,	/* Message to send		*/
	 int32	mlen,			/* Message length		*/
	 struct	rf_msg_hdr *reply,	/* Buffer for reply		*/
	 int32	rlen			/* Size of reply buffer		*/
	)
{

	return SYSERR;

//NEED IP4//	int32	i;			/* Counts retries		*/
//NEED IP4//	int32	retval;			/* Return value			*/
//NEED IP4//	int32	seq;			/* Sequence for this exchange	*/
//NEED IP4//	int16	rtype;			/* Reply type in host byte order*/
//NEED IP4//	int32	slot;			/* UDP slot			*/
//NEED IP4//
//NEED IP4//	/* For the first time after reboot, register the server port */
//NEED IP4//
//NEED IP4//	if ( ! Rf_data.rf_registered ) {
//NEED IP4//		if ( (slot = udp_register(Rf_data.rf_ser_ip,
//NEED IP4//				Rf_data.rf_ser_port,
//NEED IP4//				Rf_data.rf_loc_port)) == SYSERR) {
//NEED IP4//			return SYSERR;
//NEED IP4//		}
//NEED IP4//		Rf_data.rf_udp_slot = slot;
//NEED IP4//		Rf_data.rf_registered = TRUE;
//NEED IP4//	}
//NEED IP4//
//NEED IP4//	/* Assign message next sequence number */
//NEED IP4//
//NEED IP4//	seq = Rf_data.rf_seq++;
//NEED IP4//	msg->rf_seq = htonl(seq);
//NEED IP4//
//NEED IP4//	/* Repeat RF_RETRIES times: send message and receive reply */
//NEED IP4//
//NEED IP4//	for (i=0; i<RF_RETRIES; i++) {
//NEED IP4//
//NEED IP4//		/* Send a copy of the message */
//NEED IP4//
//NEED IP4//		retval = udp_send(Rf_data.rf_udp_slot, (char *)msg,
//NEED IP4//			mlen);
//NEED IP4//		if (retval == SYSERR) {
//NEED IP4//			kprintf("Cannot send to remote file server\n");
//NEED IP4//			return SYSERR;
//NEED IP4//		}
//NEED IP4//
//NEED IP4//		/* Receive a reply */
//NEED IP4//
//NEED IP4//		retval = udp_recv(Rf_data.rf_udp_slot, (char *)reply,
//NEED IP4//			rlen, RF_TIMEOUT);
//NEED IP4//
//NEED IP4//		if (retval == TIMEOUT) {
//NEED IP4//			continue;
//NEED IP4//		} else if (retval == SYSERR) {
//NEED IP4//			kprintf("Error reading remote file reply\n");
//NEED IP4//			return SYSERR;
//NEED IP4//		}
//NEED IP4//
//NEED IP4//		/* Verify that sequence in reply matches request */
//NEED IP4//
//NEED IP4//		if (ntohl(reply->rf_seq) != seq) {
//NEED IP4//			continue;
//NEED IP4//		}
//NEED IP4//
//NEED IP4//		/* Verify the type in the reply matches the request */
//NEED IP4//
//NEED IP4//		rtype = ntohs(reply->rf_type);
//NEED IP4//		if (rtype != ( ntohs(msg->rf_type) | RF_MSG_RESPONSE) ) {
//NEED IP4//			continue;
//NEED IP4//		}
//NEED IP4//
//NEED IP4//		return retval;		/* Return length to caller */
//NEED IP4//	}
//NEED IP4//
//NEED IP4//	/* Retries exhausted without success */
//NEED IP4//
//NEED IP4//	kprintf("Timeout on exchange with remote file server\n");
//NEED IP4//	return TIMEOUT;
}
