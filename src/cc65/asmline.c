/*****************************************************************************/
/*                                                                           */
/*				   asmline.h   	       	       	       	     */
/*                                                                           */
/*		       Internal assembler line structure		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#include <stdio.h>

#include "../common/xsprintf.h"

#include "error.h"
#include "mem.h"
#include "asmline.h"



/*****************************************************************************/
/*	       	 		     Data   				     */
/*****************************************************************************/



/* Number used to index lines */
static unsigned long LineIndex = 0;

/* The line list */
Line*  FirstLine = 0;     	/* Pointer to first line */
Line*  LastLine  = 0;    	/* Pointer to last line */



/*****************************************************************************/
/*	       	 	       	     Code   				     */
/*****************************************************************************/



static Line* NewLine (const char* Format, va_list ap)
/* Interal routine to create a new line from the given text */
{
    char 	Buf [8192];
    unsigned	Len;
    Line*	L;


    /* Make a string from the given format and arguments */
    xvsprintf (Buf, sizeof (Buf), Format, ap);

    /* Get the length of the line */
    Len = strlen (Buf);

    /* Allocate memory */
    L = xmalloc (sizeof (Line) + Len);

    /* Partially initialize the struct (the remaining fields are initialized
     * by the caller).
     */
    L->Flags  	= 0;
    L->Size   	= 0;
    L->Len    	= Len;
    memcpy (L->Line, Buf, Len+1);

    /* Return the new line */
    return L;
}



Line* NewCodeLine (const char* Format, va_list ap)
/* Create a new code line and return it */
{
    /* Create a new line struct */
    Line* L = NewLine (Format, ap);

    /* Initialize struct fields */
    L->Index  	= LineIndex++;

    /* Insert the line into the list */
    if (FirstLine == 0) {
       	/* The list is empty */
       	L->Next = L->Prev = 0;
       	FirstLine = LastLine = L;
    } else {
       	/* There are entries in the list, add the new one at the end */
       	LastLine->Next = L;
       	L->Prev = LastLine;
       	L->Next = 0;
       	LastLine = L;
    }

    /* Return the new line */
    return L;
}



Line* NewCodeLineAfter (Line* LineBefore, const char* Format, va_list ap)
/* Create a new line, insert it after L and return it. */
{
    /* Create a new line struct */
    Line* L = NewLine (Format, ap);

    /* Initialize struct fields. We use the same index for the inserted line
     * as for its predecessor, since we cannot create new numbers on the
     * fly and the index is only used to determine sort order.
     */
    L->Index   	= LineBefore->Index;

    /* Insert the line after its predecessor */
    L->Next = LineBefore->Next;
    LineBefore->Next = L;
    L->Prev = LineBefore;
    if (L->Next) {
	L->Next->Prev = L;
    } else {
	/* This is the last line */
	LastLine = L;
    }

    /* Return the new line */
    return L;
}



void FreeCodeLine (Line* L)
/* Remove a line from the list and free it */
{
    /* Unlink the line */
    if (L->Prev == 0) {
       	/* No line before this one */
       	FirstLine = L->Next;
    } else {
       	L->Prev->Next = L->Next;
    }
    if (L->Next == 0) {
       	/* No line after this one */
       	LastLine = L->Prev;
    } else {
       	L->Next->Prev = L->Prev;
    }

    /* Free the struct */
    xfree (L);
}



