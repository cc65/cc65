/*****************************************************************************/
/*                                                                           */
/*				  fragment.c				     */
/*                                                                           */
/*			  Code/data fragment routines			     */
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



/* common */
#include "fragdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "expr.h"
#include "fragment.h"
#include "fileio.h"
#include "segments.h"
#include "spool.h"



/*****************************************************************************/
/*     	      	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



static FragCheck* NewFragCheck (unsigned Action)
/* Allocate a new FragCheck struct and return it */
{
    /* Allocate memory */
    FragCheck* FC = xmalloc (sizeof (FragCheck));

    /* Initialize the fields */
    FC->Next    = 0;
    FC->Expr    = 0;
    FC->Action  = Action;
    FC->Message = INVALID_STRING_ID;

    /* Return the new struct */
    return FC;
}



FragCheck* ReadFragCheck (FILE* F, Fragment* Frag)
/* Read a fragment check expression from the given file */
{
    /* Get the object file pointer from the fragment */
    ObjData* O = Frag->Obj;

    /* Read the action and create a new struct */
    FragCheck* FC = NewFragCheck (ReadVar (F));

    /* Determine the remaining data from the action */
    switch (FC->Action) {

        case FRAG_ACT_WARN:
        case FRAG_ACT_ERROR:
            FC->Expr = ReadExpr (F, O);
            FC->Message = MakeGlobalStringId (O, ReadVar (F));
            break;

        default:
            Internal ("In module `%s', file `%s', line %lu: Invalid fragment "
                      "check action: %u",
                      GetObjFileName (O),
                      GetSourceFileName (O, Frag->Pos.Name),
                      Frag->Pos.Line, FC->Action);
    }

    /* Return the new fragment check */
    return FC;
}



Fragment* NewFragment (unsigned char Type, unsigned Size, Section* S)
/* Create a new fragment and insert it into the section S */
{
    Fragment* F;

    /* Calculate the size of the memory block. LitBuf is only needed if the
     * fragment contains literal data.
     */
    unsigned FragSize = sizeof (Fragment) - 1;
    if (Type == FRAG_LITERAL) {
        FragSize += Size;
    }

    /* Allocate memory */
    F = xmalloc (FragSize);

    /* Initialize the data */
    F->Next      = 0;
    F->Obj       = 0;
    F->Size      = Size;
    F->Expr      = 0;
    InitFilePos (&F->Pos);
    F->LI        = 0;
    F->Check     = 0;
    F->Type      = Type;

    /* Insert the code fragment into the section */
    if (S->FragRoot == 0) {
      	/* First fragment */
      	S->FragRoot = F;
    } else {
      	S->FragLast->Next = F;
    }
    S->FragLast = F;

    /* Increment the size of the section by the size of the fragment */
    S->Size += Size;

    /* Increment the size of the segment that contains the section */
    S->Seg->Size += Size;

    /* Return the new fragment */
    return F;
}



