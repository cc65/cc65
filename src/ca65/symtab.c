/*****************************************************************************/
/*                                                                           */
/*				   symtab.c				     */
/*                                                                           */
/*		   Symbol table for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#include <string.h>

/* common */
#include "check.h"
#include "hashstr.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ca65 */
#include "global.h"
#include "error.h"
#include "expr.h"
#include "objfile.h"
#include "scanner.h"
#include "symtab.h"



/*****************************************************************************/
/*	     	    	   	     Data				     */
/*****************************************************************************/



/* Bits for the Flags value in SymEntry */
#define SF_USER		0x0001		/* User bit */
#define SF_TRAMPOLINE  	0x0002		/* Trampoline entry */
#define SF_EXPORT      	0x0004		/* Export this symbol */
#define SF_IMPORT   	0x0008		/* Import this symbol */
#define SF_GLOBAL	0x0010		/* Global symbol */
#define SF_INITIALIZER	0x0020		/* Exported initializer */
#define SF_ZP  	       	0x0040		/* Declared as zeropage symbol */
#define SF_ABS		0x0080 		/* Declared as absolute symbol */
#define SF_INDEXED	0x0800		/* Index is valid */
#define SF_CONST    	0x1000		/* The symbol has a constant value */
#define SF_MULTDEF     	0x2000		/* Multiply defined symbol */
#define	SF_DEFINED  	0x4000 	       	/* Defined */
#define SF_REFERENCED	0x8000 	       	/* Referenced */

/* Combined stuff */
#define SF_UNDEFMASK	(SF_REFERENCED | SF_DEFINED | SF_IMPORT)
#define SF_UNDEFVAL	(SF_REFERENCED)
#define SF_IMPMASK	(SF_TRAMPOLINE | SF_IMPORT | SF_REFERENCED)
#define SF_IMPVAL	(SF_IMPORT | SF_REFERENCED)
#define SF_EXPMASK	(SF_TRAMPOLINE | SF_EXPORT)
#define SF_EXPVAL	(SF_EXPORT)
#define SF_DBGINFOMASK	(SF_TRAMPOLINE | SF_DEFINED | SF_EXPORT | SF_IMPORT)
#define SF_DBGINFOVAL 	(SF_DEFINED)

/* Structure of a symbol table entry */
struct SymEntry_ {
    SymEntry*  	      	    Left;      	/* Lexically smaller entry */
    SymEntry* 	    	    Right; 	/* Lexically larger entry */
    SymEntry*	    	    List;	/* List of all entries */
    SymEntry*  	       	    Locals;  	/* Root of subtree for local symbols */
    struct SymTable_*	    SymTab;	/* Table this symbol is in, 0 for locals */
    FilePos    	       	    Pos;  	/* File position for this symbol */
    unsigned                Flags;	/* Symbol flags */
    unsigned	    	    Index;	/* Index of import/export entries */
    union {
        struct ExprNode_*   Expr;      	/* Expression if CONST not set */
	long	    	    Val;  	/* Value (if CONST set) */
	SymEntry*	    Sym;	/* Symbol (if trampoline entry) */
    } V;
    char       	       	    Name [1];	/* Dynamic allocation */
};



/* Definitions for the hash table */
#define MAIN_HASHTAB_SIZE   	213
#define SUB_HASHTAB_SIZE    	53
typedef struct SymTable_ SymTable;
struct SymTable_ {
    unsigned 	     	TableSlots;	/* Number of hash table slots */
    unsigned 	    	TableEntries;	/* Number of entries in the table */
    SymTable*       	BackLink;   	/* Link to enclosing scope if any */
    SymEntry*	    	Table [1];  	/* Dynamic allocation */
};



/* Arguments for SymFind */
#define SF_FIND_EXISTING 	0
#define SF_ALLOC_NEW		1



