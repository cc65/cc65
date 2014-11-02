/*****************************************************************************/
/*                                                                           */
/*                                casenode.c                                 */
/*                                                                           */
/*        Node for the tree that is generated for a switch statement         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



#include <limits.h>

/* common */
#include "coll.h"
#include "xmalloc.h"

/* cc65 */
#include "asmlabel.h"
#include "error.h"
#include "casenode.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



CaseNode* NewCaseNode (unsigned char Value)
/* Create and initialize a new CaseNode */
{
    /* Allocate memory */
    CaseNode* N = xmalloc (sizeof (CaseNode));

    /* Initialize the fields */
    N->Value = Value;
    N->Label = 0;
    N->Nodes = 0;

    /* Return the new node */
    return N;
}



void FreeCaseNode (CaseNode* N)
/* Delete a case node plus all sub nodes */
{
    if (N->Nodes) {
        FreeCaseNodeColl (N->Nodes);
    }
    xfree (N);
}



void FreeCaseNodeColl (Collection* Nodes)
/* Free a collection of case nodes */
{
    unsigned I;
    for (I = 0; I < CollCount (Nodes); ++I) {
        FreeCaseNode (CollAtUnchecked (Nodes, I));
    }
    FreeCollection (Nodes);
}



int SearchCaseNode (const Collection* Nodes, unsigned char Key, int* Index)
/* Search for a node in the given collection. If the node has been found,
** set Index to the index of the node and return true. If the node was not
** found, set Index the the insertion position of the node and return
** false.
*/
{
    /* Do a binary search */
    int First = 0;
    int Last = CollCount (Nodes) - 1;
    int S = 0;

    while (First <= Last) {

        /* Set current to mid of range */
        int Current = (Last + First) / 2;

        /* Get the entry from this position */
        const CaseNode* N = CollConstAt (Nodes, Current);

        /* Compare the values */
        if (N->Value < Key) {
            First = Current + 1;
        } else {
            Last = Current - 1;
            if (N->Value == Key) {
                /* Found. We cannot have duplicates, so end the search here. */
                S = 1;
                First = Current;
            }
        }

    }

    *Index = First;
    return S;
}



unsigned InsertCaseValue (Collection* Nodes, unsigned long Val, unsigned Depth)
/* Insert a new case value into a CaseNode tree with the given depth. Return
** the code label for the value.
*/
{
    CaseNode* N = 0;
    unsigned CaseLabel = GetLocalLabel ();  /* Code label */

    while (Depth--) {

        int Index;

        /* Get the key */
        unsigned char Key = (Val >> (Depth * CHAR_BIT)) & 0xFF;

        /* Search for the node in the collection */
        if (SearchCaseNode (Nodes, Key, &Index) == 0) {

            /* Node not found - insert one */
            N = NewCaseNode (Key);
            CollInsert (Nodes, N, Index);

            /* If this is not the last round, create the collection for
            ** the subnodes, otherwise get a label for the code.
            */
            if (Depth > 0) {
                N->Nodes = NewCollection ();
            } else {
                N->Label = CaseLabel;
            }

        } else {
            /* Node found, get it */
            N = CollAt (Nodes, Index);

            /* If this is the last round and we found a node, we have a
            ** duplicate case label in a switch.
            */
            if (Depth == 0) {
                Error ("Duplicate case label");
            }
        }

        /* Get the collection from the node for the next round. */
        Nodes = N->Nodes;
    }

    /* Return the label of the node we found/created */
    return CaseLabel;
}
