/*****************************************************************************/
/*                                                                           */
/*                                 codelab.h                                 */
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



#ifndef CODELAB_H
#define CODELAB_H



/* common */
#include "coll.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct CodeEntry;



/*****************************************************************************/
/*                             struct CodeLabel                              */
/*****************************************************************************/



/* Label structure */
typedef struct CodeLabel CodeLabel;
struct CodeLabel {
    CodeLabel*          Next;           /* Next in hash list */
    char*               Name;           /* Label name */
    unsigned            Hash;           /* Hash over the name */
    struct CodeEntry*   Owner;          /* Owner entry */
    Collection          JumpFrom;       /* Entries that jump here */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



CodeLabel* NewCodeLabel (const char* Name, unsigned Hash);
/* Create a new code label, initialize and return it */

void FreeCodeLabel (CodeLabel* L);
/* Free the given code label */

#if defined(HAVE_INLINE)
INLINE unsigned CL_GetRefCount (const CodeLabel* L)
/* Get the number of references for this label */
{
    return CollCount (&L->JumpFrom);
}
#else
#  define CL_GetRefCount(L)     CollCount (&(L)->JumpFrom)
#endif

#if defined(HAVE_INLINE)
INLINE struct CodeEntry* CL_GetRef (CodeLabel* L, unsigned Index)
/* Get a code entry referencing this label */
{
    return CollAt (&L->JumpFrom, Index);
}
#else
#  define CL_GetRef(L, Index)   CollAt (&(L)->JumpFrom, (Index))
#endif

void CL_AddRef (CodeLabel* L, struct CodeEntry* E);
/* Let the CodeEntry E reference the label L */

void CL_MoveRefs (CodeLabel* OldLabel, CodeLabel* NewLabel);
/* Move all references to OldLabel to point to NewLabel. OldLabel will have no
** more references on return.
*/

void CL_Output (const CodeLabel* L);
/* Output the code label to the output file */



/* End of codelab.h */

#endif
