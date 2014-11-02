/*****************************************************************************/
/*                                                                           */
/*                                casenode.h                                 */
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



#ifndef CASENODE_H
#define CASENODE_H



/* common */
#include "coll.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct CaseNode CaseNode;
struct CaseNode {
    unsigned char Value;
    unsigned      Label;
    Collection*   Nodes;
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



CaseNode* NewCaseNode (unsigned char Value);
/* Create and initialize a new CaseNode */

void FreeCaseNode (CaseNode* N);
/* Delete a case node plus all sub nodes */

#if defined(HAVE_INLINE)
INLINE CaseNode* CN_GetSubNode (CaseNode* N, unsigned Index)
/* Get a sub node of the given node */
{
    return CollAt (N->Nodes, Index);
}
#else
#  define CN_GetSubNode(N, Index) CollAt (&(N)->Nodes, Index)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned char CN_GetValue (const CaseNode* N)
/* Return the value for a case node */
{
    return N->Value;
}
#else
#  define CN_GetValue(N)  ((N)->Value)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned CN_GetLabel (const CaseNode* N)
/* Return the label for a case node */
{
    return N->Label;
}
#else
#  define CN_GetLabel(N)  ((N)->Label)
#endif

#if defined(HAVE_INLINE)
INLINE int CN_IsLeafNode (const CaseNode* N)
/* Return true if this is a leaf node */
{
    return (N->Nodes == 0);
}
#else
#  define CN_IsLeafNode(N)  ((N)->Nodes == 0)
#endif

void FreeCaseNodeColl (Collection* Nodes);
/* Free a collection of case nodes */

int SearchCaseNode (const Collection* Nodes, unsigned char Key, int* Index);
/* Search for a node in the given collection. If the node has been found,
** set Index to the index of the node and return true. If the node was not
** found, set Index the the insertion position of the node and return
** false.
*/

unsigned InsertCaseValue (Collection* Nodes, unsigned long Val, unsigned Depth);
/* Insert a new case value into a CaseNode tree with the given depth. Return
** the code label for the value.
*/



/* End of casenode.h */

#endif
