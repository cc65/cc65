/*****************************************************************************/
/*                                                                           */
/*				    expr.c				     */
/*                                                                           */
/*		   Expression evaluation for the ld65 linker		     */
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



#include "../common/exprdefs.h"

#include "global.h"
#include "error.h"
#include "mem.h"
#include "fileio.h"
#include "segments.h"
#include "expr.h"



/*****************************************************************************/
/*	    	   	 	    Helpers				     */
/*****************************************************************************/



static ExprNode* NewExprNode (ObjData* O)
/* Create a new expression node */
{
    /* Allocate fresh memory */
    ExprNode* N = Xmalloc (sizeof (ExprNode));
    N->Op 	= EXPR_NULL;
    N->Left 	= 0;
    N->Right 	= 0;
    N->Obj	= O;
    N->V.Val	= 0;

    return N;
}



static void FreeExprNode (ExprNode* E)
/* Free a node */
{
    /* Free the memory */
    Xfree (E);
}



/*****************************************************************************/
/*   		Dump an expression tree on stdout for debugging		     */
/*****************************************************************************/



static void InternalDumpExpr (const ExprNode* Expr)
/* Dump an expression in UPN */
{
    if (Expr == 0) {
	return;
    }
    InternalDumpExpr (Expr->Left);
    InternalDumpExpr (Expr->Right);

    switch (Expr->Op) {

	case EXPR_LITERAL:
	    printf (" $%04lX", Expr->V.Val & 0xFFFF);
	    break;

	case EXPR_SYMBOL:
       	    printf (" SYM");
	    break;

	case EXPR_SEGMENT:
	    printf (" SEG");
	    break;

       	case EXPR_PLUS:
	    printf (" +");
     	    break;

       	case EXPR_MINUS:
	    printf (" -");
     	    break;

       	case EXPR_MUL:
     	    printf (" *");
     	    break;

       	case EXPR_DIV:
     	    printf (" /");
     	    break;

       	case EXPR_MOD:
     	    printf (" %%");
     	    break;

	case EXPR_OR:
     	    printf (" OR");
     	    break;

	case EXPR_XOR:
     	    printf (" XOR");
     	    break;

	case EXPR_AND:
     	    printf (" AND");
     	    break;

	case EXPR_SHL:
     	    printf (" SHL");
     	    break;

	case EXPR_SHR:
     	    printf (" SHR");
     	    break;

       	case EXPR_EQ:
     	    printf (" =");
     	    break;

       	case EXPR_NE:
     	    printf ("<>");
     	    break;

       	case EXPR_LT:
     	    printf (" <");
     	    break;

       	case EXPR_GT:
     	    printf (" >");
     	    break;

       	case EXPR_UNARY_MINUS:
	    printf (" NEG");
	    break;

       	case EXPR_NOT:
	    printf (" ~");
	    break;

       	case EXPR_LOBYTE:
	    printf (" LO");
	    break;

       	case EXPR_HIBYTE:
	    printf (" HI");
	    break;

       	case EXPR_SWAP:
	    printf (" SWAP");
	    break;

	case EXPR_BAND:
	    printf (" BOOL_AND");
	    break;

	case EXPR_BOR:
	    printf (" BOOL_OR");
	    break;

	case EXPR_BXOR:
	    printf (" BOOL_XOR");
	    break;

	case EXPR_BNOT:
    	    printf (" BOOL_NOT");
	    break;

        default:
       	    Internal ("Unknown Op type: %u", Expr->Op);

    }
}



void DumpExpr (const ExprNode* Expr)
/* Dump an expression tree to stdout */
{
    InternalDumpExpr (Expr);
    printf ("\n");
}



