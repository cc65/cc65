/*****************************************************************************/
/*                                                                           */
/*				   symtab.c				     */
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
#include "spool.h"
#include "symtab.h"



/*****************************************************************************/
/*	     	    	   	     Data				     */
/*****************************************************************************/



/* Combined symbol entry flags used within this module */
#define SF_UNDEFMASK	(SF_REFERENCED | SF_DEFINED | SF_IMPORT)
#define SF_UNDEFVAL	(SF_REFERENCED)
#define SF_EXPMASK	(SF_TRAMPOLINE | SF_EXPORT)
#define SF_EXPVAL	(SF_EXPORT)
#define SF_DBGINFOMASK	(SF_TRAMPOLINE | SF_DEFINED | SF_EXPORT | SF_IMPORT)
#define SF_DBGINFOVAL 	(SF_DEFINED)

/* Definitions for the hash table */
#define MAIN_HASHTAB_SIZE    	213
#define SUB_HASHTAB_SIZE     	53
typedef struct SymTable SymTable;
struct SymTable {
    SymTable*           Left;           /* Pointer to smaller entry */
    SymTable*           Right;          /* Pointer to greater entry */
    SymTable*          	Parent;   	/* Link to enclosing scope if any */
    SymTable*           Childs;         /* Pointer to child scopes */
    unsigned            Level;          /* Lexical level */
    unsigned   	     	TableSlots;	/* Number of hash table slots */
    unsigned   	    	TableEntries;	/* Number of entries in the table */
    unsigned            Name;           /* Name of the scope */
    SymEntry*  	    	Table [1];  	/* Dynamic allocation */
};

/* Arguments for SymFind */
#define SF_FIND_EXISTING 	0
#define SF_ALLOC_NEW		1

/* Symbol table variables */
static SymEntry*    	SymLast = 0;   	/* Pointer to last defined symbol */
static SymTable*    	SymTab  = 0; 	/* Pointer to current symbol table */
static SymTable*	RootTab = 0;	/* Root symbol table */
static unsigned        	ImportCount = 0;/* Counter for import symbols */
static unsigned     	ExportCount = 0;/* Counter for export symbols */



/*****************************************************************************/
/*     	       	       	   Internally used functions		 	     */
/*****************************************************************************/



static int IsLocalName (const char* Name)
/* Return true if Name is the name of a local symbol */
{
    return (*Name == LocalStart);
}



static int IsLocalNameId (unsigned Name)
/* Return true if Name is the name of a local symbol */
{
    return (*GetString (Name) == LocalStart);
}



static unsigned SymTableSize (unsigned Level)
/* Get the size of a table for the given lexical level */
{
    switch (Level) {
        case 0:         return 213;
        case 1:         return  53;
        default:        return  29;
    }
}



static SymTable* NewSymTable (SymTable* Parent, unsigned Name)
/* Allocate a symbol table on the heap and return it */
{
    /* Determine the lexical level and the number of table slots */
    unsigned Level = Parent? Parent->Level + 1 : 0;
    unsigned Slots = SymTableSize (Level);

    /* Allocate memory */
    SymTable* S = xmalloc (sizeof (SymTable) + (Slots-1) * sizeof (SymEntry*));

    /* Set variables and clear hash table entries */
    S->Left         = 0;
    S->Right        = 0;
    S->Childs       = 0;
    S->Level        = Level;
    S->TableSlots   = Slots;
    S->TableEntries = 0;
    S->Parent       = Parent;
    S->Name         = Name;
    while (Slots--) {
       	S->Table[Slots] = 0;
    }

    /* Insert the symbol table into the child tree of the parent */
    if (Parent) {
        SymTable* T = Parent->Childs;
        if (T == 0) {
            /* First entry */
            Parent->Childs = S;
        } else {
            while (1) {
                /* Choose next entry */
                if (S->Name < T->Name) {
                    if (T->Left) {
                        T = T->Left;
                    } else {
                        T->Left = S;
                        break;
                    }
                } else if (S->Name > T->Name) {
                    if (T->Right) {
                        T = T->Right;
                    } else {
                        T->Right = S;
                        break;
                    }
                } else {
                    /* Duplicate scope name */
                    Internal ("Duplicate scope name: `%s'", GetString (S->Name));
                }
            }
        }
    } else {
        /* This is the root table */
        RootTab = S;
    }

    /* Return the prepared struct */
    return S;
}



