/*****************************************************************************/
/*                                                                           */
/*				   symtab.h				     */
/*                                                                           */
/*		   Symbol table for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#ifndef SYMTAB_H
#define SYMTAB_H



#include <stdio.h>

/* common */
#include "exprdefs.h"

/* ca65 */
#include "symentry.h"



/*****************************************************************************/
/*   	       		      	     Data				     */
/*****************************************************************************/



/* Scope identifiers */
#define SCOPE_ANY       0
#define SCOPE_GLOBAL    1
#define SCOPE_LOCAL     2



/*****************************************************************************/
/*   	       		      	     Code				     */
/*****************************************************************************/



void SymEnterLevel (void);
/* Enter a new lexical level */

void SymLeaveLevel (void);
/* Leave the current lexical level */

void SymDef (const char* Name, ExprNode* Expr, int ZP, int Label);
/* Define a new symbol */

SymEntry* SymRef (const char* Name, int Scope);
/* Search for the symbol and return it */

int SymIsDef (const char* Name, int Scope);
/* Return true if the given symbol is already defined */

int SymIsRef (const char* Name, int Scope);
/* Return true if the given symbol has been referenced */

void SymImport (const char* Name);
/* Mark the given symbol as an imported symbol */

void SymImportZP (const char* Name);
/* Mark the given symbol as a imported zeropage symbol */

void SymImportForced (const char* Name);
/* Mark the given symbol as a forced imported symbol */

void SymExport (const char* Name);
/* Mark the given symbol as an exported symbol */

void SymExportZP (const char* Name);
/* Mark the given symbol as an exported zeropage symbol */

void SymGlobal (const char* Name);
/* Mark the given symbol as a global symbol, that is, as a symbol that is
 * either imported or exported.
 */
                                
void SymGlobalZP (const char* Name);
/* Mark the given symbol as a global zeropage symbol, that is, as a symbol
 * that is either imported or exported.
 */

void SymConDes (const char* Name, unsigned Type, unsigned Prio);
/* Mark the given symbol as a module constructor/destructor. This will also
 * mark the symbol as an export. Initializers may never be zero page symbols.
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




