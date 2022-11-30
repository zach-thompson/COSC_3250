//1. create.c: atomic increment - done
//      use getmem instead of getstk
//2. getmem() - implement first
//3. freemem() - implement second
//4. malloc(), free() - implement last ("about 5 lines each" - sabirat)
//5. testcases.c

/**
 * @file free.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup libxc
 *
 * Attempt to free a block of memory based on malloc() accounting information
 * stored in preceding two words.
 *
 * @param ptr
 *      A pointer to the memory block to free.
 */
syscall free(void *ptr)
{
    struct memblock *block;
    //block--;
	/* TODO:
     *      1) set block to point to memblock to be free'd (ptr)
     *      2) find accounting information of the memblock
     *      3) call freemem on the block with its length
     */
    //freemem(): deallocates memory and puts memory back in freelist
    
    //  SET BLOCK TO POINT TO MEMBLOCK TO BE FREED
    block = (memblk *)ptr;

    //  FIND ACCOUNTING INFO OF MEMBLOCK
    //block = (memblk *)((ulong)block - sizeof(memblk));
    block--;
    //check err
	if(block->next != block)	{
		return SYSERR;
	}

    //  CALL FREEMEM ON BLOCK W/ ITS LENGTH
    freemem(block, block->length);

    return OK;
}