static int SearchSymTree (SymEntry* T, unsigned Name, SymEntry** E)
/* Search in the given tree for a name. If we find the symbol, the function
 * will return 0 and put the entry pointer into E. If we did not find the
 * symbol, and the tree is empty, E is set to NULL. If the tree is not empty,
 * E will be set to the last entry, and the result of the function is <0 if
 * the entry should be inserted on the left side, and >0 if it should get
 * inserted on the right side.
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
        if (Name < T->Name) {
            Cmp = -1;
        } else if (Name > T->Name) {
            Cmp = +1;
        } else {
            Cmp = 0;
        }
      	if (Name < T->Name && T->Left) {
	    T = T->Left;
	} else if (Name > T->Name && T->Right) {
	    T = T->Right;
	} else {
     	    /* Found or end of search, return the result */
            *E = T;
            return Cmp;
       	}
    }
}



/*****************************************************************************/
/*     	       		   	     Code			   	     */
/*****************************************************************************/



static SymEntry* SymFind (SymTable* Tab, unsigned Name, int AllocNew)
/* Find a new symbol table entry in the given table. If AllocNew is given and
 * the entry is not found, create a new one. Return the entry found, or the
 * new entry created, or - in case AllocNew is zero - return 0.
 */
{
    SymEntry* S;
    int Cmp;
    unsigned Hash;

    if (IsLocalNameId (Name)) {

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
        Cmp = SearchSymTree (SymLast->Locals, Name, &S);

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
       	Hash = Name % Tab->TableSlots;

	/* Search for the entry */
	Cmp = SearchSymTree (Tab->Table[Hash], Name, &S);

	/* If we found an entry, return it */
	if (Cmp == 0) {
	    /* Check for a trampoline entry, in this case return the real
	     * symbol.
	     */
	    while (S->Flags & SF_TRAMPOLINE) {
	     	S = S->V.Sym;
	    }
            return S;
	}

	if (AllocNew) {

	    /* Otherwise create a new entry, insert and return it */
	    SymEntry* N = NewSymEntry (Name);
	    if (S == 0) {
	     	Tab->Table[Hash] = N;
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



static SymEntry* SymFindAny (SymTable* Tab, unsigned Name)
/* Find a symbol in any table */
{
    SymEntry* Sym;
    do {
	/* Search in the current table */
	Sym = SymFind (Tab, Name, SF_FIND_EXISTING);
	if (Sym) {
	    /* Found, return it */
	    return Sym;
	} else {
	    /* Not found, search in the parent scope, if we have one */
	    Tab = Tab->Parent;
	}
    } while (Sym == 0 && Tab != 0);

    /* Not found */
    return 0;
}



void SymEnterLevel (const char* ScopeName)
/* Enter a new lexical level */
{
    /* Accept NULL pointers for the scope name */
    if (ScopeName == 0) {
        ScopeName = "";
    }

    /* Create the new table */
    SymTab = NewSymTable (SymTab, GetStringId (ScopeName));
}



void SymLeaveLevel (void)
/* Leave the current lexical level */
{
    SymTab = SymTab->Parent;
}



int SymIsLocalLevel (void)
/* Return true if we are on a local symbol table level. */
{
    return (SymTab != RootTab);
}



void SymDef (const char* Name, ExprNode* Expr, unsigned Flags)
/* Define a new symbol */
{
    /* Do we have such a symbol? */
    SymEntry* S = SymFind (SymTab, GetStringId (Name), SF_ALLOC_NEW);
    if (S->Flags & SF_IMPORT) {
       	/* Defined symbol is marked as imported external symbol */
       	Error (ERR_SYM_ALREADY_IMPORT, Name);
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
    if (Flags & SYM_ZP) {
	S->Flags |= SF_ZP;
    }
    if (Flags & SYM_LABEL) {
	S->Flags |= SF_LABEL;
    }

    /* If the symbol is a ZP symbol, check if the value is in correct range */
    if (S->Flags & SF_ZP) {
     	/* Already marked as ZP symbol by some means */
     	if (!IsByteExpr (Expr)) {
     	    Error (ERR_RANGE);
     	}
    }

    /* If this is not a local symbol, remember it as the last global one */
    if (!IsLocalName (Name)) {
       	SymLast = S;
    }
}



SymEntry* SymRef (const char* Name, int Scope)
/* Search for the symbol and return it */
{
    SymEntry* S;
    unsigned NameId = GetStringId (Name);

    switch (Scope) {
        case SCOPE_GLOBAL:  S = SymFind (RootTab, NameId, SF_ALLOC_NEW);  break;
        case SCOPE_LOCAL:   S = SymFind (SymTab, NameId, SF_ALLOC_NEW);   break;

        /* Others are not allowed */
        case SCOPE_ANY:
        default:
            Internal ("Invalid scope in SymRef: %d", Scope);
            /* NOTREACHED */
            S = 0;
    }

    /* Mark the symbol as referenced */
    S->Flags |= SF_REFERENCED;

    /* Return it */
    return S;
}



static void SymImportInternal (const char* Name, unsigned Flags)
/* Mark the given symbol as an imported symbol */
{
    SymEntry* S;

    /* Don't accept local symbols */
    if (IsLocalName (Name)) {
     	Error (ERR_ILLEGAL_LOCAL_USE);
     	return;
    }

    /* Do we have such a symbol? */
    S = SymFind (SymTab, GetStringId (Name), SF_ALLOC_NEW);
    if (S->Flags & SF_DEFINED) {
     	Error (ERR_SYM_ALREADY_DEFINED, Name);
     	S->Flags |= SF_MULTDEF;
     	return;
    }
    if (S->Flags & SF_EXPORT) {
     	/* The symbol is already marked as exported symbol */
     	Error (ERR_SYM_ALREADY_EXPORT, Name);
     	return;
    }

    /* If the symbol is marked as global, check the symbol flags, then do
     * silently remove the global flag
     */
    if (S->Flags & SF_GLOBAL) {
     	if ((Flags & (SF_ZP | SF_FORCED)) != (S->Flags & (SF_ZP | SF_FORCED))) {
     	    Error (ERR_SYM_REDECL_MISMATCH, Name);
     	}
        S->Flags &= ~SF_GLOBAL;
    }

    /* Set the symbol data */
    S->Flags |= (SF_IMPORT | Flags);
}



void SymImport (const char* Name)
/* Mark the given symbol as an imported symbol */
{
    SymImportInternal (Name, SF_NONE);
}



void SymImportZP (const char* Name)
/* Mark the given symbol as a forced imported symbol */
{
    SymImportInternal (Name, SF_ZP);
}



void SymImportForced (const char* Name)
/* Mark the given symbol as a forced imported symbol */
{
    SymImportInternal (Name, SF_FORCED);
}



static void SymExportInternal (const char* Name, unsigned Flags)
/* Mark the given symbol as an exported symbol */
{
    SymEntry* S;

    /* Don't accept local symbols */
    if (IsLocalName (Name)) {
     	Error (ERR_ILLEGAL_LOCAL_USE);
     	return;
    }

    /* Do we have such a symbol? */
    S = SymFind (SymTab, GetStringId (Name), SF_ALLOC_NEW);
    if (S->Flags & SF_IMPORT) {
     	/* The symbol is already marked as imported external symbol */
     	Error (ERR_SYM_ALREADY_IMPORT, Name);
     	return;
    }

    /* If the symbol is marked as global, check the symbol size, then do
     * silently remove the global flag
     */
    if (S->Flags & SF_GLOBAL) {
       	if ((Flags & SF_ZP) != (S->Flags & SF_ZP)) {
     	    Error (ERR_SYM_REDECL_MISMATCH, Name);
     	}
        S->Flags &= ~SF_GLOBAL;
    }

    /* Set the symbol data */
    S->Flags |= (SF_EXPORT | SF_REFERENCED | Flags);
}



void SymExport (const char* Name)
/* Mark the given symbol as an exported symbol */
{
    SymExportInternal (Name, SF_NONE);
}



void SymExportZP (const char* Name)
/* Mark the given symbol as an exported zeropage symbol */
{
    SymExportInternal (Name, SF_ZP);
}



static void SymGlobalInternal (const char* Name, unsigned Flags)
/* Mark the given symbol as a global symbol, that is, as a symbol that is
 * either imported or exported.
 */
{
    SymEntry* S;

    /* Don't accept local symbols */
    if (IsLocalName (Name)) {
     	Error (ERR_ILLEGAL_LOCAL_USE);
     	return;
    }

    /* Search for this symbol, create a new entry if needed */
    S = SymFind (SymTab, GetStringId (Name), SF_ALLOC_NEW);

    /* If the symbol is already marked as import or export, check the
     * size of the definition, then bail out. */
    if (S->Flags & SF_IMPORT || S->Flags & SF_EXPORT) {
       	if ((Flags & SF_ZP) != (S->Flags & SF_ZP)) {
     	    Error (ERR_SYM_REDECL_MISMATCH, Name);
     	}
     	return;
    }

    /* Mark the symbol */
    S->Flags |= (SF_GLOBAL | Flags);
}



void SymGlobal (const char* Name)
/* Mark the given symbol as a global symbol, that is, as a symbol that is
 * either imported or exported.
 */
{
    SymGlobalInternal (Name, SF_NONE);
}



void SymGlobalZP (const char* Name)
/* Mark the given symbol as a global zeropage symbol, that is, as a symbol
 * that is either imported or exported.
 */
{
    SymGlobalInternal (Name, SF_ZP);
}



void SymConDes (const char* Name, unsigned Type, unsigned Prio)
/* Mark the given symbol as a module constructor/destructor. This will also
 * mark the symbol as an export. Initializers may never be zero page symbols.
 */
{
    SymEntry* S;

    /* Check the parameters */
#if (CD_TYPE_MIN != 0)
    CHECK (Type >= CD_TYPE_MIN && Type <= CD_TYPE_MAX);
#else
    CHECK (Type <= CD_TYPE_MAX);
#endif
    CHECK (Prio >= CD_PRIO_MIN && Prio <= CD_PRIO_MAX);

    /* Don't accept local symbols */
    if (IsLocalName (Name)) {
     	Error (ERR_ILLEGAL_LOCAL_USE);
     	return;
    }

    /* Do we have such a symbol? */
    S = SymFind (SymTab, GetStringId (Name), SF_ALLOC_NEW);
    if (S->Flags & SF_IMPORT) {
     	/* The symbol is already marked as imported external symbol */
     	Error (ERR_SYM_ALREADY_IMPORT, Name);
     	return;
    }

    /* If the symbol is marked as global, silently remove the global flag */
    if (S->Flags & SF_GLOBAL) {
        S->Flags &= ~SF_GLOBAL;
    }

    /* Check if the symbol was not already defined as ZP symbol */
    if ((S->Flags & SF_ZP) != 0) {
	Error (ERR_SYM_REDECL_MISMATCH, Name);
    }

    /* If the symbol was already declared as a condes, check if the new
     * priority value is the same as the old one.
     */
    if (S->ConDesPrio[Type] != CD_PRIO_NONE) {
	if (S->ConDesPrio[Type] != Prio) {
	    Error (ERR_SYM_REDECL_MISMATCH, Name);
	}
    }
    S->ConDesPrio[Type] = Prio;

    /* Set the symbol data */
    S->Flags |= SF_EXPORT | SF_REFERENCED;
}



int SymIsDef (const char* Name, int Scope)
/* Return true if the given symbol is already defined */
{
    SymEntry* S = 0;

    /* Get the string pool index for the name */
    unsigned NameId = GetStringId (Name);

    /* Search for the symbol */
    switch (Scope) {
        case SCOPE_ANY:    S = SymFindAny (SymTab, NameId);                 break;
        case SCOPE_GLOBAL: S = SymFind (RootTab, NameId, SF_FIND_EXISTING); break;
        case SCOPE_LOCAL:  S = SymFind (SymTab, NameId, SF_FIND_EXISTING);  break;
        default:           Internal ("Invalid scope in SymIsDef: %d", Scope);
    }

    /* Check if it's defined */
    return S != 0 && (S->Flags & SF_DEFINED) != 0;
}



int SymIsRef (const char* Name, int Scope)
/* Return true if the given symbol has been referenced */
{
    SymEntry* S = 0;

    /* Get the string pool index for the name */
    unsigned NameId = GetStringId (Name);

    /* Search for the symbol */
    switch (Scope) {
        case SCOPE_ANY:    S = SymFindAny (SymTab, NameId);                 break;
        case SCOPE_GLOBAL: S = SymFind (RootTab, NameId, SF_FIND_EXISTING); break;
        case SCOPE_LOCAL:  S = SymFind (SymTab, NameId, SF_FIND_EXISTING);  break;
        default:           Internal ("Invalid scope in SymIsRef: %d", Scope);
    }

    /* Check if it's defined */
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
    if (!IsLocalNameId (S->Name)                                        &&
	(S->Flags & (SF_ZP | SF_ABS | SF_DEFINED | SF_IMPORT)) == 0     &&
	S->SymTab->Parent != 0) {

	/* Try to find a symbol with the same name in the enclosing scope */
	SymEntry* E = SymFindAny (S->SymTab->Parent, S->Name);

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
    if ((S->Flags & SF_FINALIZED) == 0 && SymHasExpr (S)) {
	S->V.Expr = FinalizeExpr (S->V.Expr);
        S->Flags |= SF_FINALIZED;
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
    return GetString (S->Name);
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
	SymTable* Tab = S->SymTab->Parent;
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
	       	    Tab = Sym->SymTab->Parent;
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
	       	PError (&S->Pos, ERR_SYM_ALREADY_IMPORT, GetString (S->Name));
	    }
	    Sym->Flags |= S->Flags & (SF_EXPORT | SF_ZP);
	}

	/* Transfer the referenced flag */
	Sym->Flags |= (S->Flags & SF_REFERENCED);

    } else {
	/* The symbol is definitely undefined */
	if (S->Flags & SF_EXPORT) {
	    /* We will not auto-import an export */
	    PError (&S->Pos, ERR_EXPORT_UNDEFINED, GetString (S->Name));
	} else {
	    if (AutoImport) {
		/* Mark as import, will be indexed later */
		S->Flags |= SF_IMPORT;
	    } else {
		/* Error */
	        PError (&S->Pos, ERR_SYM_UNDEFINED, GetString (S->Name));
	    }
	}
    }
}



void SymCheck (void)
/* Run through all symbols and check for anomalies and errors */
{
    SymEntry* S;

    /* Check for open lexical levels */
    if (SymTab->Parent != 0) {
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
		PWarning (&S->Pos, WARN_SYM_NOT_REFERENCED, GetString (S->Name));
	    }
	    if (S->Flags & SF_IMPORT) {
		if ((S->Flags & (SF_REFERENCED | SF_FORCED)) == SF_NONE) {
		    /* Imported symbol is not referenced */
		    PWarning (&S->Pos, WARN_IMPORT_NOT_REFERENCED, GetString (S->Name));
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
	    fprintf (F,
		     "%-24s %s %s %s %s %s\n",
		     GetString (S->Name),
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

    /* Walk throught list and write all valid imports to the file. An import
     * is considered valid, if it is either referenced, or the forced bit is
     * set. Otherwise, the import is ignored (no need to link in something
     * that isn't used).
     */
    S = SymList;
    while (S) {
        if ((S->Flags & (SF_TRAMPOLINE | SF_IMPORT)) == SF_IMPORT &&
            (S->Flags & (SF_REFERENCED | SF_FORCED)) != 0) {

     	    if (S->Flags & SF_ZP) {
     		ObjWrite8 (IMP_ZP);
     	    } else {
     		ObjWrite8 (IMP_ABS);
     	    }
       	    ObjWriteVar (S->Name);
     	    ObjWritePos (&S->Pos);
     	}
     	S = S->List;
    }

    /* Done writing imports */
    ObjEndImports ();
}



static unsigned char GetExprMask (SymEntry* S)
/* Return the expression bits for the given symbol table entry */
{
    unsigned char ExprMask;

    /* Check if the symbol is const */
    ExprMask = (SymIsConst (S))? EXP_CONST : EXP_EXPR;

    /* Add zeropage/abs bits */
    ExprMask |= (S->Flags & SF_ZP)? EXP_ZP : EXP_ABS;

    /* Add the label/equate bits */
    ExprMask |= (S->Flags & SF_LABEL)? EXP_LABEL : EXP_EQUATE;

    /* Return the mask */
    return ExprMask;
}



void WriteExports (void)
/* Write the exports list to the object file */
{
    SymEntry* S;
    unsigned Type;

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

	    /* Get the expression bits */
	    ExprMask = GetExprMask (S);

	    /* Count the number of ConDes types */
	    for (Type = 0; Type < CD_TYPE_COUNT; ++Type) {
	    	if (S->ConDesPrio[Type] != CD_PRIO_NONE) {
	    	    INC_EXP_CONDES_COUNT (ExprMask);
	    	}
	    }

	    /* Write the type */
	    ObjWrite8 (ExprMask);

	    /* Write any ConDes declarations */
	    if (GET_EXP_CONDES_COUNT (ExprMask) > 0) {
		for (Type = 0; Type < CD_TYPE_COUNT; ++Type) {
		    unsigned char Prio = S->ConDesPrio[Type];
		    if (Prio != CD_PRIO_NONE) {
		    	ObjWrite8 (CD_BUILD (Type, Prio));
		    }
		}
	    }

	    /* Write the name */
       	    ObjWriteVar (S->Name);

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

		/* Get the expression bits */
       	       	ExprMask = GetExprMask (S);

		/* Write the type */
		ObjWrite8 (ExprMask);

		/* Write the name */
       	       	ObjWriteVar (S->Name);

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




