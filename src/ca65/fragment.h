/*****************************************************************************/
/*                                                                           */
/*                                fragment.h                                 */
/*                                                                           */
/*                Data fragments for the ca65 crossassembler                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#ifndef FRAGMENT_H
#define FRAGMENT_H



/* common */
#include "exprdefs.h"
#include "coll.h"

/* ca65 */
#include "lineinfo.h"



/*****************************************************************************/
/*                              struct Fragment                              */
/*****************************************************************************/



typedef struct Fragment Fragment;
struct Fragment {
    Fragment*           Next;       /* Pointer to next fragment in segment */
    Fragment*           LineList;   /* List of fragments for one src line */
    Collection          LI;         /* Line info for this fragment */
    unsigned short      Len;        /* Length for this fragment */
    unsigned char       Type;       /* Fragment type */
    union {
        unsigned char   Data[sizeof (ExprNode*)];       /* Literal values */
        ExprNode*       Expr;                           /* Expression */
    } V;
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Fragment* NewFragment (unsigned char Type, unsigned short Len);
/* Create, initialize and return a new fragment. The fragment will be inserted
** into the current segment.
*/



/* End of fragment.h */

#endif
