/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye; Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu; zach.thompson@marquette.edu]
 */

#include <arm.h>
#include <xinu.h>

/* Assembly routine for atomic operations */
extern int _atomic_increment(int *);
extern int _atomic_increment_mod(int *, int);

static pid_typ newpid(void);
void userret(void);
//void *getstk(ulong);

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @param ... 	   potential arguments of funaddr function
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, ulong priority, char *name, ulong nargs, ...)
{
	ulong *saddr;               /* stack address                */
	ulong pid;                  /* stores new process id        */
	pcb *ppcb;                  /* pointer to proc control blk  */
	ulong i;
	va_list ap;                 /* points to list of var args   */
	ulong pads = 0;             /* padding entries in record.   */
	void INITRET(void);

	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (ulong)(ssize + 3) & 0xFFFFFFFC;
	/* round up to even boundary    */
	 saddr = (ulong *)getmem(ssize);
	// saddr = (ulong *)getstk(ssize);     /* allocate new stack and pid   */
	pid = newpid();
	/* a little error checking      */
	if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
	{
		return SYSERR;
	}

	_atomic_increment(&numproc);

	ppcb = &proctab[pid];
	/* setup PCB entry for new proc */
	ppcb->state = PRSUSP;

	// TODO: Setup PCB (Process Control Block) entry for new process. [4 lines of code] (FROM HW5)
	ppcb->priority = priority;
	ppcb->stkbase = (void *)saddr; // stack base to stack address, maybe cast (void *)
	ppcb->stklen = ssize; // stack length
	ppcb->core_affinity = -1; // set core to -1
	strncpy(ppcb->name, name, strlen(name)); // set the name provided (char* name) using strncpy() = strncpy(ppcb->name, name)

	// Initialize stack with accounting block
	saddr = ((ulong)saddr)+ssize-sizeof(memblk);
	*saddr = STACKMAGIC; 						// indicates that it's a stack, defined constant number
	*--saddr = pid;		 						// processor id
	*--saddr = ppcb->stklen; 					// stack length
	*--saddr = (ulong)ppcb->stkbase;			// stack base

	/* Handle variable number of arguments passed to starting function   */
	if (nargs)
	{
		pads = ((nargs - 1) / 4) * 4;
	}
	/* If more than 4 args, pad record size to multiple of native memory */
	/*  transfer size.  Reserve space for extra args                     */
	for (i = 0; i < pads; i++)
	{
		*--saddr = 0;
	}

	// if 4 parameters, save in regs[]
	// if >4, save excess in stack (LIFO)


	// TODO: Initialize process context.
	// Using registers 13-15, only 3 lines of code?
	ppcb->regs[PREG_SP] = (int) *saddr;
	ppcb->regs[PREG_LR] = (int) userret;
	ppcb->regs[PREG_PC] = (int) funcaddr;

	/* TODO:  Place arguments into activation record.
	          See K&R 7.3 for example using va_start, va_arg and va_end macros for variable argument functions */

	// va_start in first line
	// 2 instances of va_arg
	// va_end as last line
	// 2 for loops: args 1-4 in regs, args 5-8 in stack
	va_start(ap, nargs);
	//put args into regs[0-3]
	for(i=0; i < nargs; i++)
	{
		if(i < 4) {
			ppcb->regs[i] = va_arg(ap,int);
		}else {
			saddr[i-4] = va_arg(ap,int);
		}
	}
	va_end(ap);

	ppcb->regs[PREG_SP] = (int)saddr;
	return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
	pid_typ pid;                /* process id to return     */
	static pid_typ nextpid = 0;

	for (pid = 0; pid < NPROC; pid++)
	{                           /* check all NPROC slots    */
		//        nextpid = (nextpid + 1) % NPROC;
		_atomic_increment_mod(&nextpid, NPROC);
		if (PRFREE == proctab[nextpid].state)
		{
			return nextpid;
		}
	}
	return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
	uint cpuid = getcpuid();
	kill(currpid[cpuid]);
}
