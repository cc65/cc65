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
extern unsigned*          __heaporg;    /* Bottom of heap */
extern unsigned*          __heapptr;    /* Current top */
extern unsigned*          __heapend;    /* Upper limit */
extern struct freeblock*  __heapfirst;  /* First free block in list */
extern struct freeblock*  __heaplast;   /* Last free block in list */

#if __CC65_STD__ == __CC65_STD_CC65__
/* define old name with one underscore for backwards compatibility */
#define _heaporg __heaporg
#define _heapptr __heapptr
#define _heapend __heapend
#define _heapfirst __heapfirst
#define _heaplast __heaplast
#endif

/* End of _heap.h */

#endif