/*****************************************************************************/
/*     	      	    	       	     Code	      			     */
/*****************************************************************************/



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
 * with no references to external symbols.
 */
{
    int Const;
    Export* E;

    if (EXPR_IS_LEAF (Root->Op)) {
      	switch (Root->Op) {

      	    case EXPR_LITERAL:
      	      	return 1;

      	    case EXPR_SYMBOL:
		/* Get the referenced export */
       	       	E = GetExprExport (Root);
		/* If this export has a mark set, we've already encountered it.
		 * This means that the export is used to define it's own value,
		 * which in turn means, that we have a circular reference.
		 */
		if (ExportHasMark (E)) {
		    Error ("Circular reference for symbol `%s', %s(%u)",
			   E->Name, E->Obj->Files [E->Pos.Name], E->Pos.Line);
		    Const = 0;
		} else {
		    MarkExport (E);
		    Const = IsConstExport (E);
		    UnmarkExport (E);
		}
		return Const;

      	    default:
      	 	return 0;

      	}
    } else if (EXPR_IS_UNARY (Root->Op)) {

      	return IsConstExpr (Root->Left);

    } else {

      	/* We must handle shortcut boolean expressions here */
      	switch (Root->Op) {

      	    case EXPR_BAND:
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

      	    case EXPR_BOR:
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

    /* Return the import */
    return Expr->Obj->Imports [Expr->V.ImpNum];
}



Export* GetExprExport (ExprNode* Expr)
/* Get the exported symbol for a symbol expression node */
{
    /* Check that this is really a symbol */
    PRECONDITION (Expr->Op == EXPR_SYMBOL);

    /* Return the export */
    return Expr->Obj->Imports [Expr->V.ImpNum]->V.Exp;
}



Section* GetExprSection (ExprNode* Expr)
/* Get the segment for a segment expression node */
{
    /* Check that this is really a segment node */
    PRECONDITION (Expr->Op == EXPR_SEGMENT);

    /* Return the export */
    return Expr->Obj->Sections [Expr->V.SegNum];
}



long GetExprVal (ExprNode* Expr)
/* Get the value of a constant expression */
{
    long Right, Left, Val;
    Section* S;
    Export* E;

    switch (Expr->Op) {

       	case EXPR_LITERAL:
    	    return Expr->V.Val;

       	case EXPR_SYMBOL:
	    /* Get the referenced export */
       	    E = GetExprExport (Expr);
	    /* If this export has a mark set, we've already encountered it.
	     * This means that the export is used to define it's own value,
	     * which in turn means, that we have a circular reference.
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

        case EXPR_SEGMENT:
       	    S = GetExprSection (Expr);
	    return S->Offs + S->Seg->PC;

	case EXPR_MEMAREA:
       	    return Expr->V.MemArea->Start;

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

       	case EXPR_UNARY_MINUS:
	    return -GetExprVal (Expr->Left);

       	case EXPR_NOT:
	    return ~GetExprVal (Expr->Left);

       	case EXPR_LOBYTE:
	    return GetExprVal (Expr->Left) & 0xFF;

       	case EXPR_HIBYTE:
	    return (GetExprVal (Expr->Left) >> 8) & 0xFF;

        case EXPR_SWAP:
	    Left = GetExprVal (Expr->Left);
	    return ((Left >> 8) & 0x00FF) | ((Left << 8) & 0xFF00);

	case EXPR_BAND:
	    return GetExprVal (Expr->Left) && GetExprVal (Expr->Right);

	case EXPR_BOR:
	    return GetExprVal (Expr->Left) || GetExprVal (Expr->Right);

	case EXPR_BXOR:
	    return (GetExprVal (Expr->Left) != 0) ^ (GetExprVal (Expr->Right) != 0);

	case EXPR_BNOT:
       	    return !GetExprVal (Expr->Left);

        default:
       	    Internal ("Unknown expression Op type: %u", Expr->Op);
      	    /* NOTREACHED */
    	    return 0;
    }
}



ExprNode* LiteralExpr (long Val, ObjData* O)
/* Return an expression tree that encodes the given literal value */
{
    ExprNode* Expr = NewExprNode (O);
    Expr->Op = EXPR_LITERAL;
    Expr->V.Val = Val;
    return Expr;
}



ExprNode* MemExpr (Memory* Mem, long Offs, ObjData* O)
/* Return an expression tree that encodes an offset into the memory area */
{
    ExprNode* Root;

    ExprNode* Expr = NewExprNode (O);
    Expr->Op = EXPR_MEMAREA;
    Expr->V.MemArea = Mem;

    Root = NewExprNode (O);
    Root->Op = EXPR_PLUS;
    Root->Left = Expr;
    Root->Right = LiteralExpr (Offs, O);

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
    Expr = NewExprNode (O);
    Expr->Op = Op;

    /* Check the tag and handle the different expression types */
    if (EXPR_IS_LEAF (Op)) {
	switch (Op) {

	    case EXPR_LITERAL:
	   	Expr->V.Val = Read32Signed (F);
	   	break;

	    case EXPR_SYMBOL:
	   	/* Read the import number */
	   	Expr->V.ImpNum = Read16 (F);
	   	break;

	    case EXPR_SEGMENT:
	   	/* Read the segment number */
	   	Expr->V.SegNum = Read8 (F);
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
	    return (E1->V.Val == E2->V.Val);

	case EXPR_SYMBOL:
	    /* Import number must be identical */
	    return (E1->V.ImpNum == E2->V.ImpNum);

	case EXPR_SEGMENT:
       	    /* Segment number must be identical */
       	    return (E1->V.SegNum == E2->V.SegNum);

	case EXPR_MEMAREA:
       	    /* Memory area must be identical */
       	    return (E1->V.MemArea == E2->V.MemArea);

	default:
	    /* Not a leaf node */
       	    return EqualExpr (E1->Left, E2->Left) && EqualExpr (E1->Right, E2->Right);
    }

}

















