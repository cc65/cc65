/*****************************************************************************/
/*                                                                           */
/*                                symentry.c                                 */
/*                                                                           */
/*              Symbol table entry for the ca65 macroassembler               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#include <string.h>

/* common */
#include "addrsize.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "global.h"
#include "scanner.h"
#include "segment.h"
#include "spool.h"
#include "studyexpr.h"          /* ### */
#include "symentry.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* List of all symbol table entries */
SymEntry* SymList = 0;

/* Pointer to last defined symbol */
SymEntry* SymLast = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SymEntry* NewSymEntry (const StrBuf* Name, unsigned Flags)
/* Allocate a symbol table entry, initialize and return it */
{
    unsigned I;

    /* Allocate memory */
    SymEntry* S = xmalloc (sizeof (SymEntry));

    /* Initialize the entry */
    S->Left       = 0;
    S->Right      = 0;
    S->Locals     = 0;
    S->Sym.Tab    = 0;
    S->DefLines   = EmptyCollection;
    S->RefLines   = EmptyCollection;
    for (I = 0; I < sizeof (S->GuessedUse) / sizeof (S->GuessedUse[0]); ++I) {
        S->GuessedUse[I] = 0;
    }
    S->HLLSym     = 0;
    S->Flags      = Flags;
    S->DebugSymId = ~0U;
    S->ImportId   = ~0U;
    S->ExportId   = ~0U;
    S->Expr       = 0;
    S->ExprRefs   = AUTO_COLLECTION_INITIALIZER;
    S->ExportSize = ADDR_SIZE_DEFAULT;
    S->AddrSize   = ADDR_SIZE_DEFAULT;
    memset (S->ConDesPrio, 0, sizeof (S->ConDesPrio));
    S->Name       = GetStrBufId (Name);

    /* Insert it into the list of all entries */
    S->List = SymList;
    SymList = S;

    /* Return the initialized entry */
    return S;
}



int SymSearchTree (SymEntry* T, const StrBuf* Name, SymEntry** E)
/* Search in the given tree for a name. If we find the symbol, the function
** will return 0 and put the entry pointer into E. If we did not find the
** symbol, and the tree is empty, E is set to NULL. If the tree is not empty,
** E will be set to the last entry, and the result of the function is <0 if
** the entry should be inserted on the left side, and >0 if it should get
** inserted on the right side.
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
        const StrBuf* SymName = GetStrBuf (T->Name);

        /* Choose next entry */
        int Cmp = SB_Compare (Name, SymName);
        if (Cmp < 0 && T->Left) {
            T = T->Left;
        } else if (Cmp > 0 && T->Right) {
            T = T->Right;
        } else {
            /* Found or end of search, return the result */
            *E = T;
            return Cmp;
        }
    }
}



void SymTransferExprRefs (SymEntry* From, SymEntry* To)
/* Transfer all expression references from one symbol to another. */
{
    unsigned I;

    for (I = 0; I < CollCount (&From->ExprRefs); ++I) {

        /* Get the expression node */
        ExprNode* E = CollAtUnchecked (&From->ExprRefs, I);

        /* Safety */
        CHECK (E->Op == EXPR_SYMBOL && E->V.Sym == From);

        /* Replace the symbol reference */
        E->V.Sym = To;

        /* Add the expression reference */
        SymAddExprRef (To, E);
    }

    /* Remove all symbol references from the old symbol */
    CollDeleteAll (&From->ExprRefs);
}



static void SymReplaceExprRefs (SymEntry* S)
/* Replace the references to this symbol by a copy of the symbol expression */
{
    unsigned I;
    long     Val;

    /* Check if the expression is const and get its value */
    int IsConst = IsConstExpr (S->Expr, &Val);
    CHECK (IsConst);

    /* Loop over all references */
    for (I = 0; I < CollCount (&S->ExprRefs); ++I) {

        /* Get the expression node */
        ExprNode* E = CollAtUnchecked (&S->ExprRefs, I);

        /* Safety */
        CHECK (E->Op == EXPR_SYMBOL && E->V.Sym == S);

        /* We cannot touch the root node, since there are pointers to it.
        ** Replace it by a literal node.
        */
        E->Op = EXPR_LITERAL;
        E->V.IVal = Val;
    }

    /* Remove all symbol references from the symbol */
    CollDeleteAll (&S->ExprRefs);
}



