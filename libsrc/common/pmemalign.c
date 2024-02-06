/*****************************************************************************/
/*                                                                           */
/*                              posix_memalign                               */
/*                                                                           */
/*                     Allocate an aligned memory block                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004-2005 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
/*                                                                           */
/*                                                                           */
/* This software is provided "as-is," without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software.  If you use this software  */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated, but is not required.                                      */
/* 2. Alterred source versions must be marked plainly as such, and must not  */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or alterred from any source             */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <stddef.h>                     /* define NULL */
#include <stdlib.h>                     /* declare function's prototype */
#include <_heap.h>

#include <errno.h>
#define EOK             0               /* No errors (non-standard name) */



/* This is a very simple version of an aligned memory allocator.  We will
** allocate a greater block, so that we can place the aligned block (that is
** returned) within it.  We use our knowledge about the internal heap
** structures to free the unused parts of the bigger block (the two chunks
** below and above the aligned block).
*/


int __fastcall__ posix_memalign (void** memptr, size_t alignment, size_t size)
/* Allocate a block of memory with the given "size", which is aligned to a
** memory address that is a multiple of "alignment".  "alignment" MUST NOT be
** zero, and MUST be a power of two; otherwise, this function will return
** EINVAL.  The function returns ENOMEM if not enough memory is available
** to satisfy the request.  "memptr" must point to a variable; that variable
** will return the address of the allocated memory.  Use free() to release that
** allocated block.
*/
{
    size_t rawsize;
    size_t uppersize;
    size_t lowersize;
    char err;
    register struct usedblock* b;       /* points to raw Block */
    register struct usedblock* u;       /* points to User block */
    register struct usedblock* p;       /* Points to upper block */

    /* Handle requests for zero-sized blocks */
    if (size == 0) {
err_einval:
        err = EINVAL;
err_out:
        *memptr = NULL;
        return err;
    }

    /* Test alignment: is it a power of two? There must be one and only one bit set. */
    if (alignment == 0) {
        goto err_einval;
    }

    if (alignment & (alignment - 1)) {
        goto err_einval;
    }

    /* Augment the block size up to the alignment, and allocate memory.
    ** We don't need to account for the additional admin. data that's needed to
    ** manage the used block, because the block returned by malloc() has that
    ** overhead added one time; and, the worst thing that might happen is that
    ** we cannot free the upper and lower blocks.
    */
    b = malloc (--alignment + size);

    /* Handle out-of-memory */
    if (b == NULL) {
        err = ENOMEM;
        goto err_out;
    }

    /* Create (and return) a new pointer that points to the user-visible
    ** aligned block.
    */
    u = *memptr = (struct usedblock*) (((unsigned)b + alignment) & ~alignment);

    /* Get a pointer to the (raw) upper block */
    p = (struct usedblock*) ((char*)u + size);

    /* Get the raw-block pointer, which is located just below the visible
    ** unaligned block.  The first word of this raw block is the total size
    ** of the block, including the admin. space.
    */
    b = (b-1)->start;
    rawsize = b->size;

    /* Check if we can free the space above the user block.  That is the case
    ** if the size of the block is at least sizeof (struct freeblock) bytes,
    ** and the size of the remaining block is at least that size, too.
    ** If the upper block is smaller, then we just will pass it to the caller,
    ** together with the requested aligned block.
    */
    uppersize = rawsize - (lowersize = (char*)p - (char*)b);
    if (uppersize >= sizeof (struct freeblock) &&
        lowersize >= sizeof (struct freeblock)) {

        /* Setup the usedblock structure */
        p->size  = uppersize;
        p->start = p;

        /* Generate a pointer to the (upper) user space, and free that block */
        free (p + 1);

        /* Decrease the raw-block size by the amount of space just freed */
        rawsize = lowersize;
    }

    /* Check if we can free the space below the user block.  That is the case
    ** if the size of the block is at least sizeof (struct freeblock) bytes,
    ** and the size of the remaining block is at least that size, too.  If the
    ** lower block is smaller, we just will pass it to the caller, together
    ** with the requested aligned block.
    ** Beware:  We need an additional struct usedblock, in the lower block,
    ** which is part of the block that is passed back to the caller.
    */
    lowersize = ((char*)u - (char*)b) - sizeof (struct usedblock);
    if (           lowersize  >= sizeof (struct freeblock) &&
        (rawsize - lowersize) >= sizeof (struct freeblock)) {

        /* b already points to the raw lower-block.
        ** Set up the usedblock structure.
        */
        b->size  = lowersize;
        b->start = b;

        /* Generate a pointer to the (lower) user space, and free that block */
        free (b + 1);

        /* Decrease the raw-block size by the amount of space just freed */
        rawsize -= lowersize;

        /* Set b to the raw user-block (that will be returned) */
        b = u - 1;
    }

    /* u points to the user-visible block, while b points to the raw block,
    ** and rawsize contains the length of the raw block.  Set up the usedblock
    ** structure, but beware:  If we didn't free the lower block, then it is
    ** split; which means that we must use b to write the size,
    ** and u to write the start field.
    */
    b->size = rawsize;
    (u-1)->start = b;

    return EOK;
}
