/*****************************************************************************/
/*                                                                           */
/*				   symtab.c				     */
/*                                                                           */
/*		   Symbol table for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#include <string.h>

/* common */
#include "addrsize.h"
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
#include "segment.h"
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

/* Symbol tables */
SymTable*      	CurrentScope = 0;       /* Pointer to current symbol table */
SymTable*	RootScope    = 0;       /* Root symbol table */

/* Symbol table variables */
static unsigned        	ImportCount = 0;/* Counter for import symbols */
static unsigned     	ExportCount = 0;/* Counter for export symbols */



/*****************************************************************************/
/*     	       	       	   Internally used functions		 	     */
/*****************************************************************************/



static unsigned ScopeTableSize (unsigned Level)
/* Get the size of a table for the given lexical level */
{
    switch (Level) {
        case 0:         return 213;
        case 1:         return  53;
        default:        return  29;
    }
}



static SymTable* NewSymTable (SymTable* Parent, const char* Name)
/* Allocate a symbol table on the heap and return it */
{
    /* Determine the lexical level and the number of table slots */
    unsigned Level = Parent? Parent->Level + 1 : 0;
    unsigned Slots = ScopeTableSize (Level);

    /* Allocate memory */
    SymTable* S = xmalloc (sizeof (SymTable) + (Slots-1) * sizeof (SymEntry*));

    /* Set variables and clear hash table entries */
    S->Left         = 0;
    S->Right        = 0;
    S->Childs       = 0;
    S->Flags        = ST_NONE;
    S->AddrSize     = ADDR_SIZE_DEFAULT;
    S->Type         = ST_UNDEF;
    S->Level        = Level;
    S->TableSlots   = Slots;
    S->TableEntries = 0;
    S->Parent       = Parent;
    S->Name         = GetStringId (Name);
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
                int Cmp = strcmp (Name, GetString (T->Name));
                if (Cmp < 0) {
                    if (T->Left) {
                        T = T->Left;
                    } else {
                        T->Left = S;
                        break;
                    }
                } else if (Cmp > 0) {
                    if (T->Right) {
                        T = T->Right;
                    } else {
                        T->Right = S;
                        break;
                    }
                } else {
                    /* Duplicate scope name */
                    Internal ("Duplicate scope name: `%s'", Name);
                }
            }
        }
    }

    /* Return the prepared struct */
    return S;
}



static int SearchSymTree (SymEntry* T, const char* Name, SymEntry** E)
/* Search in the given tree for a name. If we find the symbol, the function
 * will return 0 and put the entry pointer into E. If we did not find the
 * symbol, and the tree is empty, E is set to NULL. If the tree is not empty,
 * E will be set to the last entry, and the result of the function is <0 if
 * the entry should be inserted on the left side, and >0 if it should get
 * inserted on the right side.
 */
{
    /* Is there a tree? */
    if (T == 0) {
      	*E = 0;
      	return 1;
    }

    /* We have a table, search it */
    while (1) {

        /* Get the symbol name */
        const char* SymName = GetString (T->Name);

      	/* Choose next entry */
        int Cmp = strcmp (Name, SymName);
       	if (Cmp < 0 && T->Left) {
	    T = T->Left;
	} else if (Cmp > 0&& T->Right) {
	    T = T->Right;
	} else {
     	    /* Found or end of search, return the result */
            *E = T;
            return Cmp;
       	}
    }
}



/*****************************************************************************/
/*     	       	   	   	     Code			   	     */
/*****************************************************************************/



void SymEnterLevel (const char* ScopeName, unsigned char Type, unsigned char AddrSize)
/* Enter a new lexical level */
{
    /* Map a default address size to something real */
    if (AddrSize == ADDR_SIZE_DEFAULT) {
        /* Use the segment address size */
        AddrSize = GetCurrentSegAddrSize ();
    }

    /* If we have a current scope, search for the given name and create a
     * new one if it doesn't exist. If this is the root scope, just create it.
     */
    if (CurrentScope) {

        /* Search for the scope, create a new one */
        CurrentScope = SymFindScope (CurrentScope, ScopeName, SYM_ALLOC_NEW);

        /* Check if the scope has been defined before */
        if (CurrentScope->Flags & ST_DEFINED) {
            Error ("Duplicate scope `%s'", ScopeName);
        }

    } else {
        CurrentScope = RootScope = NewSymTable (0, ScopeName);
    }

    /* Mark the scope as defined and set type and address size */
    CurrentScope->Flags    |= ST_DEFINED;
    CurrentScope->AddrSize = AddrSize;
    CurrentScope->Type     = Type;
}



void SymLeaveLevel (void)
/* Leave the current lexical level */
{
    CurrentScope = CurrentScope->Parent;
}



SymTable* SymFindScope (SymTable* Parent, const char* Name, int AllocNew)
/* Find a scope in the given enclosing scope */
{
    SymTable** T = &Parent->Childs;
    while (*T) {
        int Cmp = strcmp (Name, GetString ((*T)->Name));
        if (Cmp < 0) {
            T = &(*T)->Left;
        } else if (Cmp > 0) {
            T = &(*T)->Right;
        } else {
            /* Found the scope */
            return *T;
        }
    }

    /* Create a new scope if requested and we didn't find one */
    if (*T == 0 && AllocNew) {
        *T = NewSymTable (Parent, Name);
    }

    /* Return the scope */
    return *T;
}



