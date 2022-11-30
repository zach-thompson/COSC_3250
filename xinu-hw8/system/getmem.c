/**
 * @file getmem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate heap memory.
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the allocated memory region.
 *      The returned pointer is guaranteed to be 8-byte aligned.  Free the block
 *      with memfree() when done with it.
 */
void *getmem(ulong nbytes)
{
    register memblk *prev, *curr, *leftover;
    irqmask im;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (ulong)roundmb(nbytes);

    im = disable();

	/* TODO:
     *      - Use cpuid to use correct freelist
     *           ex: freelist[cpuid]
     *      - Acquire memory lock (memlock)
     *      - Traverse through the freelist
     *        to find a block that's suitable 
     *        (Use First Fit with simple compaction)
     *      - Release memory lock
     *      - return memory address if successful
     */

    //get memblocks
	uint cpuid = getcpuid();
    prev = (memblk *)&freelist[cpuid];
	curr = (memblk *)freelist[cpuid].head;
    
    //lock freelist
    lock_acquire(freelist[cpuid].memlock);

    //find memblock
    while(curr != NULL) {
		// If block is larger than nbytes, MOST LIKELY
		if(curr->length > nbytes)	{
			leftover = (memblk *)((ulong)curr + nbytes);	// Create new memblock
			leftover->next = curr->next;
			//curr->next = NULL;
			leftover->length = curr->length - nbytes;

			// Memblock element allocation
			prev->next = leftover;
			curr->length = nbytes;

			// Recalculate freelist length
			freelist[cpuid].length -= nbytes;

			lock_release(freelist[cpuid].memlock); // Release lock
			restore(im);	// MAYBE?
			return (void *)curr; // Return the address of the memblock			
		}
		// If block is exactly nbytes
		else if(curr->length == nbytes)	{
			prev->next = curr->next; // Remove block from freelist
			//curr->next = NULL;
			freelist[cpuid].length -= nbytes; // Resize freelist length
			
			lock_release(freelist[cpuid].memlock); // Release lock
			restore(im);	// MAYBE?
			return (void *)curr; // Return the address 
		}
		// If nbytes doesn't fit, continue traversal
		else	{
			prev = curr;
			curr = curr->next;
		}
    }

    lock_release(freelist[cpuid].memlock);
    restore(im);
    return (void *)SYSERR;
}