/* Symbol table variables */
static SymEntry*       	SymList = 0;  	/* List of all symbol table entries */
static SymEntry*    	SymLast = 0;   	/* Pointer to last defined symbol */
static SymTable*    	SymTab  = 0; 	/* Pointer to current symbol table */
static SymTable*	RootTab = 0;	/* Root symbol table */
static unsigned        	ImportCount = 0;/* Counter for import symbols */
static unsigned     	ExportCount = 0;/* Counter for export symbols */



/*****************************************************************************/
/*     	       	       	   Internally used functions		 	     */
/*****************************************************************************/



static int IsLocal (const char* Name)
/* Return true if Name is the name of a local symbol */
{
    return (*Name == LocalStart);
}



static SymEntry* NewSymEntry (const char* Name)
/* Allocate a symbol table entry, initialize and return it */
{
    SymEntry* S;
    unsigned Len;

    /* Get the length of the name */
    Len = strlen (Name);

    /* Allocate memory */
    S = xmalloc (sizeof (SymEntry) + Len);

    /* Initialize the entry */
    S->Left   = 0;
    S->Right  = 0;
    S->Locals = 0;
    S->SymTab = 0;
    S->Flags  = 0;
    S->V.Expr = 0;
    S->Pos    = CurPos;
    memcpy (S->Name, Name, Len+1);

    /* Insert it into the list of all entries */
    S->List = SymList;
    SymList = S;

    /* Return the initialized entry */
    return S;
}



static SymTable* NewSymTable (unsigned Size)
/* Allocate a symbol table on the heap and return it */
{
    SymTable* S;

    /* Allocate memory */
    S = xmalloc (sizeof (SymTable) + (Size-1) * sizeof (SymEntry*));

    /* Set variables and clear hash table entries */
    S->TableSlots   = Size;
    S->TableEntries = 0;
    S->BackLink     = 0;
    while (Size--) {
    	S->Table [Size] = 0;
    }

    /* Return the prepared struct */
    return S;
}



static int SearchSymTab (SymEntry* T, const char* Name, SymEntry** E)
/* Search in the given table for a name (Hash is the hash value of Name and
 * is given as parameter so that it will not get calculated twice if we search
 * in more than one table). If we find the symbol, the function will return 0
 * and put the entry pointer into E. If we did not find the symbol, and the
 * tree is empty, E is set to NULL. If the tree is not empty, E will be set to
 * the last entry, and the result of the function is <0 if the entry should
 * be inserted on the left side, and >0 if it should get inserted on the right
 * side.
 */
{
    int Cmp;

    /* Is there a tree? */
    if (T == 0) {
	*E = 0;
	return 1;
    }

    /* We have a table, search it */
    while (1) {
	/* Choose next entry */
	Cmp = strcmp (Name, T->Name);
	if (Cmp < 0 && T->Left) {
	    T = T->Left;
	} else if (Cmp > 0 && T->Right) {
	    T = T->Right;
	} else {
     	    /* Found or end of search */
	    break;
       	}
    }

    /* Return the search result */
    *E = T;
    return Cmp;
}



/*****************************************************************************/
/*     	       		  	     Code			   	     */
/*****************************************************************************/



