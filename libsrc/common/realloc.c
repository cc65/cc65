/*
 * realloc.c
 *
 * Ullrich von Bassewitz, 06.06.1998
 */



#include <stdlib.h>
#include <string.h>
#include "_heap.h"



void* realloc (void* block, size_t size)
{
    unsigned* b;
    unsigned* newblock;
    unsigned oldsize;
    int diff;

    /* Check the block parameter */
    if (!block) {
     	/* Block is NULL, same as malloc */
     	return malloc (size);
    }

    /* Check the size parameter */
    if (size == 0) {
     	/* Block is not NULL, but size is: free the block */
     	free (block);
	return 0;
    }

    /* Make the internal used size from the given size */
    size += HEAP_ADMIN_SPACE;
    if (size < sizeof (struct freeblock)) {
        size = sizeof (struct freeblock);
    }

    /* Get a pointer to the real block, get the old block size */
    b = (unsigned*) (((int) block) - 2);
    oldsize = *b;

    /* Get the size difference as a signed quantity */
    diff = size - oldsize;

    /* Is the block at the current heap top? */
    if (((int) b) + oldsize == ((int) _hptr)) {
    	/* Check if we've enough memory at the heap top */
    	int newhptr;
    	newhptr = ((int) _hptr) + diff;
    	if (newhptr <= ((int) _hend)) {
    	    /* Ok, there's space enough */
       	    _hptr = (unsigned*) newhptr;
    	    *b = size;
    	    return block;
    	}
    }

    /* The given block was not located on top of the heap, or there's no
     * room left. Try to allocate a new block and copy the data.
     */
    if (newblock = malloc (size)) {
     	memcpy (newblock, block, oldsize - 2);
     	free (block);
    }
    return newblock;
}









