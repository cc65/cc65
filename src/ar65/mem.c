/*****************************************************************************/
/*									     */
/*				     mem.c				     */
/*									     */
/*		    Memory allocation for the ar65 archiver		     */
/*									     */
/*									     */
/*									     */
/* (C) 1998	Ullrich von Bassewitz					     */
/*		Wacholderweg 14						     */
/*		D-70597 Stuttgart					     */
/* EMail:	uz@musoftware.de					     */
/*									     */
/*									     */
/* This software is provided 'as-is', without any expressed or implied	     */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.				     */
/*									     */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:			     */
/*									     */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.					     */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.			     */
/* 3. This notice may not be removed or altered from any source		     */
/*    distribution.							     */
/*									     */
/*****************************************************************************/



#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "mem.h"



/*****************************************************************************/
/*				     code				     */
/*****************************************************************************/



void* Xmalloc (size_t size)
/* Allocate memory, check for out of memory condition. Do some debugging */
{
    void* p;

    p = malloc (size);
    if (p == 0 && size != 0) {
	Error ("Out of memory");
    }

    /* Return a pointer to the block */
    return p;
}



void Xfree (const void* block)
/* Free the block, do some debugging */
{
    free ((void*) block);
}



char* StrDup (const char* s)
/* Duplicate a string on the heap. The function checks for out of memory */
{
    unsigned len;

    len = strlen (s) + 1;
    return memcpy (Xmalloc (len), s, len);
}



