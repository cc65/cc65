/*****************************************************************************/
/*                                                                           */
/*				  symentry.c				     */
/*                                                                           */
/*	    Symbol table entry forward for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#include <string.h>

/* common */
#include "xmalloc.h"

/* ca65 */
#include "scanner.h"
#include "symentry.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



SymEntry* SymList = 0;  	/* List of all symbol table entries */



/*****************************************************************************/
/*     	       		  	     Code			   	     */
/*****************************************************************************/



SymEntry* NewSymEntry (const char* Name)
/* Allocate a symbol table entry, initialize and return it */
{
    SymEntry* S;
    unsigned Len;

    /* Get the length of the name */
    Len = strlen (Name);

    /* Allocate memory */
    S = xmalloc (sizeof (SymEntry) + Len);

    /* Initialize the entry */
    S->Left	= 0;
    S->Right	= 0;
    S->Locals	= 0;
    S->SymTab	= 0;
    S->Pos	= CurPos;
    S->Flags	= 0;
    S->V.Expr	= 0;
    memset (S->ConDesPrio, 0, sizeof (S->ConDesPrio));
    memcpy (S->Name, Name, Len+1);

    /* Insert it into the list of all entries */
    S->List = SymList;
    SymList = S;

    /* Return the initialized entry */
    return S;
}



