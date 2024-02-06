/*****************************************************************************/
/*                                                                           */
/*                                 strpool.h                                 */
/*                                                                           */
/*                               A string pool                               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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
** stored in the pool has a unique ID, which may be used to access the string
** in the pool. Identical strings are only stored once in the pool and have
** identical IDs. This means that instead of comparing strings, just the
** string pool IDs must be compared.
*/



#ifndef STRPOOL_H
#define STRPOOL_H



/* common */
#include "hashtab.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Opaque string pool entry */
typedef struct StringPoolEntry StringPoolEntry;

/* A string pool */
typedef struct StringPool StringPool;



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



StringPool* NewStringPool (unsigned HashSlots);
/* Allocate, initialize and return a new string pool */

void FreeStringPool (StringPool* P);
/* Free a string pool */

const StrBuf* SP_Get (const StringPool* P, unsigned Index);
/* Return a string from the pool. Index must exist, otherwise FAIL is called. */

unsigned SP_Add (StringPool* P, const StrBuf* S);
/* Add a string buffer to the buffer and return the index. If the string does
** already exist in the pool, SP_AddBuf will just return the index of the
** existing string.
*/

unsigned SP_AddStr (StringPool* P, const char* S);
/* Add a string to the buffer and return the index. If the string does already
** exist in the pool, SP_Add will just return the index of the existing string.
*/

unsigned SP_GetCount (const StringPool* P);
/* Return the number of strings in the pool */



/* End of strpool.h */

#endif
