/**
 * @file malloc.c
 * This file is deprecated in favor of mem/malloc.c and the user heap
 * allocator.  However, it remains here for backup purposes.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup libxc
 *
 * Request heap storage, record accounting information, returning pointer
 * to assigned memory region.
 *
 * @param size
 *      number of bytes requested
 *
 * @return
 *      pointer to region on success, or @c NULL on failure
 */
void *malloc(ulong size)
{
    struct memblock *pmem;

    /* we don't allocate 0 bytes. */
    if (0 == size)
    {
        return NULL;
    }

    //size+=size of accounting block (sizeof(memblk))
    //pmem=getmem(size)=allocates memory from freelist
	/** TODO:
      *      1) Make room for accounting info
      *      2) Acquire memory with getmem
      *         - handle possible error (SYSERR) from getmem...
      *      3) Set accounting info in pmem
      */  
    //pmem->next and length
    
    //make room for accounting info
    size = size + sizeof(memblk);

    //acquire memory with getmem
    pmem = (memblk *)getmem(size);
    if( (uint)pmem == SYSERR)	{ // Error check
		return NULL;
	}

    //set accounting info in pmem
    pmem->next = pmem;  //  MAYBE???
	pmem->length = size;

    return (void *)(pmem + 1);  /* +1 to skip accounting info */
}
