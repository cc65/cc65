/*****************************************************************************/
/*                                                                           */
/*				   parser.c				     */
/*                                                                           */
/*			       Expression parser			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000	 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* common */
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "datatype.h"
#include "declare.h"
#include "error.h"
#include "exprheap.h"
#include "funcdesc.h"
#include "function.h"
#include "global.h"
#include "litpool.h"
#include "macrotab.h"
#include "preproc.h"
#include "scanner.h"
#include "stdfunc.h"
#include "symtab.h"
#include "typecmp.h"
#include "parser.h"



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



static ExprNode* UnaryExpr (void);
ExprNode* Expr1 (void);
ExprNode* Expr0 (void);



/*****************************************************************************/
/*			       Helper functions				     */
/*****************************************************************************/



static int IsTypeExpr (void)
/* Return true if some sort of variable or type is waiting (helper for cast
 * and sizeof() in hie10).
 */
{
    SymEntry* Entry;

    return curtok == TOK_LPAREN && (
       	    (nxttok >= TOK_FIRSTTYPE && nxttok <= TOK_LASTTYPE) ||
	    (nxttok == TOK_CONST)                           	||
       	    (nxttok  == TOK_IDENT 			      	&&
	    (Entry = FindSym (NextTok.Ident)) != 0  		&&
	    IsTypeDef (Entry))
           );
}



/*****************************************************************************/
/*		       Expression node helper functions			     */
/*****************************************************************************/



static ExprNode* GetIntNode (int Value)
/* Allocate a new expression node from the tree, make it a valid integer
 * node and return it. Often used if an error occurs to get a safe expression
 * tree.
 */
{
    ExprNode* N = AllocExprNode (NT_CONST, type_int, RVALUE);
    N->V.I = Value;
    return N;
}



/*****************************************************************************/
/*   	     			     Code				     */
/*****************************************************************************/



ExprNode* DoAsm (void)
/* This function parses ASM statements. The syntax of the ASM directive
 * looks like the one defined for C++ (C has no ASM directive), that is,
 * a string literal in parenthesis.
 */
{
    ExprNode* N;


    /* Skip the ASM */
    NextToken ();

    /* Need left parenthesis */
    ConsumeLParen ();

    /* Create a new expression node and assign a void type */
    N = AllocExprNode (NT_ASM, type_void, RVALUE);

    /* String literal */
    if (curtok != TOK_SCONST) {

       	/* Print an error */
     	Error (ERR_STRLIT_EXPECTED);

	/* To be on the safe side later, insert an empty asm string */
	AppendItem (N, xstrdup (""));

    } else {

	/* Insert a copy of the string into the expression node */
	AppendItem (N, xstrdup (GetLiteral (curval)));

     	/* Reset the string pointer, effectivly clearing the string from the
     	 * string table. Since we're working with one token lookahead, this
     	 * will fail if the next token is also a string token, but that's a
     	 * syntax error anyway, because we expect a right paren.
     	 */
     	ResetLiteralOffs (curval);
    }

    /* Skip the string token */
    NextToken ();

    /* Closing paren needed */
    ConsumeRParen ();

    /* Return the created node */
    return N;
}



