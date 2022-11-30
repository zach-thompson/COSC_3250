/**
 * @file send.c
 * @provides send
 *
 * $Id: send.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  Al rights resered. */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye; Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu; zach.thompson@marquette.edu]
 */

#include <xinu.h>

/**
 * Send a message to another therad
 * @param pid proc id of recipient
 * @param msg contents of message
 * @return OK on sucess, SYSERR on failure
 */
syscall send(int pid, message msg)
{
	register pcb *spcb;
	register pcb *rpcb;
	
	/*TODO:
 	* - PID Error checking (check if pid is good)
 	* - Acquire receving process lock (remember to release when appropriate!) lock_release(same parameter)
 	* - Retrieving Process Error Checking (check if in PRFREE - lock_release and syserr)
 	* - If receiving process has message, block sending process and put msg in msgout and call resched
			check hasmsg, if true, save msg, change state, block sendmsg, send, call resched()
 	* - Else, deposit message, change message flag and, if receiving process is blocking, ready it.
			save msg, change hasmsg to true, check if in precv, lock release, ready() else just lock release
			// will be using stuff from sendnow() in this loop
 	* - return ok.
 	*/

	// Set pcbs
	rpcb = &proctab[pid];
	spcb = &proctab[currpid[getcpuid()]];

 	// PID Error checking
	if (isbadpid(pid))	{
		return SYSERR;
	}

	// Acquire receiving process lock
	lock_acquire(rpcb->msg_var.core_com_lock);

	//set values of pcbs
	if (rpcb->state == PRFREE) {					// check state
		lock_release(rpcb->msg_var.core_com_lock);	// release lock
		return SYSERR;								// return error
	}

	if (rpcb->msg_var.hasMessage == TRUE) {			// check for msg
		spcb->state = PRSEND; 						// block sending process
		spcb->msg_var.msgout = msg; 				// put msg into msgout
		
		enqueue(currpid[getcpuid()], rpcb->msg_var.msgqueue); // Put proc into rpcb queue
		lock_release(rpcb->msg_var.core_com_lock);			  // release lock
		resched();										      // resched proc
		
		return OK;
	}
	else {
		rpcb->msg_var.msgin = msg;							// Deposit message
		rpcb->msg_var.hasMessage = TRUE;					// set message flag
		if (rpcb->state == PRRECV) {						// check state
			lock_release(rpcb->msg_var.core_com_lock);
			ready(pid, RESCHED_YES, rpcb->core_affinity);	// ready proc
		}else {
			lock_release(rpcb->msg_var.core_com_lock);		// release lock
		}

		return OK;		
	}
}