void SymDef (SymEntry* S, ExprNode* Expr, unsigned char AddrSize, unsigned Flags)
/* Define a new symbol */
{
    if (S->Flags & SF_IMPORT) {
        /* Defined symbol is marked as imported external symbol */
        Error ("Symbol `%m%p' is already an import", GetSymName (S));
        return;
    }
    if ((Flags & SF_VAR) != 0 && (S->Flags & (SF_EXPORT | SF_GLOBAL))) {
        /* Variable symbols cannot be exports or globals */
        Error ("Var symbol `%m%p' cannot be an export or global symbol", GetSymName (S));
        return;
    }
    if (S->Flags & SF_DEFINED) {
        /* Multiple definition. In case of a variable, this is legal. */
        if ((S->Flags & SF_VAR) == 0) {
            Error ("Symbol `%m%p' is already defined", GetSymName (S));
            S->Flags |= SF_MULTDEF;
            return;
        } else {
            /* Redefinition must also be a variable symbol */
            if ((Flags & SF_VAR) == 0) {
                Error ("Symbol `%m%p' is already different kind", GetSymName (S));
                return;
            }
            /* Delete the current symbol expression, since it will get
            ** replaced
            */
            FreeExpr (S->Expr);
            S->Expr = 0;
        }
    }

    /* Map a default address size to a real value */
    if (AddrSize == ADDR_SIZE_DEFAULT) {
        /* ### Must go! Delay address size calculation until end of assembly! */
        ExprDesc ED;
        ED_Init (&ED);
        StudyExpr (Expr, &ED);
        AddrSize = ED.AddrSize;
        ED_Done (&ED);
    }

    /* Set the symbol value */
    S->Expr = Expr;

    /* In case of a variable symbol, walk over all expressions containing
    ** this symbol and replace the (sub-)expression by the literal value of
    ** the tree. Be sure to replace the expression node in place, since there
    ** may be pointers to it.
    */
    if (Flags & SF_VAR) {
        SymReplaceExprRefs (S);
    }

    /* If the symbol is marked as global, export it. Address size is checked
    ** below.
    */
    if (S->Flags & SF_GLOBAL) {
        S->Flags = (S->Flags & ~SF_GLOBAL) | SF_EXPORT;
        ReleaseFullLineInfo (&S->DefLines);
    }

    /* Mark the symbol as defined and use the given address size */
    S->Flags |= (SF_DEFINED | Flags);
    S->AddrSize = AddrSize;

    /* Remember the line info of the symbol definition */
    GetFullLineInfo (&S->DefLines);

    /* If the symbol is exported, check the address sizes */
    if (S->Flags & SF_EXPORT) {
        if (S->ExportSize == ADDR_SIZE_DEFAULT) {
            /* Use the real size of the symbol */
            S->ExportSize = S->AddrSize;
        } else if (S->AddrSize > S->ExportSize) {
            /* We're exporting a symbol smaller than it actually is */
            Warning (1, "Symbol `%m%p' is %s but exported %s",
                     GetSymName (S), AddrSizeToStr (S->AddrSize),
                     AddrSizeToStr (S->ExportSize));
        }
    }

    /* If this is not a local symbol, remember it as the last global one */
    if ((S->Flags & SF_LOCAL) == 0) {
        SymLast = S;
    }
}



void SymRef (SymEntry* S)
/* Mark the given symbol as referenced */
{
    /* Mark the symbol as referenced */
    S->Flags |= SF_REFERENCED;

    /* Remember the current location */
    CollAppend (&S->RefLines, GetAsmLineInfo ());
}



void SymImport (SymEntry* S, unsigned char AddrSize, unsigned Flags)
/* Mark the given symbol as an imported symbol */
{
    if (S->Flags & SF_DEFINED) {
        Error ("Symbol `%m%p' is already defined", GetSymName (S));
        S->Flags |= SF_MULTDEF;
        return;
    }
    if (S->Flags & SF_EXPORT) {
        /* The symbol is already marked as exported symbol */
        Error ("Cannot import exported symbol `%m%p'", GetSymName (S));
        return;
    }

    /* If no address size is given, use the address size of the enclosing
    ** segment.
    */
    if (AddrSize == ADDR_SIZE_DEFAULT) {
        AddrSize = GetCurrentSegAddrSize ();
    }

    /* If the symbol is marked as import or global, check the address size,
    ** then do silently remove the global flag.
    */
    if (S->Flags & SF_IMPORT) {
        if ((Flags & SF_FORCED) != (S->Flags & SF_FORCED)) {
            Error ("Redeclaration mismatch for symbol `%m%p'", GetSymName (S));
        }
        if (AddrSize != S->AddrSize) {
            Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
        }
    }
    if (S->Flags & SF_GLOBAL) {
        S->Flags &= ~SF_GLOBAL;
        if (AddrSize != S->AddrSize) {
            Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
        }
    }

    /* Set the symbol data */
    S->Flags |= (SF_IMPORT | Flags);
    S->AddrSize = AddrSize;

    /* Mark the position of the import as the position of the definition.
    ** Please note: In case of multiple .global or .import statements, the line
    ** infos add up.
    */
    GetFullLineInfo (&S->DefLines);
}



