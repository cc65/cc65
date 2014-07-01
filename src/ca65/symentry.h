/*****************************************************************************/
/*                                                                           */
/*                                symentry.h                                 */
/*                                                                           */
/*          Symbol table entry forward for the ca65 macroassembler           */
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



#ifndef SYMENTRY_H
#define SYMENTRY_H



/* common */
#include "cddefs.h"
#include "coll.h"
#include "filepos.h"
#include "inline.h"
#include "strbuf.h"

/* ca65 */
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forwards */
struct HLLDbgSym;

/* Bits for the Flags value in SymEntry */
#define SF_NONE         0x0000          /* Empty flag set */
#define SF_USER         0x0001          /* User bit */
#define SF_UNUSED       0x0002          /* Unused entry */
#define SF_EXPORT       0x0004          /* Export this symbol */
#define SF_IMPORT       0x0008          /* Import this symbol */
#define SF_GLOBAL       0x0010          /* Global symbol */
#define SF_LOCAL        0x0020          /* Cheap local symbol */
#define SF_LABEL        0x0040          /* Used as a label */
#define SF_VAR          0x0080          /* Variable symbol */
#define SF_FORCED       0x0100          /* Forced import, SF_IMPORT also set */
#define SF_FIXED        0x0200          /* May not be trampoline */
#define SF_MULTDEF      0x1000          /* Multiply defined symbol */
#define SF_DEFINED      0x2000          /* Defined */
#define SF_REFERENCED   0x4000          /* Referenced */

/* Combined values */
#define SF_REFIMP       (SF_REFERENCED|SF_IMPORT)       /* A ref'd import */

/* Structure of a symbol table entry */
typedef struct SymEntry SymEntry;
struct SymEntry {
    SymEntry*           Left;           /* Lexically smaller entry */
    SymEntry*           Right;          /* Lexically larger entry */
    SymEntry*           List;           /* List of all entries */
    SymEntry*           Locals;         /* Root of subtree for local symbols */
    union {
        struct SymTable*    Tab;        /* Table this symbol is in */
        struct SymEntry*    Entry;      /* Parent for cheap locals */
    } Sym;
    Collection          DefLines;       /* Line infos for definition */
    Collection          RefLines;       /* Line infos for references */
    FilePos*            GuessedUse[1];  /* File position where symbol
                                        ** address size was guessed, and the
                                        ** smallest possible addressing was NOT
                                        ** used. Currently only for zero page
                                        ** addressing
                                        */
    struct HLLDbgSym*   HLLSym;         /* Symbol from high level language */
    unsigned            Flags;          /* Symbol flags */
    unsigned            DebugSymId;     /* Debug symbol id */
    unsigned            ImportId;       /* Id of import if this is one */
    unsigned            ExportId;       /* Id of export if this is one */
    struct ExprNode*    Expr;           /* Symbol expression */
    Collection          ExprRefs;       /* Expressions using this symbol */
    unsigned char       ExportSize;     /* Export address size */
    unsigned char       AddrSize;       /* Address size of label */
    unsigned char       ConDesPrio[CD_TYPE_COUNT];      /* ConDes priorities... */
                                        /* ...actually value+1 (used as flag) */
    unsigned            Name;           /* Name index in global string pool */
};

/* List of all symbol table entries */
extern SymEntry* SymList;

/* Pointer to last defined symbol */
extern SymEntry* SymLast;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SymEntry* NewSymEntry (const StrBuf* Name, unsigned Flags);
/* Allocate a symbol table entry, initialize and return it */

int SymSearchTree (SymEntry* T, const StrBuf* Name, SymEntry** E);
/* Search in the given tree for a name. If we find the symbol, the function
** will return 0 and put the entry pointer into E. If we did not find the
** symbol, and the tree is empty, E is set to NULL. If the tree is not empty,
** E will be set to the last entry, and the result of the function is <0 if
** the entry should be inserted on the left side, and >0 if it should get
** inserted on the right side.
*/

#if defined(HAVE_INLINE)
INLINE void SymAddExprRef (SymEntry* Sym, struct ExprNode* Expr)
/* Add an expression reference to this symbol */
{
    CollAppend (&Sym->ExprRefs, Expr);
}
#else
#define SymAddExprRef(Sym,Expr)     CollAppend (&(Sym)->ExprRefs, Expr)
#endif

#if defined(HAVE_INLINE)
INLINE void SymDelExprRef (SymEntry* Sym, struct ExprNode* Expr)
/* Delete an expression reference to this symbol */
{
    CollDeleteItem (&Sym->ExprRefs, Expr);
}
#else
#define SymDelExprRef(Sym,Expr)     CollDeleteItem (&(Sym)->ExprRefs, Expr)
#endif

void SymTransferExprRefs (SymEntry* From, SymEntry* To);
/* Transfer all expression references from one symbol to another. */

void SymDef (SymEntry* Sym, ExprNode* Expr, unsigned char AddrSize, unsigned Flags);
/* Mark a symbol as defined */

void SymRef (SymEntry* Sym);
/* Mark the given symbol as referenced */

void SymImport (SymEntry* Sym, unsigned char AddrSize, unsigned Flags);
/* Mark the given symbol as an imported symbol */

void SymExport (SymEntry* Sym, unsigned char AddrSize, unsigned Flags);
/* Mark the given symbol as an exported symbol */

void SymGlobal (SymEntry* Sym, unsigned char AddrSize, unsigned Flags);
/* Mark the given symbol as a global symbol, that is, as a symbol that is
** either imported or exported.
*/

