/*****************************************************************************/
/*                                                                           */
/*				   condasm.c				     */
/*                                                                           */
/*		     Conditional assembly support for ca65		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
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



/* ca65 */
#include "error.h"
#include "expr.h"
#include "instr.h"
#include "nexttok.h"
#include "symbol.h"
#include "symtab.h"
#include "condasm.h"



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



/* Maximum count of nested .ifs */
#define MAX_IFS		256

/* Set of bitmapped flags for the if descriptor */
enum {
    ifNone	= 0x0000,		/* No flag */
    ifCond	= 0x0001,		/* IF condition was true */
    ifElse	= 0x0002,		/* We had a .ELSE branch */
    ifNeedTerm	= 0x0004		/* Need .ENDIF termination */
};



/*****************************************************************************/
/*				 struct IfDesc				     */
/*****************************************************************************/



/* One .IF descriptor */
typedef struct IfDesc IfDesc;
struct IfDesc {
    unsigned   	Flags; 	       	/* Bitmapped flags, see above */
    FilePos    	Pos;		/* File position of the .IF */
    const char* Name;	      	/* Name of the directive */
};

/* The .IF stack */
static IfDesc IfStack [MAX_IFS];
static unsigned IfCount = 0;



static IfDesc* AllocIf (const char* Directive, int NeedTerm)
/* Alloc a new element from the .IF stack */
{
    IfDesc* ID;

    /* Check for stack overflow */
    if (IfCount >= MAX_IFS) {
       	Fatal ("Too many nested .IFs");
    }

    /* Alloc one element */
    ID = &IfStack [IfCount++];

    /* Initialize elements */
    ID->Flags = NeedTerm? ifNeedTerm : ifNone;
    ID->Pos   = CurPos;
    ID->Name  = Directive;

    /* Return the result */
    return ID;
}



static IfDesc* GetCurrentIf (void)
/* Return the current .IF descriptor */
{
    if (IfCount == 0) {
       	return 0;
    } else {
        return &IfStack [IfCount-1];
    }
}



static void FreeIf (void)
/* Free all .IF descriptors until we reach one with the NeedTerm bit set */
{
    int Done = 0;
    do {
       	IfDesc* D = GetCurrentIf();
       	if (D == 0) {
       	    Error (" Unexpected .ENDIF");
	    Done = 1;
       	} else {
       	    Done = (D->Flags & ifNeedTerm) != 0;
            --IfCount;
       	}
    } while (!Done);
}



static int GetCurrentIfCond (void)
/* Return the current condition based on all conditions on the stack */
{
    unsigned Count;
    for (Count = 0; Count < IfCount; ++Count) {
       	if ((IfStack[Count].Flags & ifCond) == 0) {
       	    return 0;
       	}
    }
    return 1;
}



static void SetIfCond (IfDesc* ID, int C)
/* Set the .IF condition */
{
    if (C) {
       	ID->Flags |= ifCond;
    } else {
       	ID->Flags &= ~ifCond;
    }
}



static void InvertIfCond (IfDesc* ID)
/* Invert the current condition */
{
    ID->Flags ^= ifCond;
}



static int GetElse (const IfDesc* ID)
/* Return true if we had a .ELSE */
{
    return (ID->Flags & ifElse) != 0;
}



static void SetElse (IfDesc* ID, int E)
/* Set the .ELSE flag */
{
    if (E) {
	ID->Flags |= ifElse;
    } else {
	ID->Flags &= ~ifElse;
    }
}



/*****************************************************************************/
/*     	       	    	   	     Code			     	     */
/*****************************************************************************/



