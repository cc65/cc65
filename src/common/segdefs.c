/*****************************************************************************/
/*                                                                           */
/*				   segdefs.c   	       	       	       	     */
/*                                                                           */
/*		Segment definitions for the bin65 binary utils		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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



#include <string.h>

/* common */
#include "chartype.h"
#include "xmalloc.h"
#include "segdefs.h"



/*****************************************************************************/
/*     	       	    	    	     Code				     */
/*****************************************************************************/



SegDef* NewSegDef (const char* Name, unsigned Type)
/* Create a new segment definition and return it */
{
    /* Allocate memory */
    SegDef* D = xmalloc (sizeof (SegDef));

    /* Initialize it */
    if (D) {
        D->Name = xstrdup (Name);
        D->Type = Type;
    }

    /* Return the result */
    return D;
}



void FreeSegDef (SegDef* D)
/* Free a segment definition */
{
    xfree (D->Name);
    xfree (D);
}



SegDef* DupSegDef (const SegDef* Def)
/* Duplicate a segment definition and return it */
{
    return NewSegDef (Def->Name, Def->Type);
}



int ValidSegName (const char* Name)
/* Return true if the given segment name is valid, return false otherwise */
{
    /* Must start with '_' or a letter */
    if ((*Name != '_' && !IsAlpha(*Name)) || strlen(Name) > 80) {
       	return 0;
    }

    /* Can have letters, digits or the underline */
    while (*++Name) {
       	if (*Name != '_' && !IsAlNum(*Name)) {
       	    return 0;
       	}
    }

    /* Name is ok */
    return 1;
}