void SymConDes (SymEntry* Sym, unsigned char AddrSize, unsigned Type, unsigned Prio);
/* Mark the given symbol as a module constructor/destructor. This will also
** mark the symbol as an export. Initializers may never be zero page symbols.
*/

void SymGuessedAddrSize (SymEntry* Sym, unsigned char AddrSize);
/* Mark the address size of the given symbol as guessed. The address size
** passed as argument is the one NOT used, because the actual address size
** wasn't known. Example: Zero page addressing was not used because symbol
** is undefined, and absolute addressing was available.
*/

void SymExportFromGlobal (SymEntry* S);
/* Called at the end of assembly. Converts a global symbol that is defined
** into an export.
*/

void SymImportFromGlobal (SymEntry* S);
/* Called at the end of assembly. Converts a global symbol that is undefined
** into an import.
*/

#if defined(HAVE_INLINE)
INLINE int SymIsDef (const SymEntry* S)
/* Return true if the given symbol is already defined */
{
    return (S->Flags & SF_DEFINED) != 0;
}
#else
#  define SymIsDef(S)   (((S)->Flags & SF_DEFINED) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsRef (const SymEntry* S)
/* Return true if the given symbol has been referenced */
{
    return (S->Flags & SF_REFERENCED) != 0;
}
#else
#  define SymIsRef(S)   (((S)->Flags & SF_REFERENCED) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsImport (const SymEntry* S)
/* Return true if the given symbol is marked as import */
{
    /* Check the import flag */
    return (S->Flags & SF_IMPORT) != 0;
}
#else
#  define SymIsImport(S)  (((S)->Flags & SF_IMPORT) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsExport (const SymEntry* S)
/* Return true if the given symbol is marked as export */
{
    /* Check the export flag */
    return (S->Flags & SF_EXPORT) != 0;
}
#else
#  define SymIsExport(S)  (((S)->Flags & SF_EXPORT) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsVar (const SymEntry* S)
/* Return true if the given symbol is marked as variable */
{
    /* Check the variable flag */
    return (S->Flags & SF_VAR) != 0;
}
#else
#  define SymIsVar(S)   (((S)->Flags & SF_VAR) != 0)
#endif

int SymIsConst (const SymEntry* Sym, long* Val);
/* Return true if the given symbol has a constant value. If Val is not NULL
** and the symbol has a constant value, store it's value there.
*/

#if defined(HAVE_INLINE)
INLINE int SymHasExpr (const SymEntry* S)
/* Return true if the given symbol has an associated expression */
{
    /* Check the expression */
    return ((S->Flags & (SF_DEFINED|SF_IMPORT)) == SF_DEFINED);
}
#else
#  define SymHasExpr(S)   (((S)->Flags & (SF_DEFINED|SF_IMPORT)) == SF_DEFINED)
#endif

#if defined(HAVE_INLINE)
INLINE void SymMarkUser (SymEntry* S)
/* Set a user mark on the specified symbol */
{
    /* Set the bit */
    S->Flags |= SF_USER;
}
#else
#  define SymMarkUser(S)   ((S)->Flags |= SF_USER)
#endif

#if defined(HAVE_INLINE)
INLINE void SymUnmarkUser (SymEntry* S)
/* Remove a user mark from the specified symbol */
{
    /* Reset the bit */
    S->Flags &= ~SF_USER;
}
#else
#  define SymUnmarkUser(S)   ((S)->Flags &= ~SF_USER)
#endif

#if defined(HAVE_INLINE)
INLINE int SymHasUserMark (SymEntry* S)
/* Return the state of the user mark for the specified symbol */
{
    /* Check the bit */
    return (S->Flags & SF_USER) != 0;
}
#else
#  define SymHasUserMark(S) (((S)->Flags & SF_USER) != 0)
#endif

struct SymTable* GetSymParentScope (SymEntry* S);
/* Get the parent scope of the symbol (not the one it is defined in). Return
** NULL if the symbol is a cheap local, or defined on global level.
*/

struct ExprNode* GetSymExpr (SymEntry* Sym);
/* Get the expression for a non-const symbol */

const struct ExprNode* SymResolve (const SymEntry* Sym);
/* Helper function for DumpExpr. Resolves a symbol into an expression or return
** NULL. Do not call in other contexts!
*/

#if defined(HAVE_INLINE)
INLINE const StrBuf* GetSymName (const SymEntry* S)
/* Return the name of the symbol */
{
    return GetStrBuf (S->Name);
}
#else
#  define GetSymName(S)   GetStrBuf ((S)->Name)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned char GetSymAddrSize (const SymEntry* S)
/* Return the address size of the symbol. Beware: This function will just
** return the AddrSize member, it will not look at the expression!
*/
{
    return S->AddrSize;
}
#else
#  define GetSymAddrSize(S)   ((S)->AddrSize)
#endif

long GetSymVal (SymEntry* Sym);
/* Return the value of a symbol assuming it's constant. FAIL will be called
** in case the symbol is undefined or not constant.
*/

unsigned GetSymImportId (const SymEntry* Sym);
/* Return the import id for the given symbol */

unsigned GetSymExportId (const SymEntry* Sym);
/* Return the export id for the given symbol */

unsigned GetSymInfoFlags (const SymEntry* Sym, long* ConstVal);
/* Return a set of flags used when writing symbol information into a file.
** If the SYM_CONST bit is set, ConstVal will contain the constant value
** of the symbol. The result does not include the condes count.
** See common/symdefs.h for more information.
*/



/* End of symentry.h */

#endif
