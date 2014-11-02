/*****************************************************************************/
/*                                                                           */
/*                                 sizeof.c                                  */
/*                                                                           */
/*                      Handle sizes of types and data                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewit                                       */
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



/* common */
#include "addrsize.h"

/* ca65 */
#include "expr.h"
#include "sizeof.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The name of the symbol used to encode the size. The name of this entry is
** choosen so that it cannot be accessed by the user.
*/
static const StrBuf SizeEntryName = LIT_STRBUF_INITIALIZER (".size");



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int IsSizeOfSymbol (const SymEntry* Sym)
/* Return true if the given symbol is the one that encodes the size of some
** entity. Sym may also be a NULL pointer in which case false is returned.
*/
{
    return (Sym != 0 && SB_Compare (GetSymName (Sym), &SizeEntryName) == 0);
}



SymEntry* FindSizeOfScope (SymTable* Scope)
/* Get the size of a scope. The function returns the symbol table entry that
** encodes the size or NULL if there is no such entry.
*/
{
    return SymFind (Scope, &SizeEntryName, SYM_FIND_EXISTING);
}



SymEntry* FindSizeOfSymbol (SymEntry* Sym)
/* Get the size of a symbol table entry. The function returns the symbol table
** entry that encodes the size of the symbol or NULL if there is no such entry.
*/
{
    return SymFindLocal (Sym, &SizeEntryName, SYM_FIND_EXISTING);
}



SymEntry* GetSizeOfScope (SymTable* Scope)
/* Get the size of a scope. The function returns the symbol table entry that
** encodes the size, and will create a new entry if it does not exist.
*/
{
    return SymFind (Scope, &SizeEntryName, SYM_ALLOC_NEW);
}



SymEntry* GetSizeOfSymbol (SymEntry* Sym)
/* Get the size of a symbol table entry. The function returns the symbol table
** entry that encodes the size of the symbol and will create a new one if it
** does not exist.
*/
{
    return SymFindLocal (Sym, &SizeEntryName, SYM_ALLOC_NEW);
}



SymEntry* DefSizeOfScope (SymTable* Scope, long Size)
/* Define the size of a scope and return the size symbol */
{
    SymEntry* SizeSym = GetSizeOfScope (Scope);
    SymDef (SizeSym, GenLiteralExpr (Size), ADDR_SIZE_DEFAULT, SF_NONE);
    return SizeSym;
}



SymEntry* DefSizeOfSymbol (SymEntry* Sym, long Size)
/* Define the size of a symbol and return the size symbol */
{
    SymEntry* SizeSym = GetSizeOfSymbol (Sym);
    SymDef (SizeSym, GenLiteralExpr (Size), ADDR_SIZE_DEFAULT, SF_NONE);
    return SizeSym;
}
