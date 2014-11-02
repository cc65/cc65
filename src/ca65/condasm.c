/*****************************************************************************/
/*                                                                           */
/*                                 condasm.c                                 */
/*                                                                           */
/*                   Conditional assembly support for ca65                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



/* ca65 */
#include "error.h"
#include "expr.h"
#include "instr.h"
#include "lineinfo.h"
#include "nexttok.h"
#include "symbol.h"
#include "symtab.h"
#include "condasm.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Maximum count of nested .ifs */
#define MAX_IFS         256

/* Set of bitmapped flags for the if descriptor */
enum {
    ifNone      = 0x0000,               /* No flag */
    ifCond      = 0x0001,               /* IF condition was true */
    ifParentCond= 0x0002,               /* IF condition of parent */
    ifElse      = 0x0004,               /* We had a .ELSE branch */
    ifNeedTerm  = 0x0008,               /* Need .ENDIF termination */
};

/* The overall .IF condition */
int IfCond      = 1;



/*****************************************************************************/
/*                               struct IfDesc                               */
/*****************************************************************************/



/* One .IF descriptor */
typedef struct IfDesc IfDesc;
struct IfDesc {
    unsigned    Flags;          /* Bitmapped flags, see above */
    Collection  LineInfos;      /* File position of the .IF */
    const char* Name;           /* Name of the directive */
};

/* The .IF stack */
static IfDesc IfStack [MAX_IFS];
static unsigned IfCount = 0;



static IfDesc* GetCurrentIf (void)
/* Return the current .IF descriptor */
{
    if (IfCount == 0) {
        return 0;
    } else {
        return &IfStack[IfCount-1];
    }
}



static int GetOverallIfCond (void)
/* Get the overall condition based on all conditions on the stack. */
{
    /* Since the last entry contains the overall condition of the parent, we
    ** must check it in combination of the current condition. If there is no
    ** last entry, the overall condition is true.
    */
    return (IfCount == 0) ||
           ((IfStack[IfCount-1].Flags & (ifCond | ifParentCond)) == (ifCond | ifParentCond));
}



