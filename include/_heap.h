/*
** _heap.h
**
** Ullrich von Bassewitz, 1998-06-03, 2004-12-19
**
*/



#ifndef __HEAP_H
#define __HEAP_H



/* Structure that preceeds a user block in most cases.
** The aligned_malloc function may generate blocks where the start pointer
** and size are splitted to handle a memory hole that is needed for
** alignment.
*/
struct usedblock {
    unsigned            size;
    struct usedblock*   start;
};

/* Space needed for administering used blocks */
#define HEAP_ADMIN_SPACE        sizeof (struct usedblock)

/* The data type used to implement the free list.
** Beware: Field order is significant!
*/
struct freeblock {
    unsigned            size;
    struct freeblock*   next;
    struct freeblock*   prev;
};



/* Variables that describe the heap */
extern unsigned*          _heaporg;     /* Bottom of heap */
extern unsigned*          _heapptr;     /* Current top */
extern unsigned*          _heapend;     /* Upper limit */
extern struct freeblock*  _heapfirst;   /* First free block in list */
extern struct freeblock*  _heaplast;    /* Last free block in list */



/* End of _heap.h */

#endif



