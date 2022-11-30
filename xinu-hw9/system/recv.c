/**
 * @file recv.c
 * @provides recv.
 *
 * $Id: receive.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye; Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu; zach.thompson@marquette.edu]
 */

#include <xinu.h>

/**
 * recv - wait for a mesage and return it
 * @return message
 */

message recv(void)
{
	register pcb *ppcb;
	int senderpid = -1;
	message msg;
	ppcb = &proctab[currpid[getcpuid()]];
	
	/* TODO:
 	* - Remember to acquire lock and release lock when interacting with the msg structure
		- set spinlock and release on return
 	* - Check for Message. If no message, put in blocking state and reschedule
 	*   			If there is message, retrive message
		if (hasmsg = false, state_recv, lock release, resched(), acquire lock)
			msg = msgin
 	* - Now, check queue for wating send processes,
 	*   If stuff in queue collect message for next time recv is called and ready send process
		if (queue !empty, make variable for first dequeue, find struct holding message, set = to else in order to collect message)
			(msgout = empty, lock release, ready())
 	*   else, reset message flag
		else (set hasmsg = false, lock release)
 	*   return collected message
 	*/	
	
 	// Acquire lock
	lock_acquire(ppcb->msg_var.core_com_lock);

	// Check for message
	if (ppcb->msg_var.hasMessage == FALSE )	{
		ppcb->state = PRRECV; 						// put in blocking state
		lock_release(ppcb->msg_var.core_com_lock);  // release lock
		resched();									// resched
		lock_acquire(ppcb->msg_var.core_com_lock);	// Acquire lock
	}
	msg = ppcb->msg_var.msgin; 					// retrieve message

	// Check for more messages from queue
	senderpid = dequeue(ppcb->msg_var.msgqueue); 	// get sender PID

	if(senderpid != EMPTY)	{
		pcb *msgProc;
		msgProc = &proctab[senderpid]; 				// get sender pcb

		ppcb->msg_var.msgin = msgProc->msg_var.msgout; // deposit message
		ppcb->msg_var.hasMessage = TRUE; 			   // set message flag
		msgProc->msg_var.msgout = EMPTY;				   // remove proc
		
		lock_release(ppcb->msg_var.core_com_lock);			  // release lock
		ready(senderpid, RESCHED_NO, msgProc->core_affinity); // ready send proc
	}
	else	{
		ppcb->msg_var.hasMessage = FALSE; 			// reset message flag
		lock_release(ppcb->msg_var.core_com_lock);  // release lock
	}

	return msg;
}
