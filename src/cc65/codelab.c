/*****************************************************************************/
/*                                                                           */
/*				    label.c				     */
/*                                                                           */
/*			     Code label structure			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001     Ullrich von Bassewitz                                        */
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



/* common */
#include "xmalloc.h"

/* cc65 */
#include "label.h"



/*****************************************************************************/
/*     	       	      	  	     Code				     */
/*****************************************************************************/



CodeLabel* NewCodeLabel (const char* Name, unsigned Hash)
/* Create a new code label, initialize and return it */
{
    /* Allocate memory */
    CodeLabel* L = xmalloc (sizeof (CodeLabel));

    /* Initialize the fields */
    L->Next  = 0;
    L->Name  = xstrdup (Name);
    L->Hash  = Hash;
    L->Owner = 0;
    InitCollection (&L->JumpFrom);

    /* Return the new label */
    return L;
}



void FreeCodeLabel (CodeLabel* L)
/* Free the given code label */
{
    /* Free the name */
    xfree (L->Name);

    /* Free the collection */
    DoneCollection (&L->JumpFrom);

    /* Delete the struct */
    xfree (L);
}



void OutputCodeLabel (FILE* F, const CodeLabel* L)
/* Output the code label to a file */
{
    fprintf (F, "%s:\n", L->Name);
}