static SymEntry* SymFind (SymTable* Tab, const char* Name, int AllocNew)
/* Find a new symbol table entry in the given table. If AllocNew is given and
 * the entry is not found, create a new one. Return the entry found, or the
 * new entry created, or - in case AllocNew is zero - return 0.
 */
{
    SymEntry* S;
    int Cmp;
    unsigned Hash;

    if (IsLocal (Name)) {

    	/* Local symbol, get the table */
    	if (!SymLast) {
    	    /* No last global, so there's no local table */
    	    Error (ERR_ILLEGAL_LOCAL_USE);
    	    if (AllocNew) {
    		return NewSymEntry (Name);
    	    } else {
    	     	return 0;
    	    }
       	}

    	/* Search for the symbol if we have a table */
        Cmp = SearchSymTab (SymLast->Locals, Name, &S);

    	/* If we found an entry, return it */
    	if (Cmp == 0) {
    	    return S;
    	}

    	if (AllocNew) {

    	    /* Otherwise create a new entry, insert and return it */
    	    SymEntry* N = NewSymEntry (Name);
    	    if (S == 0) {
    	 	SymLast->Locals = N;
    	    } else if (Cmp < 0) {
    	 	S->Left = N;
    	    } else {
    	 	S->Right = N;
    	    }
    	    return N;
    	}

    } else {

    	/* Global symbol: Get the hash value for the name */
    	Hash = HashStr (Name) % Tab->TableSlots;

	/* Search for the entry */
	Cmp = SearchSymTab (Tab->Table [Hash], Name, &S);

	/* If we found an entry, return it */
	if (Cmp == 0) {
	    /* Check for a trampoline entry, in this case return the real
	     * symbol.
	     */
	    if (S->Flags & SF_TRAMPOLINE) {
	     	return S->V.Sym;
	    } else {
	        return S;
	    }
	}

	if (AllocNew) {

	    /* Otherwise create a new entry, insert and return it */
	    SymEntry* N = NewSymEntry (Name);
	    if (S == 0) {
	     	Tab->Table [Hash] = N;
	    } else if (Cmp < 0) {
	     	S->Left = N;
	    } else {
	     	S->Right = N;
	    }
       	    N->SymTab = Tab;
	    ++Tab->TableEntries;
	    return N;

	}
    }

    /* We did not find the entry and AllocNew is false. */
    return 0;
}



static SymEntry* SymFindAny (SymTable* Tab, const char* Name)
/* Find a symbol in any table */
{
    SymEntry* Sym;
    do {
	/* Search in the current table */
	Sym = SymFind (Tab, Name, 0);
	if (Sym) {
	    /* Found, return it */
	    return Sym;
	} else {
	    /* Not found, search in the backlink, if we have one */
	    Tab = Tab->BackLink;
	}
    } while (Sym == 0 && Tab != 0);

    /* Not found */
    return 0;
}



static SymEntry* SymRefInternal (SymTable* Table, const char* Name)
/* Search for the symbol in the given table and return it */
{
    /* Try to find the symbol, create a new one if the symbol does not exist */
    SymEntry* S = SymFind (Table, Name, SF_ALLOC_NEW);

    /* Mark the symbol as referenced */
    S->Flags |= SF_REFERENCED;

    /* Return it */
    return S;
}



void SymEnterLevel (void)
/* Enter a new lexical level */
{
    if (RootTab == 0) {
       	/* Create the main symbol table */
       	RootTab = SymTab = NewSymTable (MAIN_HASHTAB_SIZE);
    } else {
       	/* Create a local symbol table */
       	SymTable* LocalSyms;
	LocalSyms = NewSymTable (SUB_HASHTAB_SIZE);
	LocalSyms->BackLink = SymTab;
       	SymTab = LocalSyms;
    }
}



void SymLeaveLevel (void)
/* Leave the current lexical level */
{
    SymTab = SymTab->BackLink;
}



void SymDef (const char* Name, ExprNode* Expr, int ZP)
/* Define a new symbol */
{
    /* Do we have such a symbol? */
    SymEntry* S = SymFind (SymTab, Name, SF_ALLOC_NEW);
    if (S->Flags & SF_IMPORT) {
       	/* Defined symbol is marked as imported external symbol */
       	Error (ERR_SYM_ALREADY_IMPORT);
       	return;
    }
    if (S->Flags & SF_DEFINED) {
       	/* Multiple definition */
       	Error (ERR_SYM_ALREADY_DEFINED, Name);
       	S->Flags |= SF_MULTDEF;
       	return;
    }

    /* Set the symbol data */
    if (IsConstExpr (Expr)) {
       	/* Expression is const, store the value */
       	S->Flags |= SF_CONST;
       	S->V.Val = GetExprVal (Expr);
       	FreeExpr (Expr);
    } else {
       	/* Not const, store the expression */
        S->V.Expr  = Expr;
    }
    S->Flags |= SF_DEFINED;
    if (ZP) {
	S->Flags |= SF_ZP;
    }

    /* If the symbol is a ZP symbol, check if the value is in correct range */
    if (S->Flags & SF_ZP) {
     	/* Already marked as ZP symbol by some means */
     	if (!IsByteExpr (Expr)) {
     	    Error (ERR_RANGE);
     	}
    }

    /* If this is not a local symbol, remember it as the last global one */
    if (!IsLocal (Name)) {
       	SymLast = S;
    }
}



