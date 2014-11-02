/*****************************************************************************/
/*                                                                           */
/*                                  expr.c                                   */
/*                                                                           */
/*                 Expression evaluation for the ld65 linker                 */
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



/* common */
#include "check.h"
#include "exprdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "global.h"
#include "error.h"
#include "fileio.h"
#include "memarea.h"
#include "segments.h"
#include "expr.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ExprNode* NewExprNode (ObjData* O, unsigned char Op)
/* Create a new expression node */
{
    /* Allocate fresh memory */
    ExprNode* N = xmalloc (sizeof (ExprNode));
    N->Op       = Op;
    N->Left     = 0;
    N->Right    = 0;
    N->Obj      = O;
    N->V.IVal   = 0;

    return N;
}



static void FreeExprNode (ExprNode* E)
/* Free a node */
{
    /* Free the memory */
    xfree (E);
}



void FreeExpr (ExprNode* Root)
/* Free the expression, Root is pointing to. */
{
    if (Root) {
        FreeExpr (Root->Left);
        FreeExpr (Root->Right);
        FreeExprNode (Root);
    }
}



int IsConstExpr (ExprNode* Root)
/* Return true if the given expression is a constant expression, that is, one
** with no references to external symbols.
*/
{
    int         Const;
    Export*     E;
    Section*    S;
    MemoryArea* M;

    if (EXPR_IS_LEAF (Root->Op)) {
        switch (Root->Op) {

            case EXPR_LITERAL:
                return 1;

            case EXPR_SYMBOL:
                /* Get the referenced export */
                E = GetExprExport (Root);
                /* If this export has a mark set, we've already encountered it.
                ** This means that the export is used to define it's own value,
                ** which in turn means, that we have a circular reference.
                */
                if (ExportHasMark (E)) {
                    CircularRefError (E);
                    Const = 0;
                } else {
                    MarkExport (E);
                    Const = IsConstExport (E);
                    UnmarkExport (E);
                }
                return Const;

            case EXPR_SECTION:
                /* A section expression is const if the segment it is in is
                ** not relocatable and already placed.
                */
                S = GetExprSection (Root);
                M = S->Seg->MemArea;
                return M != 0 && (M->Flags & MF_PLACED) != 0 && !M->Relocatable;

            case EXPR_SEGMENT:
                /* A segment is const if it is not relocatable and placed */
                M = Root->V.Seg->MemArea;
                return M != 0 && (M->Flags & MF_PLACED) != 0 && !M->Relocatable;

            case EXPR_MEMAREA:
                /* A memory area is const if it is not relocatable and placed */
                return !Root->V.Mem->Relocatable &&
                       (Root->V.Mem->Flags & MF_PLACED);

            default:
                /* Anything else is not const */
                return 0;

        }

    } else if (EXPR_IS_UNARY (Root->Op)) {

        SegExprDesc D;

        /* Special handling for the BANK pseudo function */
        switch (Root->Op) {

            case EXPR_BANK:
                /* Get segment references for the expression */
                GetSegExprVal (Root->Left, &D);

                /* The expression is const if the expression contains exactly
                ** one segment that is assigned to a memory area which has a
                ** bank attribute that is constant.
                */
                return (D.TooComplex              == 0  &&
                        D.Seg                     != 0  &&
                        D.Seg->MemArea            != 0  &&
                        D.Seg->MemArea->BankExpr  != 0  &&
                        IsConstExpr (D.Seg->MemArea->BankExpr));

            default:
                /* All others handled normal */
                return IsConstExpr (Root->Left);

        }

    } else {

        /* We must handle shortcut boolean expressions here */
        switch (Root->Op) {

            case EXPR_BOOLAND:
                if (IsConstExpr (Root->Left)) {
                    /* lhs is const, if it is zero, don't eval right */
                    if (GetExprVal (Root->Left) == 0) {
                        return 1;
                    } else {
                        return IsConstExpr (Root->Right);
                    }
                } else {
                    /* lhs not const --> tree not const */
                    return 0;
                }
                break;

            case EXPR_BOOLOR:
                if (IsConstExpr (Root->Left)) {
                    /* lhs is const, if it is not zero, don't eval right */
                    if (GetExprVal (Root->Left) != 0) {
                        return 1;
                    } else {
                        return IsConstExpr (Root->Right);
                    }
                } else {
                    /* lhs not const --> tree not const */
                    return 0;
                }
                break;

            default:
                /* All others are handled normal */
                return IsConstExpr (Root->Left) && IsConstExpr (Root->Right);
        }
    }
}