void SymExport (SymEntry* S, unsigned char AddrSize, unsigned Flags)
/* Mark the given symbol as an exported symbol */
{
    /* Check if it's ok to export the symbol */
    if (S->Flags & SF_IMPORT) {
        /* The symbol is already marked as imported external symbol */
        Error ("Symbol `%m%p' is already an import", GetSymName (S));
        return;
    }
    if (S->Flags & SF_VAR) {
        /* Variable symbols cannot be exported */
        Error ("Var symbol `%m%p' cannot be exported", GetSymName (S));
        return;
    }

    /* If the symbol was marked as global before, remove the global flag and
    ** proceed, but check the address size.
    */
    if (S->Flags & SF_GLOBAL) {
        if (AddrSize != S->ExportSize) {
            Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
        }
        S->Flags &= ~SF_GLOBAL;

        /* .GLOBAL remembers line infos in case an .IMPORT follows. We have
        ** to remove these here.
        */
        ReleaseFullLineInfo (&S->DefLines);
    }

    /* If the symbol was already marked as an export, but wasn't defined
    ** before, the address sizes in both definitions must match.
    */
    if ((S->Flags & (SF_EXPORT|SF_DEFINED)) == SF_EXPORT) {
        if (S->ExportSize != AddrSize) {
            Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
        }
    }
    S->ExportSize = AddrSize;

    /* If the symbol is already defined, check symbol size against the
    ** exported size.
    */
    if (S->Flags & SF_DEFINED) {
        if (S->ExportSize == ADDR_SIZE_DEFAULT) {
            /* No export size given, use the real size of the symbol */
            S->ExportSize = S->AddrSize;
        } else if (S->AddrSize > S->ExportSize) {
            /* We're exporting a symbol smaller than it actually is */
            Warning (1, "Symbol `%m%p' is %s but exported %s",
                     GetSymName (S), AddrSizeToStr (S->AddrSize),
                     AddrSizeToStr (S->ExportSize));
        }
    }

    /* Set the symbol data */
    S->Flags |= (SF_EXPORT | SF_REFERENCED | Flags);

    /* Remember line info for this reference */
    CollAppend (&S->RefLines, GetAsmLineInfo ());
}



