/*****************************************************************************/
/*                                                                           */
/*                                studyexpr.c                                */
/*                                                                           */
/*                         Study an expression tree                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
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



/* common */
#include "check.h"
#include "print.h"
#include "shift.h"

/* ca65 */
#include "error.h"
#include "segment.h"
#include "studyexpr.h"
#include "symtab.h"
#include "ulabel.h"



/*****************************************************************************/
/*     	      	    	       	     Code				     */
/*****************************************************************************/



ExprDesc* InitExprDesc (ExprDesc* ED)
/* Initialize an ExprDesc structure for use with StudyExpr */
{
    ED->Val  	   = 0;
    ED->TooComplex = 0;
    ED->SymCount   = 0;
    ED->SecCount   = 0;
    return ED;
}



int ExprDescIsConst (const ExprDesc* ED)
/* Return true if the expression is constant */
{
    return (ED->TooComplex == 0 && ED->SymCount == 0 && ED->SecCount == 0);
}



static void StudyBinaryExpr (ExprNode* Expr, ExprDesc* D)
/* Study a binary expression subtree. Helper function for StudyExpr. */
{
    StudyExpr (Expr->Left, D, 1);
    if (ExprDescIsConst (D)) {
        D->Left = D->Val;
        D->Val = 0;
        StudyExpr (Expr->Right, D, 1);
        if (!ExprDescIsConst (D)) {
            D->TooComplex = 1;
        }
    } else {
        D->TooComplex = 1;
    }
}