SymEntry* SymRef (const char* Name)
/* Search for the symbol and return it */
{
    /* Reference the symbol in the current table */
    return SymRefInternal (SymTab, Name);
}



SymEntry* SymRefGlobal (const char* Name)
/* Search for the symbol in the global namespace and return it */
{
    /* Reference the symbol in the current table */
    return SymRefInternal (RootTab, Name);
}



void SymImport (const char* Name, int ZP)
/* Mark the given symbol as an imported symbol */
{
    SymEntry* S;

    /* Don't accept local symbols */
    if (IsLocal (Name)) {
     	Error (ERR_ILLEGAL_LOCAL_USE);
     	return;
    }

    /* Do we have such a symbol? */
    S = SymFind (SymTab, Name, SF_ALLOC_NEW);
    if (S->Flags & SF_DEFINED) {
     	Error (ERR_SYM_ALREADY_DEFINED, Name);
     	S->Flags |= SF_MULTDEF;
     	return;
    }
    if (S->Flags & SF_EXPORT) {
     	/* The symbol is already marked as exported symbol */
     	Error (ERR_SYM_ALREADY_EXPORT);
     	return;
    }

    /* If the symbol is marked as global, check the symbol size, then do
     * silently remove the global flag
     */
    if (S->Flags & SF_GLOBAL) {
     	if ((ZP != 0) != ((S->Flags & SF_ZP) != 0)) {
     	    Error (ERR_SYM_REDECL_MISMATCH);
     	}
        S->Flags &= ~SF_GLOBAL;
    }

    /* Set the symbol data */
    S->Flags |= SF_IMPORT;
    if (ZP) {
     	S->Flags |= SF_ZP;
    }
}



void SymExport (const char* Name, int ZP)
/* Mark the given symbol as an exported symbol */
{
    SymEntry* S;

    /* Don't accept local symbols */
    if (IsLocal (Name)) {
     	Error (ERR_ILLEGAL_LOCAL_USE);
     	return;
    }

    /* Do we have such a symbol? */
    S = SymFind (SymTab, Name, SF_ALLOC_NEW);
    if (S->Flags & SF_IMPORT) {
     	/* The symbol is already marked as imported external symbol */
     	Error (ERR_SYM_ALREADY_IMPORT);
     	return;
    }

    /* If the symbol is marked as global, check the symbol size, then do
     * silently remove the global flag
     */
    if (S->Flags & SF_GLOBAL) {
     	if ((ZP != 0) != ((S->Flags & SF_ZP) != 0)) {
     	    Error (ERR_SYM_REDECL_MISMATCH);
     	}
        S->Flags &= ~SF_GLOBAL;
    }

    /* Set the symbol data */
    S->Flags |= SF_EXPORT | SF_REFERENCED;
    if (ZP) {
     	S->Flags |= SF_ZP;
    }
}



void SymGlobal (const char* Name, int ZP)
/* Mark the given symbol as a global symbol, that is, as a symbol that is
 * either imported or exported.
 */
{
    SymEntry* S;

    /* Don't accept local symbols */
    if (IsLocal (Name)) {
     	Error (ERR_ILLEGAL_LOCAL_USE);
     	return;
    }

    /* Search for this symbol, create a new entry if needed */
    S = SymFind (SymTab, Name, SF_ALLOC_NEW);

    /* If the symbol is already marked as import or export, check the
     * size of the definition, then bail out. */
    if (S->Flags & SF_IMPORT || S->Flags & SF_EXPORT) {
     	if ((ZP != 0) != ((S->Flags & SF_ZP) != 0)) {
     	    Error (ERR_SYM_REDECL_MISMATCH);
     	}
     	return;
    }

    /* Mark the symbol */
    S->Flags |= SF_GLOBAL;
    if (ZP) {
     	S->Flags |= SF_ZP;
    }
}



