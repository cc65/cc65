/*****************************************************************************/
/*                                                                           */
/*                                 litpool.h                                 */
/*                                                                           */
/*              Literal string handling for the cc65 C compiler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2009, Ullrich von Bassewitz                                      */
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



#ifndef LITPOOL_H
#define LITPOOL_H



#include <stdio.h>

/* common */
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forward for struct SymEntry */
struct SymEntry;

/* Forward for a literal */
typedef struct Literal Literal;

/* Forward for a literal pool */
typedef struct LiteralPool LiteralPool;



/*****************************************************************************/
/*                              struct Literal                               */
/*****************************************************************************/



Literal* UseLiteral (Literal* L);
/* Increase the reference counter for the literal and return it */

void ReleaseLiteral (Literal* L);
/* Decrement the reference counter for the literal */

void TranslateLiteral (Literal* L);
/* Translate a literal into the target charset. */

unsigned GetLiteralLabel (const Literal* L);
/* Return the asm label for a literal */

const char* GetLiteralStr (const Literal* L);
/* Return the data for a literal as pointer to char */

const StrBuf* GetLiteralStrBuf (const Literal* L);
/* Return the data for a literal as pointer to the string buffer */

unsigned GetLiteralSize (const Literal* L);
/* Get the size of a literal string */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InitLiteralPool (void);
/* Initialize the literal pool */

void PushLiteralPool (struct SymEntry* Func);
/* Push the current literal pool onto the stack and create a new one */

LiteralPool* PopLiteralPool (void);
/* Pop the last literal pool from TOS and activate it. Return the old
** literal pool.
*/

void MoveLiteralPool (LiteralPool* LocalPool);
/* Move all referenced literals in LocalPool to the global literal pool. This
** function will free LocalPool after moving the used string literals.
*/

void OutputLiteralPool (void);
/* Output the literal pool */

Literal* AddLiteral (const char* S);
/* Add a literal string to the literal pool. Return the literal. */

Literal* AddLiteralBuf (const void* Buf, unsigned Len);
/* Add a buffer containing a literal string to the literal pool. Return the
** literal.
*/

Literal* AddLiteralStr (const StrBuf* S);
/* Add a literal string to the literal pool. Return the literal. */



/* End of litpool.h */

#endif
