/*****************************************************************************/
/*                                                                           */
/*                                  tpool.h                                  */
/*                                                                           */
/*                          Pool for generic types                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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



#ifndef TPOOL_H
#define TPOOL_H



#include <stdio.h>

/* common */
#include "strpool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* An invalid type */
#define INVALID_TYPE_ID   (~0U)

/* The string pool we're using */
extern StringPool* TypePool;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE unsigned GetTypeId (const StrBuf* Type)
/* Return the id of the given generic type */
{
    return SP_Add (TypePool, Type);
}
#else
#  define GetTypeId(Type)       SP_Add (TypePool, (Type))
#endif

#if defined(HAVE_INLINE)
INLINE const StrBuf* GetType (unsigned Index)
/* Convert a type index into a type string */
{
    return SP_Get (TypePool, Index);
}
#else
#  define GetType(Index)        SP_Get (TypePool, (Index))
#endif

#if defined(HAVE_INLINE)
INLINE unsigned TypeCount (void)
/* Return the number of types in the pool */
{
    return SP_GetCount (TypePool);
}
#else
#  define TypeCount()   SP_GetCount (TypePool)
#endif

void PrintDbgTypes (FILE* F);
/* Output the types to a debug info file */

void InitTypePool (void);
/* Initialize the type pool */



/* End of tpool.h */

#endif
