/*
 * free.c
 *
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <stdlib.h>
#include "_heap.h"



void free (void* block)
/* Release an allocated memory block. The function will accept NULL pointers
 * (and do nothing in this case).
 */
{
    unsigned* b;
    unsigned size;
    struct freeblock* f;


    /* Allow NULL arguments */
    if (block == 0) {
        return;
    }

    /* Get a pointer to the real memory block, then get the size */
    b = (unsigned*) block;
    size = *--b;

    /* Check if the block is at the top of the heap */
    if (((int) b) + size == (int) _hptr) {

        /* Decrease _hptr to release the block */
        _hptr = (unsigned*) (((int) _hptr) - size);

        /* Check if the last block in the freelist is now at heap top. If so,
         * remove this block from the freelist.
         */
        if (f = _hlast) {
            if (((int) f) + f->size == (int) _hptr) {
                /* Remove the last block */
                _hptr = (unsigned*) (((int) _hptr) - f->size);
                if (_hlast = f->prev) {
	       	    /* Block before is now last block */
                    f->prev->next = 0;
                } else {
                    /* The freelist is empty now */
                    _hfirst = 0;
                }
            }
        }

    } else {

       	/* Not at heap top, enter the block into the free list */
     	_hadd (b, size);

    }
}