void SymInitializer (const char* Name, int ZP)
/* Mark the given symbol as an initializer. This will also mark the symbol as
 * an export. Initializers may never be zero page symbols, the ZP parameter
 * is supplied to make the prototype the same as the other functions (this
 * is used in pseudo.c). Passing something else but zero as ZP argument will
 * trigger an internal error.
 */
{
    SymEntry* S;

    /* Check the ZP parameter */
    CHECK (ZP == 0);

    /* Don't accept local symbols */
    if (IsLocal (Name)) {
     	Error (ERR_ILLEGAL_LOCAL_USE);
     	return;
    }

    /* Do we have such a symbol? */
    S = SymFind (SymTab, Name, SF_ALLOC_NEW);
    if (S->Flags & SF_IMPORT) {
     	/* The symbol is already marked as imported external symbol */
     	Error (ERR_SYM_ALREADY_IMPORT);
     	return;
    }

    /* If the symbol is marked as global, check the symbol size, then do
     * silently remove the global flag
     */
    if (S->Flags & SF_GLOBAL) {
       	if ((S->Flags & SF_ZP) != 0) {
     	    Error (ERR_SYM_REDECL_MISMATCH);
     	}
        S->Flags &= ~SF_GLOBAL;
    }

    /* Set the symbol data */
    S->Flags |= SF_EXPORT | SF_INITIALIZER | SF_REFERENCED;
}



int SymIsDef (const char* Name)
/* Return true if the given symbol is already defined */
{
    SymEntry* S = SymFindAny (SymTab, Name);
    return S != 0 && (S->Flags & (SF_DEFINED | SF_IMPORT)) != 0;
}



int SymIsRef (const char* Name)
/* Return true if the given symbol has been referenced */
{
    SymEntry* S = SymFindAny (SymTab, Name);
    return S != 0 && (S->Flags & SF_REFERENCED) != 0;
}



int SymIsConst (SymEntry* S)
/* Return true if the given symbol has a constant value */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
    	S = S->V.Sym;
    }

    /* Check for constness */
    if (S->Flags & SF_CONST) {
    	return 1;
    } else if ((S->Flags & SF_DEFINED) && IsConstExpr (S->V.Expr)) {
    	/* Constant expression, remember the value */
    	ExprNode* Expr = S->V.Expr;
       	S->Flags |= SF_CONST;
    	S->V.Val = GetExprVal (Expr);
    	FreeExpr (Expr);
    	return 1;
    }
    return 0;
}



int SymIsZP (SymEntry* S)
/* Return true if the symbol is explicitly marked as zeropage symbol */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    /* If the symbol is not a global symbol, was not defined before, check the
     * enclosing scope for a symbol with the same name, and return the ZP
     * attribute of this symbol if we find one.
     */
    if (!IsLocal (S->Name)  	      					&&
	(S->Flags & (SF_ZP | SF_ABS | SF_DEFINED | SF_IMPORT)) == 0     &&
	S->SymTab->BackLink != 0) {

	/* Try to find a symbol with the same name in the enclosing scope */
	SymEntry* E = SymFindAny (S->SymTab->BackLink, S->Name);

	/* If we found one, use the ZP flag */
	if (E && (E->Flags & SF_ZP) != 0) {
	    S->Flags |= SF_ZP;
	}
    }

    /* Check the ZP flag */
    return (S->Flags & SF_ZP) != 0;
}



int SymIsImport (SymEntry* S)
/* Return true if the given symbol is marked as import */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    /* Check the import flag */
    return (S->Flags & SF_IMPORT) != 0;
}



int SymHasExpr (SymEntry* S)
/* Return true if the given symbol has an associated expression */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    /* Check the expression */
    return ((S->Flags & SF_DEFINED) != 0 &&
	    (S->Flags & SF_IMPORT)  == 0 &&
	    (S->Flags & SF_CONST)   == 0);
}