void StudyExpr (ExprNode* Expr, ExprDesc* D, int Sign)
/* Study an expression tree and place the contents into D */
{
    SymEntry* Sym;
    unsigned  Sec;
    ExprDesc  SD;
    ExprDesc  SD1;

    /* Initialize SD. This is not needed in all cases, but it's rather cheap
     * and simplifies the code below.
     */
    InitExprDesc (&SD);

    /* Study this expression node */
    switch (Expr->Op) {

    	case EXPR_LITERAL:
            D->Val += (Sign * Expr->V.Val);
    	    break;

    	case EXPR_SYMBOL:
            Sym = Expr->V.Sym;
            if (SymIsImport (Sym)) {
                if (D->SymCount == 0) {
                    D->SymCount += Sign;
                    D->SymRef = Sym;
                } else if (D->SymRef == Sym) {
                    /* Same symbol */
                    D->SymCount += Sign;
                } else {
                    /* More than one import */
                    D->TooComplex = 1;
                }
            } else if (SymHasExpr (Sym)) {
                if (SymHasUserMark (Sym)) {
                    if (Verbosity > 0) {
                        DumpExpr (Expr, SymResolve);
                    }
                    PError (GetSymPos (Sym),
                            "Circular reference in definition of symbol `%s'",
                            GetSymName (Sym));
                    D->TooComplex = 1;
                } else {
                    SymMarkUser (Sym);
                    StudyExpr (GetSymExpr (Sym), D, Sign);
                    SymUnmarkUser (Sym);
                }
            } else {
                D->TooComplex = 1;
            }
            break;

    	case EXPR_SECTION:
            Sec = Expr->V.SegNum;
            if (D->SecCount == 0) {
                D->SecCount += Sign;
                D->SecRef = Sec;
            } else if (D->SecRef == Sec) {
                /* Same section */
                D->SecCount += Sign;
            } else {
                /* More than one section */
                D->TooComplex = 1;
            }
    	    break;

	case EXPR_ULABEL:
            if (ULabCanResolve ()) {
                /* We can resolve the label */
                StudyExpr (ULabResolve (Expr->V.Val), D, Sign);
            } else {
                D->TooComplex = 1;
            }
            break;

    	case EXPR_PLUS:
       	    StudyExpr (Expr->Left, D, Sign);
       	    StudyExpr (Expr->Right, D, Sign);
    	    break;

    	case EXPR_MINUS:
    	    StudyExpr (Expr->Left, D, Sign);
    	    StudyExpr (Expr->Right, D, -Sign);
    	    break;

        case EXPR_MUL:
            InitExprDesc (&SD1);
            StudyExpr (Expr->Left, &SD, 1);
            StudyExpr (Expr->Right, &SD1, 1);
            if (SD.TooComplex == 0 && SD1.TooComplex == 0) {
                /* First calculate SD = SD*SD1 if possible */
                if (ExprDescIsConst (&SD)) {
                    /* Left is a constant */
                    SD1.Val      *= SD.Val;
                    SD1.SymCount *= SD.Val;
                    SD1.SecCount *= SD.Val;
                    SD = SD1;
                } else if (ExprDescIsConst (&SD1)) {
                    /* Right is constant */
                    SD.Val      *= SD1.Val;
                    SD.SymCount *= SD1.Val;
                    SD.SecCount *= SD1.Val;
                } else {
                    D->TooComplex = 1;
                }
                /* Now calculate D * Sign * SD */
                if (!D->TooComplex) {
                    if ((D->SymCount == 0 || SD.SymCount == 0 || D->SymRef == SD.SymRef) &&
                        (D->SecCount == 0 || SD.SecCount == 0 || D->SecRef == SD.SecRef)) {
                        D->Val      += (Sign * SD.Val);
                        if (D->SymCount == 0) {
                            D->SymRef = SD.SymRef;
                        }
                        D->SymCount += (Sign * SD.SymCount);
                        if (D->SecCount == 0) {
                            D->SecRef = SD.SecRef;
                        }
                        D->SecCount += (Sign * SD.SecCount);
                    }
                } else {
                    D->TooComplex = 1;
                }
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_DIV:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                if (SD.Val == 0) {
                    Error ("Division by zero");
                    D->TooComplex = 1;
                } else {
                    D->Val += Sign * (SD.Left / SD.Val);
                }
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_MOD:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                if (SD.Val == 0) {
                    Error ("Modulo operation with zero");
                    D->TooComplex = 1;
                } else {
                    D->Val += Sign * (SD.Left % SD.Val);
                }
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_OR:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left | SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_XOR:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left ^ SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_AND:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left & SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_SHL:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += (Sign * shl_l (SD.Left, (unsigned) SD.Val));
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_SHR:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += (Sign * shr_l (SD.Left, (unsigned) SD.Val));
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_EQ:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left == SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_NE:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left != SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_LT:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left < SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_GT:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left > SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_LE:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left <= SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_GE:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * (SD.Left >= SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_BOOLAND:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                if (SD.Val != 0) {   /* Shortcut op */
                    SD.Val = 0;
                    StudyExpr (Expr->Right, &SD, 1);
                    if (ExprDescIsConst (&SD)) {
                        D->Val += Sign * (SD.Val != 0);
                    } else {
                        D->TooComplex = 1;
                    }
                }
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_BOOLOR:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                if (SD.Val == 0) {   /* Shortcut op */
                    StudyExpr (Expr->Right, &SD, 1);
                    if (ExprDescIsConst (&SD)) {
                        D->Val += Sign * (SD.Val != 0);
                    } else {
                        D->TooComplex = 1;
                    }
                } else {
                    D->Val += Sign;
                }
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_BOOLXOR:
            StudyBinaryExpr (Expr, &SD);
            if (!SD.TooComplex) {
                D->Val += Sign * ((SD.Left != 0) ^ (SD.Val != 0));
            }
            break;

        case EXPR_UNARY_MINUS:
            StudyExpr (Expr->Left, D, -Sign);
            break;

        case EXPR_NOT:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += (Sign * ~SD.Val);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_SWAP:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += Sign * (((SD.Val >> 8) & 0x00FF) | ((SD.Val << 8) & 0xFF00));
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_BOOLNOT:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += Sign * (SD.Val != 0);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_FORCEWORD:
        case EXPR_FORCEFAR:
            /* Ignore */
            StudyExpr (Expr->Left, D, Sign);
            break;

        case EXPR_BYTE0:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += Sign * (SD.Val & 0xFF);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_BYTE1:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += Sign * ((SD.Val >> 8) & 0xFF);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_BYTE2:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += Sign * ((SD.Val >> 16) & 0xFF);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_BYTE3:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += Sign * ((SD.Val >> 24) & 0xFF);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_WORD0:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += Sign * (SD.Val & 0xFFFF);
            } else {
                D->TooComplex = 1;
            }
            break;

        case EXPR_WORD1:
            StudyExpr (Expr->Left, &SD, 1);
            if (ExprDescIsConst (&SD)) {
                D->Val += Sign * ((SD.Val >> 16) & 0xFFFF);
            } else {
                D->TooComplex = 1;
            }
            break;

        default:
	    Internal ("Unknown Op type: %u", Expr->Op);
    	    break;
    }
}