static ExprNode* Primary (void)
/* Evaluate a primary expression */
{
    ExprNode* N;

    /* Process a parenthesized subexpression. In this case we don't need to
     * allocate a new node ourselves.
     */
    if (curtok == TOK_LPAREN) {
       	NextToken ();
       	N = Expr0 ();
       	ConsumeRParen ();
       	return N;
    }

    /* Check for an integer or character constant */
    if (curtok == TOK_ICONST || curtok == TOK_CCONST) {

	/* Create the new node */
	N = AllocExprNode (NT_CONST, CurTok.Type, RVALUE);
       	N->V.I = CurTok.IVal;

	/* Skip the token and return the result */
    	NextToken ();
    	return N;
    }

    /* Check for a float constant */
    if (curtok == TOK_FCONST) {

	/* Create the new node */
	N = AllocExprNode (NT_CONST, CurTok.Type, RVALUE);
       	N->V.F = CurTok.FVal;

	/* Skip the token and return the result */
    	NextToken ();
    	return N;
    }

    /* All others may only be used if the expression evaluation is not called
     * recursively by the preprocessor.
     */
    if (Preprocessing) {
       	/* Illegal expression in PP mode */
	Error (ERR_CPP_EXPR_EXPECTED);

	/* Skip the token for error recovery */
	NextToken ();

	/* Return an integer constant */
	return GetIntNode (0);
    }

    /* Identifier? */
    if (curtok == TOK_IDENT) {

	/* Identifier */
	SymEntry* Sym;
	ident Ident;

	/* Get a pointer to the symbol table entry */
       	Sym = FindSym (CurTok.Ident);

	/* Is the symbol known? */
 	if (Sym) {

	    /* We found the symbol - skip the name token */
     	    NextToken ();

    	    /* Check for illegal symbol types */
	    if ((Sym->Flags & SC_LABEL) == SC_LABEL) {
	       	/* Cannot use labels in expressions */
	       	Error (ERR_SYMBOL_KIND);
	       	return GetIntNode (0);
       	    } else if (Sym->Flags & SC_TYPE) {
	       	/* Cannot use type symbols */
	       	Error (ERR_VAR_IDENT_EXPECTED);
	       	/* Assume an int type to make lval valid */
	   	return GetIntNode (0);
	    }

	    /* Handle enum values as constant integers */
       	    if ((Sym->Flags & SC_ENUM) == SC_ENUM) {

		N = GetIntNode (Sym->V.EnumVal);

	    } else {

		/* All symbols besides functions and arrays are lvalues */
		int LVal = (!IsTypeFunc (Sym->Type) && !IsTypeArray (Sym->Type));

		/* Create the node */
		N = AllocExprNode (NT_SYM, Sym->Type, LVal);

		/* Set the symbol pointer */
		SetNodeSym (N, Sym);
	    }

	    /* The symbol is referenced now */
	    Sym->Flags |= SC_REF;

	} else {

	    /* We did not find the symbol. Remember the name, then skip it */
	    strcpy (Ident, CurTok.Ident);
	    NextToken ();

	    /* IDENT is either an auto-declared function or an undefined
	     * variable.
	     */
	    if (curtok == TOK_LPAREN) {

	     	/* Warn about the use of a function without prototype */
	     	Warning (WARN_FUNC_WITHOUT_PROTO);

	     	/* Declare a function returning int. For that purpose, prepare
	     	 * a function signature for a function having an empty param
	     	 * list and returning int.
	     	 */
	     	Sym = AddGlobalSym (Ident, GetImplicitFuncType(), SC_EXTERN | SC_REF | SC_FUNC);
		N   = AllocExprNode (NT_SYM, Sym->Type, RVALUE);
	     	SetNodeSym (N, Sym);

	    } else {

	     	/* Print an error about an undeclared variable */
	     	Error (ERR_UNDEFINED_SYMBOL, Ident);

	     	/* Undeclared Variable */
	     	Sym = AddLocalSym (Ident, type_int, SC_AUTO | SC_REF, 0);
		N   = AllocExprNode (NT_SYM, Sym->Type, LVALUE);
	     	SetNodeSym (N, Sym);

	    }

	}

    } else if (curtok == TOK_SCONST) {

	/* String literal */
	N = AllocExprNode (NT_CONST, GetCharArrayType (strlen (GetLiteral (curval))), RVALUE);
       	N->V.I = curval;

    } else if (curtok == TOK_ASM) {

	/* ASM statement? */
	N = DoAsm ();

    } else if (curtok == TOK_A) {

	/* A register */
	N = AllocExprNode (NT_REG_A, type_uchar, LVALUE);

    } else if (curtok == TOK_X) {

	/* X register */
       	N = AllocExprNode (NT_REG_X, type_uchar, LVALUE);

    } else if (curtok == TOK_Y) {

	/* Y register */
       	N = AllocExprNode (NT_REG_Y, type_uchar, LVALUE);

    } else if (curtok == TOK_AX) {

	/* AX pseudo register */
       	N = AllocExprNode (NT_REG_AX, type_uint, LVALUE);

    } else if (curtok == TOK_EAX) {

	/* EAX pseudo register */
       	N = AllocExprNode (NT_REG_EAX, type_ulong, LVALUE);

    } else {

	/* Illegal primary. */
	Error (ERR_EXPR_EXPECTED);
       	N = GetIntNode (0);

    }

    /* Return the new node */
    return N;
}