void SymFinalize (SymEntry* S)
/* Finalize a symbol expression if there is one */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    /* Check if we have an expression */
    if (SymHasExpr (S)) {
	S->V.Expr = FinalizeExpr (S->V.Expr);
    }
}



void SymMarkUser (SymEntry* S)
/* Set a user mark on the specified symbol */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    /* Set the bit */
    S->Flags |= SF_USER;
}



void SymUnmarkUser (SymEntry* S)
/* Remove a user mark from the specified symbol */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    /* Reset the bit */
    S->Flags &= ~SF_USER;
}



int SymHasUserMark (SymEntry* S)
/* Return the state of the user mark for the specified symbol */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    /* Check the bit */
    return (S->Flags & SF_USER) != 0;
}



long GetSymVal (SymEntry* S)
/* Return the symbol value */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    PRECONDITION ((S->Flags & SF_DEFINED) != 0 && (S->Flags & SF_CONST) != 0);
    return S->V.Val;
}



ExprNode* GetSymExpr (SymEntry* S)
/* Get the expression for a non-const symbol */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    PRECONDITION (S != 0 && (S->Flags & SF_CONST) == 0);
    return S->V.Expr;
}



const char* GetSymName (SymEntry* S)
/* Return the name of the symbol */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }
    return S->Name;
}



unsigned GetSymIndex (SymEntry* S)
/* Return the symbol index for the given symbol */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }
    PRECONDITION (S != 0 && (S->Flags & SF_INDEXED));
    return S->Index;
}



const FilePos* GetSymPos (SymEntry* S)
/* Return the position of first occurence in the source for the given symbol */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }
    PRECONDITION (S != 0);
    return &S->Pos;
}



static void SymCheckUndefined (SymEntry* S)
/* Handle an undefined symbol */
{
    /* Undefined symbol. It may be...
     *
     *   - An undefined symbol in a nested lexical level. In this
     *     case, search for the symbol in the higher levels and
     * 	   make the entry a trampoline entry if we find one.
     *
     *   - If the symbol is not found, it is a real undefined symbol.
     *     If the AutoImport flag is set, make it an import. If the
     *     AutoImport flag is not set, it's an error.
     */
    SymEntry* Sym = 0;
    if (S->SymTab) {
	/* It's a global symbol, get the higher level table */
	SymTable* Tab = S->SymTab->BackLink;
	while (Tab) {
	    Sym = SymFindAny (Tab, S->Name);
	    if (Sym) {
	       	if (Sym->Flags & (SF_DEFINED | SF_IMPORT)) {
	       	    /* We've found a symbol in a higher level that is
	       	     * either defined in the source, or an import.
	       	     */
	       	     break;
	       	} else {
	       	    /* The symbol found is undefined itself. Look further */
	       	    Tab = Sym->SymTab->BackLink;
	       	}
	    } else {
	       	/* No symbol found */
	       	break;
	    }
	}
    }
    if (Sym) {
	/* We found the symbol in a higher level. Make S a trampoline
	 * symbol. Beware: We have to transfer the symbol attributes to
	 * the real symbol and check for any conflicts.
	 */
	S->Flags |= SF_TRAMPOLINE;
	S->V.Sym = Sym;

	/* Transfer the flags. Note: S may not be imported, since in that
	 * case it wouldn't be undefined.
	 */
       	if (S->Flags & SF_EXPORT) {
	    if (Sym->Flags & SF_IMPORT) {
	       	/* The symbol is already marked as imported external symbol */
	       	PError (&S->Pos, ERR_SYM_ALREADY_IMPORT);
	    }
	    Sym->Flags |= S->Flags & (SF_EXPORT | SF_ZP);
	}

	/* Transfer the referenced flag */
	Sym->Flags |= (S->Flags & SF_REFERENCED);

    } else {
	/* The symbol is definitely undefined */
	if (S->Flags & SF_EXPORT) {
	    /* We will not auto-import an export */
	    PError (&S->Pos, ERR_EXPORT_UNDEFINED, S->Name);
	} else {
	    if (AutoImport) {
		/* Mark as import, will be indexed later */
		S->Flags |= SF_IMPORT;
	    } else {
		/* Error */
	        PError (&S->Pos, ERR_SYM_UNDEFINED, S->Name);
	    }
	}
    }
}



