/*****************************************************************************/
/*                                                                           */
/*                                 codelab.c                                 */
/*                                                                           */
/*                           Code label structure                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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
#include "output.h"



/*****************************************************************************/
/*                                   Code                                    */
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



void CL_AddRef (CodeLabel* L, struct CodeEntry* E)
/* Let the CodeEntry E reference the label L */
{
    /* The insn at E jumps to this label */
    E->JumpTo = L;

    /* Replace the code entry argument with the name of the new label */
    CE_SetArg (E, L->Name);

    /* Remember that in the label */
    CollAppend (&L->JumpFrom, E);
}



void CL_MoveRefs (CodeLabel* OldLabel, CodeLabel* NewLabel)
/* Move all references to OldLabel to point to NewLabel. OldLabel will have no
** more references on return.
*/
{
    /* Walk through all instructions referencing the old label */
    unsigned Count = CL_GetRefCount (OldLabel);
    while (Count--) {

        /* Get the instruction that references the old label */
        CodeEntry* E = CL_GetRef (OldLabel, Count);

        /* Change the reference to the new label */
        CHECK (E->JumpTo == OldLabel);
        CL_AddRef (NewLabel, E);

    }

    /* There are no more references to the old label */
    CollDeleteAll (&OldLabel->JumpFrom);
}



void CL_Output (const CodeLabel* L)
/* Output the code label to the output file */
{
    WriteOutput ("%s:", L->Name);
    if (strlen (L->Name) > 6) {
        /* Label is too long, add a linefeed */
        WriteOutput ("\n");
    }
}
