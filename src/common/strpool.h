/*****************************************************************************/
/*                                                                           */
/*                                 strpool.h                                 */
/*                                                                           */
/*                               A string pool                               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
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



/* A string pool is used to store identifiers and other strings. Each string
 * stored in the pool has a unique id, which may be used to access the string
 * in the pool. Identical strings are only stored once in the pool and have
 * identical ids. This means that instead of comparing strings, just the
 * string pool ids must be compared.
 */



#ifndef STRPOOL_H
#define STRPOOL_H



/* common */
#include "coll.h"
#include "inline.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Opaque entry */
typedef struct StrPoolEntry StrPoolEntry;

typedef struct StrPool StrPool;
struct StrPool {
    StrPoolEntry*   Tab[211];   /* Entry hash table */
    Collection      Entries;    /* Entries sorted by number */
    unsigned        TotalSize;  /* Total size of all string data */
};



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



StrPool* InitStrPool (StrPool* P);
/* Initialize a string pool */

void DoneStrPool (StrPool* P);
/* Free the data of a string pool (but not the data itself) */

StrPool* NewStrPool (void);
/* Allocate, initialize and return a new string pool */

void FreeStrPool (StrPool* P);
/* Free a string pool */

void SP_Use (char* Buffer, unsigned Size);
/* Delete existing data and use the data from Buffer instead. Buffer must be
 * allocated on the heap and will be freed using xfree() if necessary.
 */

const char* SP_Get (const StrPool* P, unsigned Index);
/* Return a string from the pool. Index must exist, otherwise FAIL is called. */

unsigned SP_Add (StrPool* P, const char* S);
/* Add a string to the buffer and return the index. If the string does already
 * exist in the pool, SP_Add will just return the index of the existing string.
 */

#if defined(HAVE_INLINE)
INLINE unsigned SB_GetCount (const StrPool* P)
/* Return the number of strings in the pool */
{
    return CollCount (&P->Entries);
}
#else
#  define SB_GetCount(P)        CollCount (&(P)->Entries)
#endif



/* End of strpool.h */

#endif



