/*****************************************************************************/
/*									     */
/*				   xmalloc.c				     */
/*                                                                           */
/*			 Memory allocation subroutines			     */
/*					    				     */
/*					    				     */
/*					    				     */
/* (C) 2000-2001 Ullrich von Bassewitz	    				     */
/*	   	 Wacholderweg 14					     */
/*	   	 D-70597 Stuttgart					     */
/* EMail:  	 uz@musoftware.de					     */
/*	   								     */
/*	   								     */
/* This software is provided 'as-is', without any expressed or implied	     */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.				     */
/*	   								     */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:			     */
/*	   								     */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.					     */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.			     */
/* 3. This notice may not be removed or altered from any source		     */
/*    distribution.							     */
/*	   								     */
/*****************************************************************************/



#include <stdlib.h>
#include <string.h>

#include "abend.h"
#include "debugflag.h"
#include "xmalloc.h"



/*****************************************************************************/
/*		      		     code				     */
/*****************************************************************************/



void* xmalloc (size_t Size)
/* Allocate memory, check for out of memory condition. Do some debugging */
{
    /* Allocate memory */
    void* P = malloc (Size);

    /* Check for errors */
    if (P == 0 && Size != 0) {
	AbEnd ("Out of memory - requested block size = %lu", (unsigned long) Size);
    }

    /* Return a pointer to the block */
    return P;
}



void* xrealloc (void* P, size_t Size)
/* Reallocate a memory block, check for out of memory */
{
    /* Reallocate the block */
    void* N = realloc (P, Size);

    /* Check for errors */
    if (N == 0 && Size != 0) {
       	AbEnd ("Out of memory in realloc - requested block size = %lu", (unsigned long) Size);
    }

    /* Return the pointer to the new block */
    return N;
}



void xfree (void* Block)
/* Free the block, do some debugging */
{
    free (Block);
}



char* xstrdup (const char* S)
/* Duplicate a string on the heap. The function checks for out of memory */
{
    /* Allow dup'ing of NULL strings */
    if (S) {

	/* Get the length of the string */
	unsigned Len = strlen (S) + 1;

	/* Allocate memory and return a copy */
	return memcpy (xmalloc (Len), S, Len);

    } else {

	/* Return a NULL pointer */
	return 0;

    }
}