Import* GetExprImport (ExprNode* Expr)
/* Get the import data structure for a symbol expression node */
{
    /* Check that this is really a symbol */
    PRECONDITION (Expr->Op == EXPR_SYMBOL);

    /* If we have an object file, get the import from it, otherwise
    ** (internally generated expressions), get the import from the
    ** import pointer.
    */
    if (Expr->Obj) {
        /* Return the Import */
        return GetObjImport (Expr->Obj, Expr->V.ImpNum);
    } else {
        return Expr->V.Imp;
    }
}



Export* GetExprExport (ExprNode* Expr)
/* Get the exported symbol for a symbol expression node */
{
    /* Check that this is really a symbol */
    PRECONDITION (Expr->Op == EXPR_SYMBOL);

    /* Return the export for an import*/
    return GetExprImport (Expr)->Exp;
}



Section* GetExprSection (ExprNode* Expr)
/* Get the segment for a section expression node */
{
    /* Check that this is really a section node */
    PRECONDITION (Expr->Op == EXPR_SECTION);

    /* If we have an object file, get the section from it, otherwise
    ** (internally generated expressions), get the section from the
    ** section pointer.
    */
    if (Expr->Obj) {
        /* Return the export */
        return CollAt (&Expr->Obj->Sections, Expr->V.SecNum);
    } else {
        return Expr->V.Sec;
    }
}



long GetExprVal (ExprNode* Expr)
/* Get the value of a constant expression */
{
    long        Right;
    long        Left;
    long        Val;
    Section*    S;
    Export*     E;
    SegExprDesc D;

    switch (Expr->Op) {

        case EXPR_LITERAL:
            return Expr->V.IVal;

        case EXPR_SYMBOL:
            /* Get the referenced export */
            E = GetExprExport (Expr);
            /* If this export has a mark set, we've already encountered it.
            ** This means that the export is used to define it's own value,
            ** which in turn means, that we have a circular reference.
            */
            if (ExportHasMark (E)) {
                CircularRefError (E);
                Val = 0;
            } else {
                MarkExport (E);
                Val = GetExportVal (E);
                UnmarkExport (E);
            }
            return Val;

        case EXPR_SECTION:
            S = GetExprSection (Expr);
            return S->Offs + S->Seg->PC;

        case EXPR_SEGMENT:
            return Expr->V.Seg->PC;

        case EXPR_MEMAREA:
            return Expr->V.Mem->Start;

        case EXPR_PLUS:
            return GetExprVal (Expr->Left) + GetExprVal (Expr->Right);

        case EXPR_MINUS:
            return GetExprVal (Expr->Left) - GetExprVal (Expr->Right);

        case EXPR_MUL:
            return GetExprVal (Expr->Left) * GetExprVal (Expr->Right);

        case EXPR_DIV:
            Left  = GetExprVal (Expr->Left);
            Right = GetExprVal (Expr->Right);
            if (Right == 0) {
                Error ("Division by zero");
            }
            return Left / Right;

        case EXPR_MOD:
            Left  = GetExprVal (Expr->Left);
            Right = GetExprVal (Expr->Right);
            if (Right == 0) {
                Error ("Modulo operation with zero");
            }
            return Left % Right;

        case EXPR_OR:
            return GetExprVal (Expr->Left) | GetExprVal (Expr->Right);

        case EXPR_XOR:
            return GetExprVal (Expr->Left) ^ GetExprVal (Expr->Right);

        case EXPR_AND:
            return GetExprVal (Expr->Left) & GetExprVal (Expr->Right);

        case EXPR_SHL:
            return GetExprVal (Expr->Left) << GetExprVal (Expr->Right);

        case EXPR_SHR:
            return GetExprVal (Expr->Left) >> GetExprVal (Expr->Right);

        case EXPR_EQ:
            return (GetExprVal (Expr->Left) == GetExprVal (Expr->Right));

        case EXPR_NE:
            return (GetExprVal (Expr->Left) != GetExprVal (Expr->Right));

        case EXPR_LT:
            return (GetExprVal (Expr->Left) < GetExprVal (Expr->Right));

        case EXPR_GT:
            return (GetExprVal (Expr->Left) > GetExprVal (Expr->Right));

        case EXPR_LE:
            return (GetExprVal (Expr->Left) <= GetExprVal (Expr->Right));

        case EXPR_GE:
            return (GetExprVal (Expr->Left) >= GetExprVal (Expr->Right));

        case EXPR_BOOLAND:
            return GetExprVal (Expr->Left) && GetExprVal (Expr->Right);

        case EXPR_BOOLOR:
            return GetExprVal (Expr->Left) || GetExprVal (Expr->Right);

        case EXPR_BOOLXOR:
            return (GetExprVal (Expr->Left) != 0) ^ (GetExprVal (Expr->Right) != 0);

        case EXPR_MAX:
            Left = GetExprVal (Expr->Left);
            Right = GetExprVal (Expr->Right);
            return (Left > Right)? Left : Right;

        case EXPR_MIN:
            Left = GetExprVal (Expr->Left);
            Right = GetExprVal (Expr->Right);
            return (Left < Right)? Left : Right;

        case EXPR_UNARY_MINUS:
            return -GetExprVal (Expr->Left);

        case EXPR_NOT:
            return ~GetExprVal (Expr->Left);

        case EXPR_SWAP:
            Left = GetExprVal (Expr->Left);
            return ((Left >> 8) & 0x00FF) | ((Left << 8) & 0xFF00);

        case EXPR_BOOLNOT:
            return !GetExprVal (Expr->Left);

        case EXPR_BANK:
            GetSegExprVal (Expr->Left, &D);
            if (D.TooComplex || D.Seg == 0) {
                Error ("Argument for .BANK is not segment relative or too complex");
            }
            if (D.Seg->MemArea == 0) {
                Error ("Segment `%s' is referenced by .BANK but "
                       "not assigned to a memory area",
                       GetString (D.Seg->Name));
            }
            if (D.Seg->MemArea->BankExpr == 0) {
                Error ("Memory area `%s' is referenced by .BANK but "
                       "has no BANK attribute",
                       GetString (D.Seg->MemArea->Name));
            }
            return GetExprVal (D.Seg->MemArea->BankExpr);

        case EXPR_BYTE0:
            return GetExprVal (Expr->Left) & 0xFF;

        case EXPR_BYTE1:
            return (GetExprVal (Expr->Left) >> 8) & 0xFF;

        case EXPR_BYTE2:
            return (GetExprVal (Expr->Left) >> 16) & 0xFF;

        case EXPR_BYTE3:
            return (GetExprVal (Expr->Left) >> 24) & 0xFF;

        case EXPR_WORD0:
            return GetExprVal (Expr->Left) & 0xFFFF;

        case EXPR_WORD1:
            return (GetExprVal (Expr->Left) >> 16) & 0xFFFF;

        case EXPR_FARADDR:
            return GetExprVal (Expr->Left) & 0xFFFFFF;

        case EXPR_DWORD:
            return GetExprVal (Expr->Left) & 0xFFFFFFFF;

        default:
            Internal ("Unknown expression Op type: %u", Expr->Op);
            /* NOTREACHED */
            return 0;
    }
}



