/**
 * @file spinlock_util.S
 * @provides _lock_acquire _lock_release _atomic_increment
 *
 * Embedded Xinu, Copyright (C) 2018. All rights reserved.
 */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye; Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu; zach.thompson@marquette.edu]
 */

#include <spinlock.h>


	/** LAB NOTES ON ARM ASSEMBLY

	layout CMD var1, var2,...

	REGISTERS:
	registers r0-r3 are called non-volatile
	r0 is called the return variable
	non-volatile registers are the parameters

	volatile registers r4-r15 are temporary variables
	^^^SHOULD NOT NEED TO USE THESE FOR THIS ASSIGNMENT^^^

	COMMANDS:
//	mov r0, #5
	r0 = 5
	can also use predefined vars
//	mov r0, #SPINLOCK_LOCKED

//	cmp r0, r1
	compares r0 to r1
	if(r0 == r1)

//	beq _method
	method()
	runs if cmp command is (eq) equal

//	bne _method
	method()
	runs if cmp command is (ne) not equal

//	ldrex r1, [r0]
	(do not use ldr in this assignment, only ldrex)
	loads the address from r0 into r1
	for pointers!

//	strex r2, r1, [r0]
	r2 is the success register
	address in r1 is loaded into r0
	for pointers and memory addresses

	*/


/**
 * @fn void _lock_acquire(unsigned int *lock)
 *
 * Uses atomic operations to set a lock variable.
 *
 * @param lock 	pointer to lock variable
 */
.globl _lock_acquire
_lock_acquire:
	.func	_lock_acquire

	pldw	[r0]		/* Preload Data with intention to write		*/

	// TODO: Implement a spinlock acquire method by:
	//       First, use ldrex to obtain the value at the lock.
	//       Then, check if it is locked (SPINLOCK_LOCKED).
	//	 	If it is, then retry.
	//	 Second, use strex to try to set the lock to SPINLOCK_LOCKED.
	//	 	If strex fails, try from the top.

	//from lab:
	//ldrex will be first command
	//if locked then branch back to a certain point to retry until the lock is unlocked
	//if unlocked then store value in memory address and check if successful
	// end from lab

	//Noah's edits
	_retry: ldrex r1, [r0]
	cmp r1, #SPINLOCK_LOCKED
	beq _retry
	mov r1, #SPINLOCK_LOCKED
	strex r2, r1, [r0]
	cmp r2, #0
	bne _retry
	//end Noah's edits

	dmb			/* Data Memory Barrier	*/
	bx	lr

	.endfunc

/**
 * @fn void _lock_release(unsigned int *lock)
 *
 * Sets lock variable to unlocked.
 *
 * @param lock	pointer to lock variable
 */
.globl _lock_release
_lock_release:
	.func	_lock_release

	mov	r1, #SPINLOCK_UNLOCKED
	dmb			/* Required before accessing protected resource */
	str	r1, [r0]	/* Unlock lock */
	dmb
	bx	lr

	.endfunc


/**
 * @fn int _atomic_lock_check(unsigned int *)
 *
 * Tries to atomically check if a lock is available.
 *
 * @param address of a lock entries state.
 * @return 0 on success, -1 on failure.
 */
.globl _atomic_lock_check
_atomic_lock_check:
	.func _atomic_lock_check

	pldw	[r0]		/* Preload data w/ intent to write	*/
	ldrex	r1, [r0]	/* load exclusive the muxtab state 	*/
	cmp	r1, #SPINLOCK_FREE	/* IF state == SPINLOCK_FREE		*/
	beq	_increment	/* THEN increment to SPINLOCK_USED	*/
				/* OTHERWISE clrex and return 		*/
	clrex
	dmb
	mov	r0, #-1
	bx	lr

_increment:
	add	r1, r1, #1	/* increments to SPINLOCK_USED		*/
	strex	r2, r1, [r0]	/* strex SPINLOCK_USED			*/
	cmp	r2, #0x0	/* IF strex doesnt succeed,		*/
	bne	_atomic_lock_check	/* jump back to the top		*/
				/* OTHERWISE return as success		*/
	dmb
	mov	r0, #0
	bx	lr

	.endfunc
