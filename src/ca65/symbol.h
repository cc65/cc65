/*****************************************************************************/
/*                                                                           */
/*                                 symbol.h                                  */
/*                                                                           */
/*                   Parse a symbol name and search for it                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



#ifndef SYMBOL_H
#define SYMBOL_H



/* cc65 */
#include "symtab.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct StrBuf;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



struct SymTable* ParseScopedIdent (struct StrBuf* Name, struct StrBuf* FullName);
/* Parse a (possibly scoped) identifer. The scope of the name must exist and
** is returned as function result, while the last part (the identifier) which
** may be either a symbol or a scope depending on the context is returned in
** Name. FullName is a string buffer that is used to store the full name of
** the identifier including the scope. It is used internally and may be used
** by the caller for error messages or similar.
*/

struct SymEntry* ParseScopedSymName (SymFindAction Action);
/* Parse a (possibly scoped) symbol name, search for it in the symbol table
** and return the symbol table entry.
*/

struct SymTable* ParseScopedSymTable (void);
/* Parse a (possibly scoped) symbol table (scope) name, search for it in the
** symbol space and return the symbol table struct.
*/

struct SymEntry* ParseAnySymName (SymFindAction Action);
/* Parse a cheap local symbol or a a (possibly scoped) symbol name, search
** for it in the symbol table and return the symbol table entry.
*/



/* End of symbol.h */

#endif