static ExprNode* DoArray (ExprNode* Left)
/* Handle arrays */
{
    ExprNode* Right;
    ExprNode* Root;
    type*     ElementType;


    /* Skip the bracket */
    NextToken ();

    /* Get the index */
    Right = Expr0 ();

    /* Check the types.	As special "C" feature, accept a reversal of base and
     * index types:
     * 	 char C = 3["abcdefg"];
     * is legal C!
     */
    if (IsClassPtr (Left->Type)) {
	/* Right side must be some sort of integer */
	if (!IsClassInt (Right->Type)) {
	    /* Print an error */
	    Error (ERR_CANNOT_SUBSCRIPT);
	    /* To avoid problems later, create a new, legal subscript
	     * expression
	     */
	    Right = GetIntNode (0);
	}
    } else if (IsClassPtr (Right->Type)) {

	ExprNode* Tmp;

	/* Left side must be some sort of integer */
	if (!IsClassInt (Right->Type)) {
	    /* Print an error */
	    Error (ERR_CANNOT_SUBSCRIPT);
	    /* To avoid problems later, create a new, legal subscript
	     * expression
	     */
	    Left = GetIntNode (0);
	}

    	/* Swap the expression to it's normal form */
	Tmp   = Right;
	Right = Left;
	Left  = Tmp;

    } else {
	/* Invalid array expression. Skip the closing bracket, then return
	 * an integer instead of the array expression to be safe later.
	 */
	Error (ERR_CANNOT_SUBSCRIPT);
	ConsumeRBrack ();
	return GetIntNode (0);
    }

    /* Skip the right bracket */
    ConsumeRBrack ();

    /* Get the type of the array elements */
    ElementType = Indirect (Left->Type);

    /* Allocate the branch node for the array expression */
    Root = AllocExprNode (NT_ARRAY_SUBSCRIPT,
	   		  ElementType,
	   		  IsTypeArray (ElementType)? RVALUE : LVALUE);

    /* Setup the branches */
    SetLeftNode (Root, Left);
    SetRightNode (Root, Right);

    /* ...and return it */
    return Root;
}



static ExprNode* DoStruct (ExprNode* Left)
/* Process struct field access */
{
    nodetype_t	NT;
    ident       Ident;
    type*	StructType;
    ExprNode*   Right;
    ExprNode*   Root;
    SymEntry*	Field;


    /* Type check */
    StructType = Left->Type;
    if (curtok == TOK_PTR_REF) {
	NT = NT_STRUCTPTR_ACCESS;
	if (!IsTypePtr (StructType)) {
	    Error (ERR_STRUCT_PTR_EXPECTED);
	    return GetIntNode (0);
	}
	StructType = Indirect (StructType);
    } else {
	NT = NT_STRUCT_ACCESS;
    }
    if (!IsClassStruct (StructType)) {
	Error (ERR_STRUCT_EXPECTED);
	return GetIntNode (0);
    }

    /* Skip the token and check for an identifier */
    NextToken ();
    if (curtok != TOK_IDENT) {
    	/* Print an error */
    	Error (ERR_IDENT_EXPECTED);
    	/* Return an integer expression instead */
    	return GetIntNode (0);
    }

    /* Get the symbol table entry and check for a struct field */
    strcpy (Ident, CurTok.Ident);
    NextToken ();
    Field = FindStructField (StructType, Ident);
    if (Field == 0) {
    	/* Struct field not found */
     	Error (ERR_STRUCT_FIELD_MISMATCH, Ident);
    	/* Return an integer expression instead */
    	return GetIntNode (0);
    }

    /* Allocate and set up the right (== field) node */
    Right = AllocExprNode (NT_SYM, Field->Type, RVALUE);
    SetNodeSym (Right, Field);

    /* Allocate the branch node for the resulting expression */
    Root = AllocExprNode (NT, Right->Type,
			  IsTypeArray (Right->Type)? RVALUE : LVALUE);

    /* Setup the branches */
    SetLeftNode (Root, Left);
    SetRightNode (Root, Right);

    /* ...and return it */
    return Root;
}