static void CalcOverallIfCond (void)
/* Calculate the overall condition, based on all conditions on the stack. */
{
    IfCond = GetOverallIfCond ();
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



static void ElseClause (IfDesc* ID, const char* Directive)
/* Enter an .ELSE clause */
{
    /* Check if we have an open .IF - otherwise .ELSE is not allowed */
    if (ID == 0) {
        Error ("Unexpected %s", Directive);
        return;
    }

    /* Check for a duplicate else, then remember that we had one */
    if (ID->Flags & ifElse) {
        /* We already had a .ELSE ! */
        Error ("Duplicate .ELSE");
    }
    ID->Flags |= ifElse;

    /* Condition is inverted now */
    ID->Flags ^= ifCond;
}



static IfDesc* AllocIf (const char* Directive, int NeedTerm)
/* Alloc a new element from the .IF stack */
{
    IfDesc* ID;

    /* Check for stack overflow */
    if (IfCount >= MAX_IFS) {
        Fatal ("Too many nested .IFs");
    }

    /* Get the next element */
    ID = &IfStack[IfCount];

    /* Initialize elements */
    ID->Flags = NeedTerm? ifNeedTerm : ifNone;
    if (GetOverallIfCond ()) {
        /* The parents .IF condition is true */
        ID->Flags |= ifParentCond;
    }
    ID->LineInfos = EmptyCollection;
    GetFullLineInfo (&ID->LineInfos);
    ID->Name = Directive;

    /* One more slot allocated */
    ++IfCount;

    /* Return the result */
    return ID;
}



static void FreeIf (void)
/* Free all .IF descriptors until we reach one with the NeedTerm bit set */
{
    int Done;
    do {
        IfDesc* ID = GetCurrentIf();
        if (ID == 0) {
            Error (" Unexpected .ENDIF");
            Done = 1;
        } else {
            Done = (ID->Flags & ifNeedTerm) != 0;
            ReleaseFullLineInfo (&ID->LineInfos);
            DoneCollection (&ID->LineInfos);
            --IfCount;
        }
    } while (!Done);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void DoConditionals (void)
/* Catch all for conditional directives */
{
    IfDesc* D;

    do {

        switch (CurTok.Tok) {

            case TOK_ELSE:
                D = GetCurrentIf ();

                /* Allow an .ELSE */
                ElseClause (D, ".ELSE");

                /* Remember the data for the .ELSE */
                if (D) {
                    ReleaseFullLineInfo (&D->LineInfos);
                    GetFullLineInfo (&D->LineInfos);
                    D->Name = ".ELSE";
                }

                /* Calculate the new overall condition */
                CalcOverallIfCond ();

                /* Skip .ELSE */
                NextTok ();
                ExpectSep ();
                break;

            case TOK_ELSEIF:
                D = GetCurrentIf ();
                /* Handle as if there was an .ELSE first */
                ElseClause (D, ".ELSEIF");

                /* Calculate the new overall if condition */
                CalcOverallIfCond ();

                /* Allocate and prepare a new descriptor */
                D = AllocIf (".ELSEIF", 0);
                NextTok ();

                /* Ignore the new condition if we are inside a false .ELSE
                ** branch. This way we won't get any errors about undefined
                ** symbols or similar...
                */
                if (IfCond) {
                    SetIfCond (D, ConstExpression ());
                    ExpectSep ();
                }

                /* Get the new overall condition */
                CalcOverallIfCond ();
                break;

            case TOK_ENDIF:
                /* We're done with this .IF.. - remove the descriptor(s) */
                FreeIf ();

                /* Be sure not to read the next token until the .IF stack
                ** has been cleanup up, since we may be at end of file.
                */
                NextTok ();
                ExpectSep ();

                /* Get the new overall condition */
                CalcOverallIfCond ();
                break;

            case TOK_IF:
                D = AllocIf (".IF", 1);
                NextTok ();
                if (IfCond) {
                    SetIfCond (D, ConstExpression ());
                    ExpectSep ();
                }
                CalcOverallIfCond ();
                break;

            case TOK_IFBLANK:
                D = AllocIf (".IFBLANK", 1);
                NextTok ();
                if (IfCond) {
                    if (TokIsSep (CurTok.Tok)) {
                        SetIfCond (D, 1);
                    } else {
                        SetIfCond (D, 0);
                        SkipUntilSep ();
                    }
                }
                CalcOverallIfCond ();
                break;

            case TOK_IFCONST:
                D = AllocIf (".IFCONST", 1);
                NextTok ();
                if (IfCond) {
                    ExprNode* Expr = Expression();
                    SetIfCond (D, IsConstExpr (Expr, 0));
                    FreeExpr (Expr);
                    ExpectSep ();
                }
                CalcOverallIfCond ();
                break;

            case TOK_IFDEF:
                D = AllocIf (".IFDEF", 1);
                NextTok ();
                if (IfCond) {
                    SymEntry* Sym = ParseAnySymName (SYM_FIND_EXISTING);
                    SetIfCond (D, Sym != 0 && SymIsDef (Sym));
                }
                CalcOverallIfCond ();
                break;

            case TOK_IFNBLANK:
                D = AllocIf (".IFNBLANK", 1);
                NextTok ();
                if (IfCond) {
                    if (TokIsSep (CurTok.Tok)) {
                        SetIfCond (D, 0);
                    } else {
                        SetIfCond (D, 1);
                        SkipUntilSep ();
                    }
                }
                CalcOverallIfCond ();
                break;

            case TOK_IFNCONST:
                D = AllocIf (".IFNCONST", 1);
                NextTok ();
                if (IfCond) {
                    ExprNode* Expr = Expression();
                    SetIfCond (D, !IsConstExpr (Expr, 0));
                    FreeExpr (Expr);
                    ExpectSep ();
                }
                CalcOverallIfCond ();
                break;

            case TOK_IFNDEF:
                D = AllocIf (".IFNDEF", 1);
                NextTok ();
                if (IfCond) {
                    SymEntry* Sym = ParseAnySymName (SYM_FIND_EXISTING);
                    SetIfCond (D, Sym == 0 || !SymIsDef (Sym));
                    ExpectSep ();
                }
                CalcOverallIfCond ();
                break;

            case TOK_IFNREF:
                D = AllocIf (".IFNREF", 1);
                NextTok ();
                if (IfCond) {
                    SymEntry* Sym = ParseAnySymName (SYM_FIND_EXISTING);
                    SetIfCond (D, Sym == 0 || !SymIsRef (Sym));
                    ExpectSep ();
                }
                CalcOverallIfCond ();
                break;

            case TOK_IFP02:
                D = AllocIf (".IFP02", 1);
                NextTok ();
                if (IfCond) {
                    SetIfCond (D, GetCPU() == CPU_6502);
                }
                ExpectSep ();
                CalcOverallIfCond ();
                break;

            case TOK_IFP816:
                D = AllocIf (".IFP816", 1);
                NextTok ();
                if (IfCond) {
                    SetIfCond (D, GetCPU() == CPU_65816);
                }
                ExpectSep ();
                CalcOverallIfCond ();
                break;

            case TOK_IFPC02:
                D = AllocIf (".IFPC02", 1);
                NextTok ();
                if (IfCond) {
                    SetIfCond (D, GetCPU() == CPU_65C02);
                }
                ExpectSep ();
                CalcOverallIfCond ();
                break;

            case TOK_IFPSC02:
                D = AllocIf (".IFPSC02", 1);
                NextTok ();
                if (IfCond) {
                    SetIfCond (D, GetCPU() == CPU_65SC02);
                }
                ExpectSep ();
                CalcOverallIfCond ();
                break;

            case TOK_IFREF:
                D = AllocIf (".IFREF", 1);
                NextTok ();
                if (IfCond) {
                    SymEntry* Sym = ParseAnySymName (SYM_FIND_EXISTING);
                    SetIfCond (D, Sym != 0 && SymIsRef (Sym));
                    ExpectSep ();
                }
                CalcOverallIfCond ();
                break;

            default:
                /* Skip tokens */
                NextTok ();

        }

    } while (IfCond == 0 && CurTok.Tok != TOK_EOF);
}



int CheckConditionals (void)
/* Check if the current token is one that starts a conditional directive, and
** call DoConditionals if so. Return true if a conditional directive was found,
** return false otherwise.
*/
{
    switch (CurTok.Tok) {
        case TOK_ELSE:
        case TOK_ELSEIF:
        case TOK_ENDIF:
        case TOK_IF:
        case TOK_IFBLANK:
        case TOK_IFCONST:
        case TOK_IFDEF:
        case TOK_IFNBLANK:
        case TOK_IFNCONST:
        case TOK_IFNDEF:
        case TOK_IFNREF:
        case TOK_IFP02:
        case TOK_IFP816:
        case TOK_IFPC02:
        case TOK_IFPSC02:
        case TOK_IFREF:
            DoConditionals ();
            return 1;

        default:
            return 0;
    }
}



void CheckOpenIfs (void)
/* Called from the scanner before closing an input file. Will check for any
** open .ifs in this file.
*/
{
    const LineInfo* LI;

    while (1) {
        /* Get the current file number and check if the topmost entry on the
        ** .IF stack was inserted with this file number
        */
        IfDesc* D = GetCurrentIf ();
        if (D == 0) {
            /* There are no open .IFs */
            break;
        }

        LI = CollConstAt (&D->LineInfos, 0);
        if (GetSourcePos (LI)->Name != CurTok.Pos.Name) {
            /* The .if is from another file, bail out */
            break;
        }

        /* Start of .if is in the file we're about to leave */
        LIError (&D->LineInfos, "Conditional assembly branch was never closed");
        FreeIf ();
    }

    /* Calculate the new overall .IF condition */
    CalcOverallIfCond ();
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

    /* Calculate the new overall .IF condition */
    CalcOverallIfCond ();
}