static void GetSegExprValInternal (ExprNode* Expr, SegExprDesc* D, int Sign)
/* Check if the given expression consists of a segment reference and only
** constant values, additions and subtractions. If anything else is found,
** set D->TooComplex to true.
** Internal, recursive routine.
*/
{
    Export* E;

    switch (Expr->Op) {

        case EXPR_LITERAL:
            D->Val += (Sign * Expr->V.IVal);
            break;

        case EXPR_SYMBOL:
            /* Get the referenced export */
            E = GetExprExport (Expr);
            /* If this export has a mark set, we've already encountered it.
            ** This means that the export is used to define it's own value,
            ** which in turn means, that we have a circular reference.
            */
            if (ExportHasMark (E)) {
                CircularRefError (E);
            } else {
                MarkExport (E);
                GetSegExprValInternal (E->Expr, D, Sign);
                UnmarkExport (E);
            }
            break;

        case EXPR_SECTION:
            if (D->Seg) {
                /* We cannot handle more than one segment reference in o65 */
                D->TooComplex = 1;
            } else {
                /* Get the section from the expression */
                Section* S = GetExprSection (Expr);
                /* Remember the segment reference */
                D->Seg = S->Seg;
                /* Add the offset of the section to the constant value */
                D->Val += Sign * (S->Offs + D->Seg->PC);
            }
            break;

        case EXPR_SEGMENT:
            if (D->Seg) {
                /* We cannot handle more than one segment reference in o65 */
                D->TooComplex = 1;
            } else {
                /* Remember the segment reference */
                D->Seg = Expr->V.Seg;
                /* Add the offset of the segment to the constant value */
                D->Val += (Sign * D->Seg->PC);
            }
            break;

        case EXPR_PLUS:
            GetSegExprValInternal (Expr->Left, D, Sign);
            GetSegExprValInternal (Expr->Right, D, Sign);
            break;

        case EXPR_MINUS:
            GetSegExprValInternal (Expr->Left, D, Sign);
            GetSegExprValInternal (Expr->Right, D, -Sign);
            break;

        default:
            /* Expression contains illegal operators */
            D->TooComplex = 1;
            break;

    }
}



