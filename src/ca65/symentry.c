/*****************************************************************************/
/*                                                                           */
/*				  symentry.c				     */
/*                                                                           */
/*	    Symbol table entry forward for the ca65 macroassembler	     */
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
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "global.h"
#include "scanner.h"
#include "spool.h"
#include "symentry.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* List of all symbol table entries */
SymEntry* SymList = 0;

/* Pointer to last defined symbol */
SymEntry* SymLast = 0;



/*****************************************************************************/
/*     	       		  	     Code			   	     */
/*****************************************************************************/



int IsLocalName (const char* Name)
/* Return true if Name is the name of a local symbol */
{
    return (*Name == LocalStart);
}



int IsLocalNameId (unsigned Name)
/* Return true if Name is the name of a local symbol */
{
    return (*GetString (Name) == LocalStart);
}



SymEntry* NewSymEntry (const char* Name)
/* Allocate a symbol table entry, initialize and return it */
{
    /* Allocate memory */
    SymEntry* S = xmalloc (sizeof (SymEntry));

    /* Initialize the entry */
    S->Left	= 0;
    S->Right	= 0;
    S->Locals	= 0;
    S->SymTab	= 0;
    S->Pos	= CurPos;
    S->Flags	= 0;
    S->V.Expr	= 0;
    S->ExprRefs = AUTO_COLLECTION_INITIALIZER;
    memset (S->ConDesPrio, 0, sizeof (S->ConDesPrio));
    S->Name     = GetStringId (Name);

    /* Insert it into the list of all entries */
    S->List = SymList;
    SymList = S;

    /* Return the initialized entry */
    return S;
}



void SymRef (SymEntry* S)
/* Mark the given symbol as referenced */
{
    /* Mark the symbol as referenced */
    S->Flags |= SF_REFERENCED;
}



void SymDef (SymEntry* S, ExprNode* Expr, unsigned Flags)
/* Define a new symbol */
{
    if (S->Flags & SF_IMPORT) {
       	/* Defined symbol is marked as imported external symbol */
       	Error (ERR_SYM_ALREADY_IMPORT, GetSymName (S));
       	return;
    }
    if (S->Flags & SF_DEFINED) {
       	/* Multiple definition */
       	Error (ERR_SYM_ALREADY_DEFINED, GetSymName (S));
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
    if (!IsLocalNameId (S->Name)) {
       	SymLast = S;
    }
}



int SymIsDef (const SymEntry* S)
/* Return true if the given symbol is already defined */
{
    return (S->Flags & SF_DEFINED) != 0;
}



int SymIsRef (const SymEntry* S)
/* Return true if the given symbol has been referenced */
{
    return (S->Flags & SF_REFERENCED) != 0;
}



int SymIsImport (const SymEntry* S)
/* Return true if the given symbol is marked as import */
{
    /* Resolve trampoline entries */
    if (S->Flags & SF_TRAMPOLINE) {
	S = S->V.Sym;
    }

    /* Check the import flag */
    return (S->Flags & SF_IMPORT) != 0;
}



int SymHasExpr (const SymEntry* S)
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



struct ExprNode* GetSymExpr (SymEntry* S)
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




