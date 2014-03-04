/*****************************************************************************/
/*                                                                           */
/*                                 symtab.h                                  */
/*                                                                           */
/*              Symbol table management for the cc65 C compiler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2013, Ullrich von Bassewitz                                      */
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



#ifndef SYMTAB_H
#define SYMTAB_H



#include <stdio.h>

#include "datatype.h"
#include "symentry.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Symbol table */
typedef struct SymTable SymTable;
struct SymTable {
    SymTable*           PrevTab;        /* Pointer to higher level symbol table */
    SymEntry*           SymHead;        /* Double linked list of symbols */
    SymEntry*           SymTail;        /* Double linked list of symbols */
    unsigned            SymCount;       /* Count of symbols in this table */
    unsigned            Size;           /* Size of table */
    SymEntry*           Tab[1];         /* Actual table, dynamically allocated */
};

/* An empty symbol table */
extern SymTable         EmptySymTab;

/* Forwards */
struct FuncDesc;

/* Predefined lexical levels */
#define LEX_LEVEL_GLOBAL        1U
#define LEX_LEVEL_FUNCTION      2U



/*****************************************************************************/
/*                        Handling of lexical levels                         */
/*****************************************************************************/



unsigned GetLexicalLevel (void);
/* Return the current lexical level */

void EnterGlobalLevel (void);
/* Enter the program global lexical level */

void LeaveGlobalLevel (void);
/* Leave the program global lexical level */

void EnterFunctionLevel (void);
/* Enter function lexical level */

void RememberFunctionLevel (struct FuncDesc* F);
/* Remember the symbol tables for the level and leave the level without checks */

void ReenterFunctionLevel (struct FuncDesc* F);
/* Reenter the function lexical level using the existing tables from F */

void LeaveFunctionLevel (void);
/* Leave function lexical level */

void EnterBlockLevel (void);
/* Enter a nested block in a function */

void LeaveBlockLevel (void);
/* Leave a nested block in a function */

void EnterStructLevel (void);
/* Enter a nested block for a struct definition */

void LeaveStructLevel (void);
/* Leave a nested block for a struct definition */



/*****************************************************************************/
/*                              Find functions                               */
/*****************************************************************************/



SymEntry* FindSym (const char* Name);
/* Find the symbol with the given name */

SymEntry* FindGlobalSym (const char* Name);
/* Find the symbol with the given name in the global symbol table only */

SymEntry* FindLocalSym (const char* Name);
/* Find the symbol with the given name in the current symbol table only */

SymEntry* FindTagSym (const char* Name);
/* Find the symbol with the given name in the tag table */

SymEntry* FindStructField (const Type* TypeArray, const char* Name);
/* Find a struct field in the fields list */



/*****************************************************************************/
/*                       Add stuff to the symbol table                       */
/*****************************************************************************/



SymEntry* AddStructSym (const char* Name, unsigned Type, unsigned Size, SymTable* Tab);
/* Add a struct/union entry and return it */

SymEntry* AddBitField (const char* Name, unsigned Offs, unsigned BitOffs, unsigned Width);
/* Add a bit field to the local symbol table and return the symbol entry */

SymEntry* AddConstSym (const char* Name, const Type* T, unsigned Flags, long Val);
/* Add an constant symbol to the symbol table and return it */

SymEntry* AddLabelSym (const char* Name, unsigned Flags);
/* Add a goto label to the symbol table */

SymEntry* AddLocalSym (const char* Name, const Type* T, unsigned Flags, int Offs);
/* Add a local symbol and return the symbol entry */

SymEntry* AddGlobalSym (const char* Name, const Type* T, unsigned Flags);
/* Add an external or global symbol to the symbol table and return the entry */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SymTable* GetSymTab (void);
/* Return the current symbol table */

SymTable* GetGlobalSymTab (void);
/* Return the global symbol table */

int SymIsLocal (SymEntry* Sym);
/* Return true if the symbol is defined in the highest lexical level */

void MakeZPSym (const char* Name);
/* Mark the given symbol as zero page symbol */

void PrintSymTable (const SymTable* Tab, FILE* F, const char* Header, ...);
/* Write the symbol table to the given file */

void EmitExternals (void);
/* Write import/export statements for external symbols */

void EmitDebugInfo (void);
/* Emit debug infos for the locals of the current scope */



/* End of symtab.h */

#endif
