/*
 * malloc.c
 *
 * Ullrich von Bassewitz, 03.06.1998
 */



#include <stddef.h>
#include "_heap.h"



void* malloc (size_t size)
/* Allocate memory from the given heap. The function returns a pointer to the
 * allocated memory block or a NULL pointer if not enough memory is available.
 * Allocating a zero size block is not allowed.
 */
{
    struct freeblock* f;
    unsigned* p;


    /* Check for a size of zero, then add the administration space and round
     * up the size if needed.
     */
    if (size == 0) {
	return 0;
    }
    size += HEAP_ADMIN_SPACE;
    if (size < sizeof (struct freeblock)) {
        size = sizeof (struct freeblock);
    }

    /* Search the freelist for a block that is big enough */
    f = _hfirst;
    while (f && f->size < size) {
        f = f->next;
    }

    /* Did we find one? */
    if (f) {

        /* We found a block big enough. If the block can hold just the
         * requested size, use the block in full. Beware: When slicing blocks,
         * there must be space enough to create a new one! If this is not the
         * case, then use the complete block.
         */
        if (f->size - size < sizeof (struct freeblock)) {

            /* Use the actual size */
            size = f->size;

            /* Remove the block from the free list */
            if (f->prev) {
                /* We have a previous block */
                f->prev->next = f->next;
            } else {
                /* This is the first block, correct the freelist pointer */
                _hfirst = f->next;
            }
            if (f->next) {
                /* We have a next block */
                f->next->prev = f->prev;
            } else {
                /* This is the last block, correct the freelist pointer */
                _hlast = f->prev;
            }

        } else {

            /* We must slice the block found. Cut off space from the upper
	     * end, so we can leave the actual free block chain intact.
	     */

	    /* Decrement the size of the block */
	    f->size -= size;

	    /* Set f to the now unused space above the current block */
	    f = (struct freeblock*) (((unsigned) f) + f->size);

        }

        /* Setup the pointer for the block */
        p = (unsigned*) f;

    } else {

        /* We did not find a block big enough. Try to use new space from the
         * heap top.
         */
	if (((unsigned) _hend) - ((unsigned) _hptr) < size) {
            /* Out of heap space */
            return 0;
    	}


	/* There is enough space left, take it from the heap top */
	p = _hptr;
       	_hptr = (unsigned*) (((unsigned) _hptr) + size);

    }

    /* New block is now in p. Fill in the size and return the user pointer */
    *p++ = size;
    return p;
}



