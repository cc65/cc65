/*****************************************************************************/
/*                                                                           */
/*                                 symtab.h                                  */
/*                                                                           */
/*                 Symbol table for the ca65 macroassembler                  */
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



#ifndef SYMTAB_H
#define SYMTAB_H



#include <stdio.h>

/* common */
#include "exprdefs.h"
#include "inline.h"

/* ca65 */
#include "symentry.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Arguments for SymFind... */
typedef enum {
    SYM_FIND_EXISTING   = 0x00,
    SYM_ALLOC_NEW       = 0x01,
    SYM_CHECK_ONLY      = 0x02,
} SymFindAction;

/* Symbol table flags */
#define ST_NONE         0x00            /* No flags */
#define ST_DEFINED      0x01            /* Scope has been defined */
#define ST_CLOSED       0x02            /* Scope is closed */

/* A symbol table */
typedef struct SymTable SymTable;
struct SymTable {
    SymTable*           Next;           /* Pointer to next table in list */
    SymTable*           Left;           /* Pointer to smaller entry */
    SymTable*           Right;          /* Pointer to greater entry */
    SymTable*           Parent;         /* Link to enclosing scope if any */
    SymTable*           Childs;         /* Pointer to child scopes */
    SymEntry*           Label;          /* Scope label */
    Collection          Spans;          /* Spans for this scope */
    unsigned            Id;             /* Scope id */
    unsigned short      Flags;          /* Symbol table flags */
    unsigned char       AddrSize;       /* Address size */
    unsigned char       Type;           /* Type of the scope */
    unsigned            Level;          /* Lexical level */
    unsigned            TableSlots;     /* Number of hash table slots */
    unsigned            TableEntries;   /* Number of entries in the table */
    unsigned            Name;           /* Name of the scope */
    SymEntry*           Table[1];       /* Dynamic allocation */
};

/* Symbol tables */
extern SymTable*        CurrentScope;   /* Pointer to current symbol table */
extern SymTable*        RootScope;      /* Root symbol table */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void SymEnterLevel (const StrBuf* ScopeName, unsigned char Type,
                    unsigned char AddrSize, SymEntry* OwnerSym);
/* Enter a new lexical level */

void SymLeaveLevel (void);
/* Leave the current lexical level */

SymTable* SymFindScope (SymTable* Parent, const StrBuf* Name, SymFindAction Action);
/* Find a scope in the given enclosing scope */

SymTable* SymFindAnyScope (SymTable* Parent, const StrBuf* Name);
/* Find a scope in the given or any of its parent scopes. The function will
** never create a new symbol, since this can only be done in one specific
** scope.
*/

SymEntry* SymFindLocal (SymEntry* Parent, const StrBuf* Name, SymFindAction Action);
/* Find a cheap local symbol. If Action contains SYM_ALLOC_NEW and the entry is
** not found, create a new one. Return the entry found, or the new entry
** created, or - in case Action is SYM_FIND_EXISTING - return 0.
*/

SymEntry* SymFind (SymTable* Scope, const StrBuf* Name, SymFindAction Action);
/* Find a new symbol table entry in the given table. If Action contains
** SYM_ALLOC_NEW and the entry is not found, create a new one. Return the
** entry found, or the new entry created, or - in case Action is
** SYM_FIND_EXISTING - return 0.
*/

SymEntry* SymFindAny (SymTable* Scope, const StrBuf* Name);
/* Find a symbol in the given or any of its parent scopes. The function will
** never create a new symbol, since this can only be done in one specific
** scope.
*/

#if defined(HAVE_INLINE)
INLINE unsigned char GetSymTabType (const SymTable* S)
/* Return the type of the given symbol table */
{
    return S->Type;
}
#else
#  define GetSymTabType(S)      ((S)->Type)
#endif

#if defined(HAVE_INLINE)
INLINE int SymTabIsClosed (const SymTable* S)
/* Return true if the symbol table has been closed */
{
    return (S->Flags & ST_CLOSED) != 0;
}
#else
#  define SymTabIsClosed(S)      (((S)->Flags & ST_CLOSED) != 0)
#endif

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

void WriteScopes (void);
/* Write the scope table to the object file */



/* End of symtab.h */

#endif