static ExprNode* DoFunctionCall (ExprNode* Left)
/* Process a function call */
{
    type*	ResultType;	/* Type of function result */
    FuncDesc*	Func;	       	/* Function descriptor */
    ExprNode*	Root;		/* Function call node */
    int		Ellipsis;    	/* True if we have an open param list */
    SymEntry* 	Param;       	/* Current formal parameter */
    unsigned	ParamCount;  	/* Actual parameter count */
    unsigned	ParamSize;   	/* Number of parameter bytes */


    /* Type check */
    if (!IsTypeFunc (Left->Type) && !IsTypeFuncPtr (Left->Type)) {

	/* Call to non function */
	Error (ERR_ILLEGAL_FUNC_CALL);

	/* Free the old node */
	FreeExprNode (Left);

	/* Return something safe */
	return GetIntNode (0);
    }

    /* Get the type of the function result */
    ResultType = Left->Type;
    if (IsTypeFuncPtr (Left->Type)) {
	++ResultType;
    }
    ResultType += DECODE_SIZE + 1;   	/* Set to result type */

    /* Skip the opening parenthesis */
    NextToken ();

    /* Allocate the function call node */
    Root = AllocExprNode (NT_FUNCTION_CALL, ResultType, RVALUE);

    /* Get a pointer to the function descriptor from the type string */
    Func = GetFuncDesc (Left->Type);

    /* Initialize vars to keep gcc silent */
    Param = 0;

    /* Parse the parameter list */
    ParamSize  = 0;
    ParamCount = 0;
    Ellipsis   = 0;
    while (curtok != TOK_RPAREN) {

	/* Count arguments */
	++ParamCount;

	/* Fetch the pointer to the next argument, check for too many args */
	if (ParamCount <= Func->ParamCount) {
	    if (ParamCount == 1) {
		/* First argument */
		Param = Func->SymTab->SymHead;
	    } else {
	     	/* Next argument */
	     	Param = Param->NextSym;
	     	CHECK ((Param->Flags & SC_PARAM) != 0);
	    }
	} else if (!Ellipsis) {
	    /* Too many arguments. Do we have an open param list? */
	    if ((Func->Flags & FD_ELLIPSIS) == 0) {
	      	/* End of param list reached, no ellipsis */
     	      	Error (ERR_TOO_MANY_FUNC_ARGS);
	    }
	    /* Assume an ellipsis even in case of errors to avoid an error
	     * message for each other argument.
	     */
	    Ellipsis = 1;
     	}

       	/* Get the parameter value expression tree and add it to the parameter
	 * list. (### check expr level)
	 */
	AppendItem (Root, Expr1 ());

	/* Check for end of argument list */
     	if (curtok != TOK_COMMA) {
     	    break;
	}
     	NextToken ();
    }

    /* We need the closing bracket here */
    ConsumeRParen ();

    /* Check if we had enough parameters */
    if (ParamCount < Func->ParamCount) {
      	Error (ERR_TOO_FEW_FUNC_ARGS);
    }

    /* Return the function call node */
    return Root;
}