void SymCheck (void)
/* Run through all symbols and check for anomalies and errors */
{
    SymEntry* S;

    /* Check for open lexical levels */
    if (SymTab->BackLink != 0) {
	Error (ERR_OPEN_PROC);
    }

    /* First pass: Walk through all symbols, checking for undefined's and
     * changing them to trampoline symbols or make them imports.
     */
    S = SymList;
    while (S) {
	/* If the symbol is marked as global, mark it as export, if it is
	 * already defined, otherwise mark it as import.
	 */
	if (S->Flags & SF_GLOBAL) {
	    S->Flags &= ~SF_GLOBAL;
	    if (S->Flags & SF_DEFINED) {
		S->Flags |= SF_EXPORT;
	    } else {
		S->Flags |= SF_IMPORT;
	    }
	}

	/* Handle undefined symbols */
       	if ((S->Flags & SF_UNDEFMASK) == SF_UNDEFVAL) {
	    /* This is an undefined symbol. Handle it. */
	    SymCheckUndefined (S);
	}

	/* Next symbol */
	S = S->List;
    }

    /* Second pass: Walk again through the symbols. Ignore undefined's, since
     * we handled them in the last pass, and ignore trampoline symbols, since
     * we handled them in the last pass, too.
     */
    S = SymList;
    while (S) {
	if ((S->Flags & SF_TRAMPOLINE) == 0 &&
	    (S->Flags & SF_UNDEFMASK) != SF_UNDEFVAL) {
	    if ((S->Flags & SF_DEFINED) != 0 && (S->Flags & SF_REFERENCED) == 0) {
		/* Symbol was defined but never referenced */
		PWarning (&S->Pos, WARN_SYM_NOT_REFERENCED, S->Name);
	    }
	    if (S->Flags & SF_IMPORT) {
		if ((S->Flags & SF_REFERENCED) == 0) {
		    /* Imported symbol is not referenced */
		    PWarning (&S->Pos, WARN_IMPORT_NOT_REFERENCED, S->Name);
		} else {
		    /* Give the import an index, count imports */
		    S->Index = ImportCount++;
		    S->Flags |= SF_INDEXED;
		}
	    }
	    if (S->Flags & SF_EXPORT) {
		/* Give the export an index, count exports */
		S->Index = ExportCount++;
		S->Flags |= SF_INDEXED;
	    }
	}

	/* Next symbol */
     	S = S->List;
    }
}



void SymDump (FILE* F)
/* Dump the symbol table */
{
    SymEntry* S = SymList;

    while (S) {
	/* Ignore trampoline symbols */
	if ((S->Flags & SF_TRAMPOLINE) != 0) {
	    printf ("%-24s %s %s %s %s %s\n",
		    S->Name,
		    (S->Flags & SF_DEFINED)? "DEF" : "---",
		    (S->Flags & SF_REFERENCED)? "REF" : "---",
		    (S->Flags & SF_IMPORT)? "IMP" : "---",
		    (S->Flags & SF_EXPORT)? "EXP" : "---",
		    (S->Flags & SF_ZP)? "ZP" : "--");
	}
	/* Next symbol */
	S = S->List;
    }
}



