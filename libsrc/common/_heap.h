/*
 * _heap.h
 *
 * Ullrich von Bassewitz, 03.06.1998
 *
 */



#ifndef __HEAP_H
#define __HEAP_H



/* Space needed for administering used blocks */
#define HEAP_ADMIN_SPACE        sizeof (unsigned)

/* The data type used to implement the free list. 
 * Beware: Field order is significant!
 */
struct freeblock {
    unsigned 		size;
    struct freeblock*  	next;
    struct freeblock*  	prev;
};



/* Variables that describe the heap */
extern unsigned*       	  _horg;       	/* Bottom of heap */
extern unsigned*  	  _hptr;	/* Current top */
extern unsigned*  	  _hend;	/* Upper limit */
extern struct freeblock*  _hfirst;	/* First free block in list */
extern struct freeblock*  _hlast;	/* Last free block in list */



/* End of _heap.h */

#endif