static ExprNode* PostfixExpr (void)
{
    /* Get the lower level expression */
    ExprNode* Root = Primary ();

    /* */
    while (curtok == TOK_LBRACK || curtok == TOK_LPAREN  ||
	   curtok == TOK_DOT    || curtok == TOK_PTR_REF ||
	   curtok == TOK_INC    || curtok == TOK_DEC) {

	/* This is for us */
	switch (curtok) {

	    case TOK_LBRACK:
		Root = DoArray (Root);
		break;

	    case TOK_LPAREN:
		Root = DoFunctionCall (Root);
		break;

	    case TOK_DOT:
	    case TOK_PTR_REF:
		Root = DoStruct (Root);
		break;

	    case TOK_INC:
 		break;

 	    case TOK_DEC:
 		break;

 	    default:
 		Internal ("Unexpected token");

 	}
    }

    /* Return the result */
    return Root;
}



static ExprNode* DoPreIncDec (void)
/* Handle preincrement and predecrement */
{
    ExprNode* Op;
    ExprNode* Root;

    /* Determine the type of the node */
    nodetype_t NT = (curtok == TOK_INC)? NT_PRE_INC : NT_PRE_DEC;

    /* Skip the operator token */
    NextToken ();

    /* Get the expression to increment or decrement */
    Op = UnaryExpr ();

    /* The operand must be an lvalue */
    if (Op->LValue == 0) {

    	/* Print a diagnostics */
     	Error (ERR_LVALUE_EXPECTED);

    	/* It is safe to return the operand expression and probably better
    	 * than returning an int, since the operand expression already has
    	 * the correct type as expected by the program at this place, and
    	 * it is even an rvalue.
    	 */
    	return Op;
    }

    /* Setup the expression tree */
    Root = AllocExprNode (NT, Op->Type, RVALUE);
    SetLeftNode (Root, Op);

    /* Return the new node */
    return Root;
}



static ExprNode* DoUnaryPlusMinus (void)
/* Handle unary +/- */
{
    ExprNode* Op;
    ExprNode* Root;

    /* Remember the current token for later, then skip it */
    token_t Tok = curtok;
    NextToken ();

    /* Get the operand */
    Op = UnaryExpr ();

    /* Type check */
    if (!IsClassInt (Op->Type) && !IsClassFloat (Op->Type)) {

	/* Display diagnostic */
	Error (ERR_SYNTAX);

	/* Free the errorneous node */
	FreeExprNode (Op);

	/* Return something that makes sense later */
	return GetIntNode (0);
    }

    /* In case of PLUS, we must do nothing */
    if (Tok == TOK_PLUS) {

	/* Use the operand unchanged */
	Root = Op;

    } else {

	/* Setup the expression tree */
	Root = AllocExprNode (NT_UNARY_MINUS, Op->Type, RVALUE);
	SetLeftNode (Root, Op);

    }

    /* Return the new node */
    return Root;
}



static ExprNode* UnaryExpr (void)
{
    /* */
    if (curtok == TOK_INC    || curtok == TOK_DEC      ||
       	curtok == TOK_PLUS   || curtok == TOK_MINUS    ||
       	curtok == TOK_AND    || curtok == TOK_STAR     ||
       	curtok == TOK_COMP   || curtok == TOK_BOOL_NOT ||
       	curtok == TOK_SIZEOF || IsTypeExpr ()) {

     	/* Check the token */
       	switch (curtok) {

     	    case TOK_INC:
     	    case TOK_DEC:
     		return DoPreIncDec ();

     	    case TOK_PLUS:
     	    case TOK_MINUS:
		return DoUnaryPlusMinus ();

     	    case TOK_AND:
     		break;

     	    case TOK_STAR:
     		break;

     	    case TOK_COMP:
     		break;

     	    case TOK_BOOL_NOT:
     		break;

	    case TOK_SIZEOF:
		break;

	    default:
		break;

	}

    } else {

	/* Call the lower level */
	return PostfixExpr ();

    }
}
