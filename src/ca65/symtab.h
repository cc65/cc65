/*****************************************************************************/
/*                                                                           */
/*				   symtab.h				     */
/*                                                                           */
/*		   Symbol table for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#ifndef SYMTAB_H
#define SYMTAB_H



#include <stdio.h>

/* common */
#include "exprdefs.h"

/* ca65 */
#include "symentry.h"



/*****************************************************************************/
/*  	       		      	     Code				     */
/*****************************************************************************/



void SymEnterLevel (void);
/* Enter a new lexical level */

void SymLeaveLevel (void);
/* Leave the current lexical level */

void SymDef (const char* Name, ExprNode* Expr, int ZP);
/* Define a new symbol */

SymEntry* SymRef (const char* Name);
/* Search for the symbol and return it */

SymEntry* SymRefGlobal (const char* Name);
/* Search for the symbol in the global namespace and return it */

int SymIsDef (const char* Name);
/* Return true if the given symbol is already defined */

int SymIsRef (const char* Name);
/* Return true if the given symbol has been referenced */

void SymImport (const char* Name, int ZP);
/* Mark the given symbol as an imported symbol */

void SymExport (const char* Name, int ZP);
/* Mark the given symbol as an exported symbol */

void SymGlobal (const char* Name, int ZP);
/* Mark the given symbol as a global symbol, that is, as a symbol that is
 * either imported or exported.
 */

void SymInitializer (const char* Name, int ZP);
/* Mark the given symbol as an initializer. This will also mark the symbol as
 * an export. Initializers may never be zero page symbols, the ZP parameter
 * is supplied to make the prototype the same as the other functions (this
 * is used in pseudo.c). Passing something else but zero as ZP argument will
 * trigger an internal error.
 */

int SymIsConst (SymEntry* Sym);
/* Return true if the given symbol has a constant value */

int SymIsZP (SymEntry* Sym);
/* Return true if the symbol is explicitly marked as zeropage symbol */

int SymIsImport (SymEntry* Sym);
/* Return true if the given symbol is marked as import */

int SymHasExpr (SymEntry* Sym);
/* Return true if the given symbol has an associated expression */

void SymMarkUser (SymEntry* Sym);
/* Set a user mark on the specified symbol */

void SymUnmarkUser (SymEntry* Sym);
/* Remove a user mark from the specified symbol */

int SymHasUserMark (SymEntry* Sym);
/* Return the state of the user mark for the specified symbol */

long GetSymVal (SymEntry* Sym);
/* Return the symbol value */

ExprNode* GetSymExpr (SymEntry* Sym);
/* Get the expression for a non-const symbol */

const char* GetSymName (SymEntry* Sym);
/* Return the name of the symbol */

unsigned GetSymIndex (SymEntry* Sym);
/* Return the symbol index for the given symbol */

const FilePos* GetSymPos (SymEntry* Sym);
/* Return the position of first occurence in the source for the given symbol */

void SymCheck (void);
/* Run through all symbols and check for anomalies and errors */

void SymDump (FILE* F);
/* Dump the symbol table */

void WriteImports (void);
/* Write the imports list to the object file */

void WriteExports (void);
/* Write the exports list to the object file */

void WriteDbgSyms (void);
/* Write a list of all symbols to the object file */



/* End of symtab.h */

#endif




