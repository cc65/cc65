/*
 * _hadd.c
 *
 * Ullrich von Bassewitz, 19.06.1998
 */



#include <stddef.h>
#include "_heap.h"



void _hadd (void* mem, size_t size)
/* Add an arbitrary memory block to the heap. This function is used by
 * free(), but it does also allow usage of otherwise unused memory
 * blocks as heap space. The given block is entered in the free list
 * without any checks, so beware!
 */
{
    struct freeblock* f;
    struct freeblock* left;
    struct freeblock* right;

    if (size >= sizeof (struct freeblock)) {

    	/* Set the admin data */
    	f = (struct freeblock*) mem;
    	f->size = size;

    	/* Check if the freelist is empty */
    	if (_hfirst == 0) {

    	    /* The freelist is empty until now, insert the block */
    	    f->prev = 0;
    	    f->next = 0;
    	    _hfirst = f;
    	    _hlast  = f;

    	} else {

    	    /* We have to search the free list. As we are doing so, we check
    	     * if it is possible to combine this block with another already
    	     * existing block. Beware: The block may be the "missing link"
             * between *two* other blocks.
    	     */
    	    left = 0;
    	    right = _hfirst;
    	    while (right && f > right) {
    		left = right;
    		right = right->next;
    	    }


    	    /* Ok, the current block must be inserted between left and right (but
    	     * beware: one of the two may be zero!). Also check for the condition
    	     * that we have to merge two or three blocks.
    	     */
    	    if (right) {
    		/* Check if we must merge the block with the right one */
    		if (((int) f) + size == (int) right) {
    		    /* Merge with the right block */
    		    f->size += right->size;
    		    if (f->next = right->next) {
       			f->next->prev = f;
    		    } else {
    			/* This is now the last block */
    			_hlast = f;
    		    }
    		} else {
    		    /* No merge, just set the link */
    		    f->next = right;
    		    right->prev = f;
    		}
    	    } else {
    		f->next = 0;
    		/* Special case: This is the new freelist end */
    		_hlast = f;
    	    }
    	    if (left) {
    		/* Check if we must merge the block with the left one */
    		if ((int) f == ((int) left) + left->size) {
    		    /* Merge with the left block */
    		    left->size += f->size;
    		    if (left->next = f->next) {
    			left->next->prev = left;
    		    } else {
    			/* This is now the last block */
    			_hlast = left;
    		    }
    		} else {
    		    /* No merge, just set the link */
    		    left->next = f;
    		    f->prev = left;
    		}
    	    } else {
    		f->prev = 0;
    		/* Special case: This is the new freelist start */
    		_hfirst = f;
    	    }
	}
    }
}



