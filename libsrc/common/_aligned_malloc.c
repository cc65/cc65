/*****************************************************************************/
/*                                                                           */
/*                              _aligned_malloc                              */
/*                                                                           */
/*                     Allocate an aligned memory block                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004-2005 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <stdlib.h>
#include <_heap.h>



/* This is a very simple version of an aligned memory allocator. We will
 * allocate a greater block, so we can place the aligned block within it
 * that is returned. We use our knowledge about the internal heap
 * structures to free the unused parts of the bigger block (the two chunks
 * below and above the aligned block).
 */



void* __fastcall__ _aligned_malloc (size_t size, size_t alignment)
/* Allocate a block of memory with the given size, which is aligned to a
 * memory address that is a multiple of alignment. alignment MUST NOT be
 * zero and MUST be a power of two, otherwise a call to this function will 
 * cause undefined behaviour. The function returns NULL if not enough memory 
 * is available to satisfy the request. To free the allocated block, use the
 * free() function.
 */
{
    size_t rawsize;
    size_t uppersize;
    size_t lowersize;
    register struct usedblock* b;
    register struct usedblock* u;
    register struct usedblock* p;

    /* Handle requests for zero sized blocks */
    if (size == 0) {
        return 0;
    }

    /* We don't really need alignment, but alignment-1 */
    --alignment;

    /* Round up the block size and allocate memory. We don't need to account
     * for the additional admin data needed to manage the used block, since
     * the block returned by malloc has this overhead added one time, and
     * the worst thing that may happen is that we cannot free the upper and
     * lower blocks.
     */
    b = malloc (size + alignment);

    /* Handle out of memory */
    if (b == 0) {
        return 0;
    }

    /* Create a new pointer that points to the user visible aligned block. */
    u = (struct usedblock*) (((unsigned)b + alignment) & ~alignment);

    /* Get the raw block pointer, which is located just below the user visible
     * block. The first word of this raw block is the total size of the block
     * including the admin space.
     */
    b = (b-1)->start;
    rawsize = b->size;

    /* Get a pointer to the (raw) upper block */
    p = (struct usedblock*) (size + (unsigned)u);

    /* Check if we can free the space above the allocated block. This is the
     * case if the size of the block is at least sizeof (struct freeblock)
     * bytes and the size of the remaining block is at least of this size,
     * too. If the upper block is smaller, we will just pass it to the caller
     * together with the requested aligned block.
     */
    uppersize = rawsize + (unsigned)b - (unsigned)p;
    if (uppersize >= sizeof (struct freeblock) &&
        (rawsize - uppersize) >= sizeof (struct freeblock)) {

        /* Setup the usedblock structure */
        p->size  = uppersize;
        p->start = p;

        /* Generate a pointer to the user space and free the block */
        free (p + 1);

        /* Decrement the raw block size by the amount of space just free'd */
        rawsize -= uppersize;
    }

    /* Check if we can free the space below the allocated block. This is the
     * case, if the size of the block is at least sizeof (struct freeblock)
     * bytes and the size of the remaining block is at least of this size,
     * too. If the lower block is smaller, we will just pass it to the caller
     * together with the requested aligned block.
     * Beware: We need an additional struct usedblock in the lower block which
     * is part of the block that is passed back to the caller.
     */
    lowersize = ((unsigned)u - (unsigned)b) - sizeof (struct usedblock);
    if (lowersize >= sizeof (struct freeblock) &&
        (rawsize - lowersize) >= sizeof (struct freeblock)) {

        /* b does already point to the raw lower block. Setup the usedblock
         * structure.
         */
        b->size  = lowersize;
        b->start = b;

        /* Generate a pointer to the user space and free the block */
        free (b + 1);

        /* Decrement the raw block size by the amount of space just free'd */
        rawsize -= lowersize;

        /* Set b to the raw user block */
        b = u - 1;
    }

    /* u does now point to the user visible block, while b points to the raw
     * block, and rawsize contains the size of the raw block. Setup the
     * usedblock structure but beware: If we didn't free the lower block, it
     * is splitted, which means that we must use u to write the start field,
     * and b to write the size.
     */
    (u-1)->start = b;
    b->size  = rawsize;

    /* Return the user portion of the aligned block */
    return u;
}



