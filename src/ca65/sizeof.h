/*****************************************************************************/
/*                                                                           */
/*                                 sizeof.h                                  */
/*                                                                           */
/*                      Handle sizes of types and data                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
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



#ifndef SIZEOF_H
#define SIZEOF_H



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct SymEntry;
struct SymTable;



/*****************************************************************************/
/*     	       	    		     Code	   			     */
/*****************************************************************************/



struct SymEntry* GetSizeOfScope (struct SymTable* Scope);
/* Get the size of a scope. The function returns the symbol table entry that
 * encodes the size, and will create a new entry if it does not exist.
 */

struct SymEntry* GetSizeOfSymbol (struct SymEntry* Sym);
/* Get the size of a symbol table entry. The function returns the symbol table
 * entry that encodes the size of the symbol and will create a new one if it
 * does not exist.
 */

struct SymEntry* DefSizeOfScope (struct SymTable* Scope, long Size);
/* Define the size of a scope and return the size symbol */

struct SymEntry* DefSizeOfSymbol (struct SymEntry* Sym, long Size);
/* Define the size of a symbol and return the size symbol */



/* End of sizeof.h */
#endif



