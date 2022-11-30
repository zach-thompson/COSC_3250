/**
 * @file recvnow.c
 * @provides recvnow
 *
 */
/* Embedded Xinu, Copyright (C) 2020.  All rights resered. */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye; Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu; zach.thompson@marquette.edu]
 */

#include <xinu.h>

/**
 * recvnow - return pre-stored message, or immediate error.
 * @return message or SYSERR
 */

message recvnow(void)
{
	register pcb *ppcb;
	message msg;

	ppcb = &proctab[currpid[getcpuid()]];
	
	/* TODO:
 	* - Acquire and release lock when working in msg structure
 	* - check for message, if no messsage, error
 	*   		       else, retrieve & return message
			once read message, set hasmsgg to false
 	*/

 	lock_acquire(ppcb->msg_var.core_com_lock);		// acquire lock

	if (ppcb->msg_var.hasMessage == FALSE) {		// check for msg
		lock_release(ppcb->msg_var.core_com_lock);	// release lock	
		return SYSERR;								// return error
	}
	else {
		msg = ppcb->msg_var.msgin;					// retrieve msg
		ppcb->msg_var.hasMessage = FALSE;			// reset flag	
		lock_release(ppcb->msg_var.core_com_lock);	// release lock
	}

	return msg;										// return msg
}