void WriteImports (void)
/* Write the imports list to the object file */
{
    SymEntry* S;

    /* Tell the object file module that we're about to start the imports */
    ObjStartImports ();

    /* Write the import count to the list */
    ObjWriteVar (ImportCount);

    /* Walk throught list and write all imports to the file */
    S = SymList;
    while (S) {
     	if ((S->Flags & SF_IMPMASK) == SF_IMPVAL) {
     	    if (S->Flags & SF_ZP) {
     		ObjWrite8 (IMP_ZP);
     	    } else {
     		ObjWrite8 (IMP_ABS);
     	    }
       	    ObjWriteStr (S->Name);
     	    ObjWritePos (&S->Pos);
     	}
     	S = S->List;
    }

    /* Done writing imports */
    ObjEndImports ();
}



void WriteExports (void)
/* Write the exports list to the object file */
{
    SymEntry* S;

    /* Tell the object file module that we're about to start the exports */
    ObjStartExports ();

    /* Write the export count to the list */
    ObjWriteVar (ExportCount);

    /* Walk throught list and write all exports to the file */
    S = SymList;
    while (S) {
       	if ((S->Flags & SF_EXPMASK) == SF_EXPVAL) {
       	    unsigned char ExprMask;

	    /* Finalize an associated expression if we have one */
	    SymFinalize (S);

	    /* Check if the symbol is const */
	    ExprMask = (SymIsConst (S))? EXP_CONST : EXP_EXPR;

	    /* Add zeropage/abs bits */
	    ExprMask |= (S->Flags & SF_ZP)? EXP_ZP : EXP_ABS;

	    /* Add the initializer bits */
	    if (S->Flags & SF_INITIALIZER) {
	     	ExprMask |= EXP_INITIALIZER;
	    }

	    /* Write the type */
	    ObjWrite8 (ExprMask);

	    /* Write the name */
       	    ObjWriteStr (S->Name);

	    /* Write the value */
	    if ((ExprMask & EXP_MASK_VAL) == EXP_CONST) {
	     	/* Constant value */
	     	ObjWrite32 (S->V.Val);
	    } else {
	     	/* Expression involved */
	        WriteExpr (S->V.Expr);
            }

	    /* Write the source file position */
	    ObjWritePos (&S->Pos);
	}
     	S = S->List;
    }

    /* Done writing exports */
    ObjEndExports ();
}



void WriteDbgSyms (void)
/* Write a list of all symbols to the object file */
{
    unsigned Count;
    SymEntry* S;

    /* Tell the object file module that we're about to start the debug info */
    ObjStartDbgSyms ();

    /* Check if debug info is requested */
    if (DbgSyms) {

	/* Walk through the list and count the symbols */
	Count = 0;
	S = SymList;
	while (S) {
	    if ((S->Flags & SF_DBGINFOMASK) == SF_DBGINFOVAL) {
		++Count;
	    }
	    S = S->List;
	}

	/* Write the symbol count to the list */
       	ObjWriteVar (Count);

       	/* Walk through list and write all symbols to the file */
	S = SymList;
	while (S) {
	    if ((S->Flags & SF_DBGINFOMASK) == SF_DBGINFOVAL) {
		unsigned char ExprMask;

		/* Finalize an associated expression if we have one */
		SymFinalize (S);

		/* Check if the symbol is const */
		ExprMask = (SymIsConst (S))? EXP_CONST : EXP_EXPR;

		/* Add zeropage/abs bits */
		ExprMask |= (S->Flags & SF_ZP)? EXP_ZP : EXP_ABS;

		/* Add the initializer bits */
		if (S->Flags & SF_INITIALIZER) {
		    ExprMask |= EXP_INITIALIZER;
		}

		/* Write the type */
		ObjWrite8 (ExprMask);

		/* Write the name */
		ObjWriteStr (S->Name);

		/* Write the value */
		if ((ExprMask & EXP_MASK_VAL) == EXP_CONST) {
		    /* Constant value */
		    ObjWrite32 (S->V.Val);
		} else {
		    /* Expression involved */
		    WriteExpr (S->V.Expr);
		}

		/* Write the source file position */
		ObjWritePos (&S->Pos);
	    }
	    S = S->List;
	}

    } else {

	/* No debug symbols */
	ObjWriteVar (0);

    }

    /* Done writing debug symbols */
    ObjEndDbgSyms ();
}



