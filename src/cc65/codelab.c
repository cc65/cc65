/*****************************************************************************/
/*                                                                           */
/*				   codelab.c				     */
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
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "codeent.h"
#include "codelab.h"



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
    L->Flags = 0;
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



void AddLabelRef (CodeLabel* L, struct CodeEntry* E)
/* Let the CodeEntry E reference the label L */
{
    /* The insn at E jumps to this label */
    E->JumpTo = L;

    /* Remember that in the label */
    CollAppend (&L->JumpFrom, E);
}



unsigned RemoveLabelRef (CodeLabel* L, const struct CodeEntry* E)
/* Remove a reference to this label, return the number of remaining references */
{
    /* Delete the item */
    CollDeleteItem (&L->JumpFrom, E);

    /* Return the number of remaining references */
    return CollCount (&L->JumpFrom);
}



void MoveLabelRefs (CodeLabel* OldLabel, CodeLabel* NewLabel)
/* Move all references to OldLabel to point to NewLabel. OldLabel will have no
 * more references on return.
 */
{
    /* Walk through all instructions referencing the old label */
    unsigned Count = CollCount (&OldLabel->JumpFrom);
    while (Count--) {

	/* Get the instruction that references the old label */
	CodeEntry* E = CollAt (&OldLabel->JumpFrom, Count);

	/* Change the reference to the new label */
	CHECK (E->JumpTo == OldLabel);
	AddLabelRef (NewLabel, E);

    }

    /* There are no more references to the old label */
    CollDeleteAll (&OldLabel->JumpFrom);
}



void OutputCodeLabel (const CodeLabel* L, FILE* F)
/* Output the code label to a file */
{
    fprintf (F, "%s:", L->Name);
}