void DoConditionals (void)
/* Catch all for conditional directives */
{
    IfDesc* D;

    int IfCond = GetCurrentIfCond ();
    do {

    	switch (Tok) {

    	    case TOK_ELSE:
       	        D = GetCurrentIf ();
	       	if (D == 0) {
	       	    Error ("Unexpected .ELSE");
       	       	} else if (GetElse(D)) {
	       	    /* We already had a .ELSE ! */
	       	    Error ("Duplicate .ELSE");
       	       	} else {
	       	    /* Allow an .ELSE */
	  	    InvertIfCond (D);
	  	    SetElse (D, 1);
	  	    D->Pos = CurPos;
	  	    D->Name = ".ELSE";
	  	    IfCond = GetCurrentIfCond ();
	  	}
	       	NextTok ();
       	       	break;

       	    case TOK_ELSEIF:
	        D = GetCurrentIf ();
	       	if (D == 0) {
	       	    Error ("Unexpected .ELSEIF");
	       	} else if (GetElse(D)) {
	       	    /* We already had a .ELSE */
	       	    Error ("Duplicate .ELSE");
	       	} else {
	       	    /* Handle as if there was an .ELSE first */
	       	    InvertIfCond (D);
	       	    SetElse (D, 1);

		    /* Allocate and prepare a new descriptor */
       	       	    D = AllocIf (".ELSEIF", 0);
		    NextTok ();

		    /* Ignore the new condition if we are inside a false .ELSE
		     * branch. This way we won't get any errors about undefined
		     * symbols or similar...
		     */
		    if (IfCond == 0) {
		       	SetIfCond (D, ConstExpression ());
		    }

		    /* Get the new overall condition */
		    IfCond = GetCurrentIfCond ();
		}
		break;

	    case TOK_ENDIF:
		/* We're done with this .IF.. - remove the descriptor(s) */
		FreeIf ();

	       	/* Be sure not to read the next token until the .IF stack
		 * has been cleanup up, since we may be at end of file.
		 */
		NextTok ();

		/* Get the new overall condition */
	        IfCond = GetCurrentIfCond ();
		break;

    	    case TOK_IF:
	      	D = AllocIf (".IF", 1);
	      	NextTok ();
		if (IfCond) {
		    SetIfCond (D, ConstExpression ());
		}
	      	IfCond = GetCurrentIfCond ();
	      	break;

	    case TOK_IFBLANK:
		D = AllocIf (".IFBLANK", 1);
		NextTok ();
		if (IfCond) {
                    if (TokIsSep (Tok)) {
                        SetIfCond (D, 1);
                    } else {
		        SetIfCond (D, 0);
                        SkipUntilSep ();
                    }
		}
		IfCond = GetCurrentIfCond ();
		break;

	    case TOK_IFCONST:
		D = AllocIf (".IFCONST", 1);
		NextTok ();
		if (IfCond) {
		    ExprNode* Expr = Expression();
		    SetIfCond (D, IsConstExpr (Expr));
		    FreeExpr (Expr);
		}
		IfCond = GetCurrentIfCond ();
		break;

    	    case TOK_IFDEF:
	        D = AllocIf (".IFDEF", 1);
		NextTok ();
		if (IfCond) {
		    SymEntry* Sym = ParseScopedSymName (SYM_FIND_EXISTING);
		    SetIfCond (D, Sym != 0 && SymIsDef (Sym));
		}
	        IfCond = GetCurrentIfCond ();
		break;

	    case TOK_IFNBLANK:
	     	D = AllocIf (".IFNBLANK", 1);
		NextTok ();
		if (IfCond) {
                    if (TokIsSep (Tok)) {
                        SetIfCond (D, 0);
                    } else {
		        SetIfCond (D, 1);
                        SkipUntilSep ();
                    }
		}
		IfCond = GetCurrentIfCond ();
		break;

	    case TOK_IFNCONST:
		D = AllocIf (".IFNCONST", 1);
		NextTok ();
		if (IfCond) {
		    ExprNode* Expr = Expression();
		    SetIfCond (D, !IsConstExpr (Expr));
		    FreeExpr (Expr);
		}
		IfCond = GetCurrentIfCond ();
		break;

    	    case TOK_IFNDEF:
	        D = AllocIf (".IFNDEF", 1);
		NextTok ();
		if (IfCond) {
		    SymEntry* Sym = ParseScopedSymName (SYM_FIND_EXISTING);
		    SetIfCond (D, Sym == 0 || !SymIsDef (Sym));
		}
	        IfCond = GetCurrentIfCond ();
    	     	break;

	    case TOK_IFNREF:
	        D = AllocIf (".IFNREF", 1);
		NextTok ();
		if (IfCond) {
		    SymEntry* Sym = ParseScopedSymName (SYM_FIND_EXISTING);
		    SetIfCond (D, Sym == 0 || !SymIsRef (Sym));
	     	}
	        IfCond = GetCurrentIfCond ();
		break;

	    case TOK_IFP02:
       	       	D = AllocIf (".IFP02", 1);
		NextTok ();
		if (IfCond) {
		    SetIfCond (D, GetCPU() == CPU_6502);
		}
		IfCond = GetCurrentIfCond ();
		break;

	    case TOK_IFP816:
		D = AllocIf (".IFP816", 1);
		NextTok ();
		if (IfCond) {
       	       	    SetIfCond (D, GetCPU() == CPU_65816);
		}
		IfCond = GetCurrentIfCond ();
		break;

	    case TOK_IFPC02:
		D = AllocIf (".IFPC02", 1);
		NextTok ();
		if (IfCond) {
       	       	    SetIfCond (D, GetCPU() == CPU_65C02);
		}
		IfCond = GetCurrentIfCond ();
		break;

	    case TOK_IFPSC02:
		D = AllocIf (".IFPSC02", 1);
		NextTok ();
		if (IfCond) {
       	       	    SetIfCond (D, GetCPU() == CPU_65SC02);
		}
		IfCond = GetCurrentIfCond ();
		break;

	    case TOK_IFREF:
	        D = AllocIf (".IFREF", 1);
     		NextTok ();
     		if (IfCond) {
		    SymEntry* Sym = ParseScopedSymName (SYM_FIND_EXISTING);
		    SetIfCond (D, Sym != 0 && SymIsRef (Sym));
     		}
     	        IfCond = GetCurrentIfCond ();
     		break;

     	    default:
     		/* Skip tokens */
     		NextTok ();

     	}

    } while (IfCond == 0 && Tok != TOK_EOF);
}



void CheckOpenIfs (void)
/* Called from the scanner before closing an input file. Will check for any
 * open .ifs in this file.
 */
{
    while (1) {
	/* Get the current file number and check if the topmost entry on the
	 * .IF stack was inserted with this file number
	 */
	IfDesc* D = GetCurrentIf ();
	if (D == 0) {
	    /* There are no open .IFs */
	    break;
	}

	if (D->Pos.Name != CurPos.Name) {
	    /* The .if is from another file, bail out */
	    break;
	}

       	/* Start of .if is in the file we're about to leave */
	PError (&D->Pos, "Conditional assembly branch was never closed");
	FreeIf ();
    }
}



unsigned GetIfStack (void)
/* Get the current .IF stack pointer */
{
    return IfCount;
}



void CleanupIfStack (unsigned SP)
/* Cleanup the .IF stack, remove anything above the given stack pointer */
{
    while (IfCount > SP) {
	FreeIf ();
    }
}



