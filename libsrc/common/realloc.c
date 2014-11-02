/*****************************************************************************/
/*                                                                           */
/*                                 realloc.c                                 */
/*                                                                           */
/*               Change the size of an allocated memory block                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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
#include <string.h>
#include <_heap.h>



void* __fastcall__ realloc (void* block, register size_t size)
{
    register struct usedblock* b;
    struct usedblock* newblock;
    unsigned oldsize;
    unsigned newhptr;

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

    /* The word below the user block contains a pointer to the start of the
    ** raw memory block. The first word of this raw memory block is the full
    ** size of the block. Get a pointer to the real block, get the old block
    ** size.
    */
    b = (((struct usedblock*) block) - 1)->start;
    oldsize = b->size;

    /* Is the block at the current heap top? */
    if (((unsigned) b) + oldsize == ((unsigned) _heapptr)) {
        /* Check if we've enough memory at the heap top */
        newhptr = ((unsigned) _heapptr) - oldsize + size;
        if (newhptr <= ((unsigned) _heapend)) {
            /* Ok, there's space enough */
            _heapptr = (unsigned*) newhptr;
            b->size = size;
            b->start = b;
            return block;
        }
    }

    /* The given block was not located on top of the heap, or there's no
    ** room left. Try to allocate a new block and copy the data.
    */
    if (newblock = malloc (size)) {

        /* Adjust the old size to the user visible portion */
        oldsize -= HEAP_ADMIN_SPACE;

        /* If the new block is larger than the old one, copy the old
        ** data only
        */
        if (size > oldsize) {
            size = oldsize;
        }

        /* Copy the block data */
        memcpy (newblock, block, size);
        free (block);
    }
    return newblock;
}



