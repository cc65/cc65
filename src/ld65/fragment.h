/*****************************************************************************/
/*                                                                           */
/*                                fragment.h                                 */
/*                                                                           */
/*                        Code/data fragment routines                        */
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
#include "coll.h"
#include "filepos.h"

/* Ld65 */
#include "lineinfo.h"


/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct ObjData;
struct Section;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Fragment structure */
typedef struct Fragment Fragment;
struct Fragment {
    Fragment*           Next;           /* Next fragment in list */
    struct ObjData*     Obj;            /* Source of fragment */
    struct Section*     Sec;            /* Section for this fragment */
    unsigned            Size;           /* Size of data/expression */
    struct ExprNode*    Expr;           /* Expression if FRAG_EXPR */
    Collection          LineInfos;      /* Line info for this fragment */
    unsigned char       Type;           /* Type of fragment */
    unsigned char       LitBuf [1];     /* Dynamically alloc'ed literal buffer */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Fragment* NewFragment (unsigned char Type, unsigned Size, struct Section* S);
/* Create a new fragment and insert it into the section S */

#if defined(HAVE_INLINE)
INLINE const char* GetFragmentSourceName (const Fragment* F)
/* Return the name of the source file for this fragment */
{
    return GetSourceNameFromList (&F->LineInfos);
}
#else
#  define GetFragmentSourceName(F)      GetSourceNameFromList (&(F)->LineInfos)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned GetFragmentSourceLine (const Fragment* F)
/* Return the source file line for this fragment */
{               
    return GetSourceLineFromList (&F->LineInfos);
}
#else
#  define GetFragmentSourceLine(F)      GetSourceLineFromList (&(F)->LineInfos)
#endif



/* End of fragment.h */

#endif