void GetSegExprVal (ExprNode* Expr, SegExprDesc* D)
/* Check if the given expression consists of a segment reference and only
** constant values, additions and subtractions. If anything else is found,
** set D->TooComplex to true. The function will initialize D.
*/
{
    /* Initialize the given structure */
    D->Val        = 0;
    D->TooComplex = 0;
    D->Seg        = 0;

    /* Call our recursive calculation routine */
    GetSegExprValInternal (Expr, D, 1);
}



ExprNode* LiteralExpr (long Val, ObjData* O)
/* Return an expression tree that encodes the given literal value */
{
    ExprNode* Expr = NewExprNode (O, EXPR_LITERAL);
    Expr->V.IVal = Val;
    return Expr;
}



ExprNode* MemoryExpr (MemoryArea* Mem, long Offs, ObjData* O)
/* Return an expression tree that encodes an offset into a memory area */
{
    ExprNode* Root;

    ExprNode* Expr = NewExprNode (O, EXPR_MEMAREA);
    Expr->V.Mem = Mem;

    if (Offs != 0) {
        Root = NewExprNode (O, EXPR_PLUS);
        Root->Left = Expr;
        Root->Right = LiteralExpr (Offs, O);
    } else {
        Root = Expr;
    }

    return Root;
}



ExprNode* SegmentExpr (Segment* Seg, long Offs, ObjData* O)
/* Return an expression tree that encodes an offset into a segment */
{
    ExprNode* Root;

    ExprNode* Expr = NewExprNode (O, EXPR_SEGMENT);
    Expr->V.Seg = Seg;

    if (Offs != 0) {
        Root = NewExprNode (O, EXPR_PLUS);
        Root->Left = Expr;
        Root->Right = LiteralExpr (Offs, O);
    } else {
        Root = Expr;
    }

    return Root;
}



ExprNode* SectionExpr (Section* Sec, long Offs, ObjData* O)
/* Return an expression tree that encodes an offset into a section */
{
    ExprNode* Root;

    ExprNode* Expr = NewExprNode (O, EXPR_SECTION);
    Expr->V.Sec = Sec;

    if (Offs != 0) {
        Root = NewExprNode (O, EXPR_PLUS);
        Root->Left = Expr;
        Root->Right = LiteralExpr (Offs, O);
    } else {
        Root = Expr;
    }

    return Root;
}



ExprNode* ReadExpr (FILE* F, ObjData* O)
/* Read an expression from the given file */
{
    ExprNode* Expr;

    /* Read the node tag and handle NULL nodes */
    unsigned char Op = Read8 (F);
    if (Op == EXPR_NULL) {
        return 0;
    }

    /* Create a new node */
    Expr = NewExprNode (O, Op);

    /* Check the tag and handle the different expression types */
    if (EXPR_IS_LEAF (Op)) {
        switch (Op) {

            case EXPR_LITERAL:
                Expr->V.IVal = Read32Signed (F);
                break;

            case EXPR_SYMBOL:
                /* Read the import number */
                Expr->V.ImpNum = ReadVar (F);
                break;

            case EXPR_SECTION:
                /* Read the section number */
                Expr->V.SecNum = ReadVar (F);
                break;

            default:
                Error ("Invalid expression op: %02X", Op);

        }

    } else {

        /* Not a leaf node */
        Expr->Left = ReadExpr (F, O);
        Expr->Right = ReadExpr (F, O);

    }

    /* Return the tree */
    return Expr;
}



int EqualExpr (ExprNode* E1, ExprNode* E2)
/* Check if two expressions are identical. */
{
    /* If one pointer is NULL, both must be NULL */
    if ((E1 == 0) ^ (E2 == 0)) {
        return 0;
    }
    if (E1 == 0) {
        return 1;
    }

    /* Both pointers not NULL, check OP */
    if (E1->Op != E2->Op) {
        return 0;
    }

    /* OPs are identical, check data for leafs, or subtrees */
    switch (E1->Op) {

        case EXPR_LITERAL:
            /* Value must be identical */
            return (E1->V.IVal == E2->V.IVal);

        case EXPR_SYMBOL:
            /* Import must be identical */
            return (GetExprImport (E1) == GetExprImport (E2));

        case EXPR_SECTION:
            /* Section must be identical */
            return (GetExprSection (E1) == GetExprSection (E2));

        case EXPR_SEGMENT:
            /* Segment must be identical */
            return (E1->V.Seg == E2->V.Seg);

        case EXPR_MEMAREA:
            /* Memory area must be identical */
            return (E1->V.Mem == E2->V.Mem);

        default:
            /* Not a leaf node */
            return EqualExpr (E1->Left, E2->Left) && EqualExpr (E1->Right, E2->Right);
    }

}