void SymGlobal (SymEntry* S, unsigned char AddrSize, unsigned Flags)
/* Mark the given symbol as a global symbol, that is, as a symbol that is
** either imported or exported.
*/
{
    if (S->Flags & SF_VAR) {
        /* Variable symbols cannot be exported or imported */
        Error ("Var symbol `%m%p' cannot be made global", GetSymName (S));
        return;
    }

    /* If the symbol is already marked as import, the address size must match.
    ** Apart from that, ignore the global declaration.
    */
    if (S->Flags & SF_IMPORT) {
        if (AddrSize == ADDR_SIZE_DEFAULT) {
            /* Use the size of the current segment */
            AddrSize = GetCurrentSegAddrSize ();
        }
        if (AddrSize != S->AddrSize) {
            Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
        }
        return;
    }

    /* If the symbol is already an export: If it is not defined, the address
    ** sizes must match.
    */
    if (S->Flags & SF_EXPORT) {
        if ((S->Flags & SF_DEFINED) == 0) {
            /* Symbol is undefined */
            if (AddrSize != S->ExportSize) {
                Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
            }
        } else if (AddrSize != ADDR_SIZE_DEFAULT) {
            /* Symbol is defined and address size given */
            if (AddrSize != S->ExportSize) {
                Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
            }
        }
        return;
    }

    /* If the symbol is already marked as global, the address size must match.
    ** Use the ExportSize here, since it contains the actual address size
    ** passed to this function.
    */
    if (S->Flags & SF_GLOBAL) {
        if (AddrSize != S->ExportSize) {
            Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
        }
        return;
    }

    /* If we come here, the symbol was neither declared as export, import or
    ** global before. Check if it is already defined, in which case it will
    ** become an export. If it is not defined, mark it as global and remember
    ** the given address sizes.
    */
    if (S->Flags & SF_DEFINED) {
        /* The symbol is defined, export it */
        S->ExportSize = AddrSize;
        if (S->ExportSize == ADDR_SIZE_DEFAULT) {
            /* No export size given, use the real size of the symbol */
            S->ExportSize = S->AddrSize;
        } else if (S->AddrSize > S->ExportSize) {
            /* We're exporting a symbol smaller than it actually is */
            Warning (1, "Symbol `%m%p' is %s but exported %s",
                     GetSymName (S), AddrSizeToStr (S->AddrSize),
                     AddrSizeToStr (S->ExportSize));
        }
        S->Flags |= (SF_EXPORT | Flags);
    } else {
        /* Since we don't know if the symbol will get exported or imported,
        ** remember two different address sizes: One for an import in AddrSize,
        ** and the other one for an export in ExportSize.
        */
        S->AddrSize = AddrSize;
        if (S->AddrSize == ADDR_SIZE_DEFAULT) {
            /* Use the size of the current segment */
            S->AddrSize = GetCurrentSegAddrSize ();
        }
        S->ExportSize = AddrSize;
        S->Flags |= (SF_GLOBAL | Flags);

        /* Remember the current location as location of definition in case
        ** an .IMPORT follows later.
        */
        GetFullLineInfo (&S->DefLines);
    }
}



void SymConDes (SymEntry* S, unsigned char AddrSize, unsigned Type, unsigned Prio)
/* Mark the given symbol as a module constructor/destructor. This will also
** mark the symbol as an export. Initializers may never be zero page symbols.
*/
{
    /* Check the parameters */
#if (CD_TYPE_MIN != 0)
    CHECK (Type >= CD_TYPE_MIN && Type <= CD_TYPE_MAX);
#else
    CHECK (Type <= CD_TYPE_MAX);
#endif
    CHECK (Prio >= CD_PRIO_MIN && Prio <= CD_PRIO_MAX);

    /* Check for errors */
    if (S->Flags & SF_IMPORT) {
        /* The symbol is already marked as imported external symbol */
        Error ("Symbol `%m%p' is already an import", GetSymName (S));
        return;
    }
    if (S->Flags & SF_VAR) {
        /* Variable symbols cannot be exported or imported */
        Error ("Var symbol `%m%p' cannot be exported", GetSymName (S));
        return;
    }

    /* If the symbol was already marked as an export or global, check if
    ** this was done specifiying the same address size. In case of a global
    ** declaration, silently remove the global flag.
    */
    if (S->Flags & (SF_EXPORT | SF_GLOBAL)) {
        if (S->ExportSize != AddrSize) {
            Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
        }
        S->Flags &= ~SF_GLOBAL;
    }
    S->ExportSize = AddrSize;

    /* If the symbol is already defined, check symbol size against the
    ** exported size.
    */
    if (S->Flags & SF_DEFINED) {
        if (S->ExportSize == ADDR_SIZE_DEFAULT) {
            /* Use the real size of the symbol */
            S->ExportSize = S->AddrSize;
        } else if (S->AddrSize != S->ExportSize) {
            Error ("Address size mismatch for symbol `%m%p'", GetSymName (S));
        }
    }

    /* If the symbol already was declared as a condes of this type,
    ** check if the new priority value is the same as the old one.
    */
    if (S->ConDesPrio[Type] != CD_PRIO_NONE) {
        if (S->ConDesPrio[Type] != Prio) {
            Error ("Redeclaration mismatch for symbol `%m%p'", GetSymName (S));
        }
    }
    S->ConDesPrio[Type] = Prio;

    /* Set the symbol data */
    S->Flags |= (SF_EXPORT | SF_REFERENCED);

    /* Remember the line info for this reference */
    CollAppend (&S->RefLines, GetAsmLineInfo ());
}