SymEntry* SymFind (SymTable* Scope, const char* Name, int AllocNew)
/* Find a new symbol table entry in the given table. If AllocNew is given and
 * the entry is not found, create a new one. Return the entry found, or the
 * new entry created, or - in case AllocNew is zero - return 0.
 */
{
    SymEntry* S;
    int Cmp;

    if (IsLocalName (Name)) {

    	/* Local symbol, get the table */
    	if (!SymLast) {
    	    /* No last global, so there's no local table */
    	    Error ("No preceeding global symbol");
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
       	unsigned Hash = HashStr (Name) % Scope->TableSlots;

	/* Search for the entry */
	Cmp = SearchSymTree (Scope->Table[Hash], Name, &S);

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
	     	Scope->Table[Hash] = N;
	    } else if (Cmp < 0) {
	     	S->Left = N;
	    } else {
	     	S->Right = N;
	    }
       	    N->SymTab = Scope;
	    ++Scope->TableEntries;
	    return N;

	}
    }

    /* We did not find the entry and AllocNew is false. */
    return 0;
}



static SymEntry* SymFindAny (SymTable* Scope, const char* Name)
/* Find a symbol in the given or any of its parent scopes. The function will
 * never create a new symbol, since this can only be done in one specific
 * scope.
 */
{
    SymEntry* Sym;
    do {
	/* Search in the current table */
	Sym = SymFind (Scope, Name, SYM_FIND_EXISTING);
       	if (Sym) {
	    /* Found, return it */
	    return Sym;
	} else {
	    /* Not found, search in the parent scope, if we have one */
	    Scope = Scope->Parent;
	}
    } while (Sym == 0 && Scope != 0);

    /* Not found */
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
    if (!IsLocalNameId (S->Name) && (S->Flags & (SF_DEFINED | SF_IMPORT)) == 0 &&
	S->SymTab->Parent != 0) {

	/* Try to find a symbol with the same name in the enclosing scope */
	SymEntry* E = SymFindAny (S->SymTab->Parent, GetString (S->Name));

	/* If we found one, use the ZP flag */
       	if (E && E->AddrSize == ADDR_SIZE_ZP) {
            S->AddrSize = ADDR_SIZE_ZP;
	}
    }

    /* Check the ZP flag */
    return (S->AddrSize == ADDR_SIZE_ZP);
}



unsigned char GetCurrentSymTabType ()
/* Return the type of the current symbol table */
{
    CHECK (CurrentScope != 0);
    return CurrentScope->Type;
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
	    Sym = SymFindAny (Tab, GetString (S->Name));
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
	       	PError (&S->Pos, "Symbol `%s' is already an import", GetString (S->Name));
	    }
	    Sym->Flags |= (S->Flags & SF_EXPORT);
            Sym->ExportSize = S->ExportSize;
	}

	/* Transfer the referenced flag */
	Sym->Flags |= (S->Flags & SF_REFERENCED);

    } else {
	/* The symbol is definitely undefined */
	if (S->Flags & SF_EXPORT) {
	    /* We will not auto-import an export */
	    PError (&S->Pos, "Exported symbol `%s' was never defined",
                    GetString (S->Name));
	} else {
	    if (AutoImport) {
		/* Mark as import, will be indexed later */
		S->Flags |= SF_IMPORT;
	    } else {
		/* Error */
	        PError (&S->Pos, "Symbol `%s' is undefined", GetString (S->Name));
	    }
	}
    }
}



void SymCheck (void)
/* Run through all symbols and check for anomalies and errors */
{
    SymEntry* S;

    /* Check for open scopes */
    if (CurrentScope->Parent != 0) {
	Error ("Local scope was not closed");
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
		PWarning (&S->Pos, 2,
                          "Symbol `%s' is defined but never used",
                          GetString (S->Name));
	    }
	    if (S->Flags & SF_IMPORT) {
		if ((S->Flags & (SF_REFERENCED | SF_FORCED)) == SF_NONE) {
		    /* Imported symbol is not referenced */
		    PWarning (&S->Pos, 2,
                              "Symbol `%s' is imported but never used",
                              GetString (S->Name));
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
                     AddrSizeToStr (S->AddrSize));
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

     	    if (S->AddrSize == ADDR_SIZE_ZP) {
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

            long ConstVal;

	    /* Get the expression bits */
            unsigned char ExprMask = SymIsConst (S, &ConstVal)? EXP_CONST : EXP_EXPR;
            ExprMask |= (S->ExportSize == ADDR_SIZE_ZP)? EXP_ZP : EXP_ABS;
            ExprMask |= (S->Flags & SF_LABEL)? EXP_LABEL : EXP_EQUATE;

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
	     	ObjWrite32 (ConstVal);
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

                long ConstVal;

		/* Get the expression bits */
                unsigned char ExprMask = (SymIsConst (S, &ConstVal))? EXP_CONST : EXP_EXPR;
                ExprMask |= (S->AddrSize == ADDR_SIZE_ZP)? EXP_ZP : EXP_ABS;
                ExprMask |= (S->Flags & SF_LABEL)? EXP_LABEL : EXP_EQUATE;

		/* Write the type */
		ObjWrite8 (ExprMask);

		/* Write the name */
       	       	ObjWriteVar (S->Name);

		/* Write the value */
		if ((ExprMask & EXP_MASK_VAL) == EXP_CONST) {
		    /* Constant value */
		    ObjWrite32 (ConstVal);
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