void SymGuessedAddrSize (SymEntry* Sym, unsigned char AddrSize)
/* Mark the address size of the given symbol as guessed. The address size
** passed as argument is the one NOT used, because the actual address size
** wasn't known. Example: Zero page addressing was not used because symbol
** is undefined, and absolute addressing was available.
*/
{
    /* We must have a valid address size passed */
    PRECONDITION (AddrSize != ADDR_SIZE_DEFAULT);

    /* We do not support all address sizes currently */
    if (AddrSize > sizeof (Sym->GuessedUse) / sizeof (Sym->GuessedUse[0])) {
        return;
    }

    /* We can only remember one such occurance */
    if (Sym->GuessedUse[AddrSize-1]) {
        return;
    }

    /* Ok, remember the file position */
    Sym->GuessedUse[AddrSize-1] = xdup (&CurTok.Pos, sizeof (CurTok.Pos));
}



void SymExportFromGlobal (SymEntry* S)
/* Called at the end of assembly. Converts a global symbol that is defined
** into an export.
*/
{
    /* Remove the global flag and make the symbol an export */
    S->Flags &= ~SF_GLOBAL;
    S->Flags |= SF_EXPORT;
}



void SymImportFromGlobal (SymEntry* S)
/* Called at the end of assembly. Converts a global symbol that is undefined
** into an import.
*/
{
    /* Remove the global flag and make it an import */
    S->Flags &= ~SF_GLOBAL;
    S->Flags |= SF_IMPORT;
}



int SymIsConst (const SymEntry* S, long* Val)
/* Return true if the given symbol has a constant value. If Val is not NULL
** and the symbol has a constant value, store it's value there.
*/
{
    /* Check for constness */
    return (SymHasExpr (S) && IsConstExpr (S->Expr, Val));
}



SymTable* GetSymParentScope (SymEntry* S)
/* Get the parent scope of the symbol (not the one it is defined in). Return
** NULL if the symbol is a cheap local, or defined on global level.
*/
{
    if ((S->Flags & SF_LOCAL) != 0) {
        /* This is a cheap local symbol */
        return 0;
    } else if (S->Sym.Tab == 0) {
        /* Symbol not in a table. This may happen if there have been errors
        ** before. Return NULL in this case to avoid further errors.
        */
        return 0;
    } else {
        /* This is a global symbol */
        return S->Sym.Tab->Parent;
    }
}



struct ExprNode* GetSymExpr (SymEntry* S)
/* Get the expression for a non-const symbol */
{
    PRECONDITION (S != 0 && SymHasExpr (S));
    return S->Expr;
}



const struct ExprNode* SymResolve (const SymEntry* S)
/* Helper function for DumpExpr. Resolves a symbol into an expression or return
** NULL. Do not call in other contexts!
*/
{
    return SymHasExpr (S)? S->Expr : 0;
}



long GetSymVal (SymEntry* S)
/* Return the value of a symbol assuming it's constant. FAIL will be called
** in case the symbol is undefined or not constant.
*/
{
    long Val;
    CHECK (S != 0 && SymHasExpr (S) && IsConstExpr (GetSymExpr (S), &Val));
    return Val;
}



unsigned GetSymImportId (const SymEntry* S)
/* Return the import id for the given symbol */
{
    PRECONDITION (S != 0 && (S->Flags & SF_IMPORT) && S->ImportId != ~0U);
    return S->ImportId;
}



unsigned GetSymExportId (const SymEntry* S)
/* Return the export id for the given symbol */
{
    PRECONDITION (S != 0 && (S->Flags & SF_EXPORT) && S->ExportId != ~0U);
    return S->ExportId;
}



unsigned GetSymInfoFlags (const SymEntry* S, long* ConstVal)
/* Return a set of flags used when writing symbol information into a file.
** If the SYM_CONST bit is set, ConstVal will contain the constant value
** of the symbol. The result does not include the condes count.
** See common/symdefs.h for more information.
*/
{
    /* Setup info flags */
    unsigned Flags = 0;
    Flags |= SymIsConst (S, ConstVal)? SYM_CONST : SYM_EXPR;
    Flags |= (S->Flags & SF_LABEL)? SYM_LABEL : SYM_EQUATE;
    Flags |= (S->Flags & SF_LOCAL)? SYM_CHEAP_LOCAL : SYM_STD;
    if (S->Flags & SF_EXPORT) {
        Flags |= SYM_EXPORT;
    }
    if (S->Flags & SF_IMPORT) {
        Flags |= SYM_IMPORT;
    }

    /* Return the result */
    return Flags;
}
