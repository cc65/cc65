/*****************************************************************************/
/*                                                                           */
/*				   parser.c				     */
/*                                                                           */
/*			       Expression parser			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2001 Ullrich von Bassewitz                                       */
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
ExprNode* AssignExpr (void);
ExprNode* Expression (void);



/*****************************************************************************/
/*			       Helper functions				     */
/*****************************************************************************/



static int IsTypeExpr (void)
/* Return true if some sort of variable or type is waiting (helper for cast
 * and sizeof() in hie10).
 */
{
    SymEntry* Entry;

    return CurTok.Tok == TOK_LPAREN && (
       	    (nxttok >= TOK_FIRSTTYPE && nxttok <= TOK_LASTTYPE) ||
	    (nxttok == TOK_CONST)                           	||
       	    (nxttok  == TOK_IDENT 			      	&&
	    (Entry = FindSym (NextTok.Ident)) != 0  		&&
	    IsTypeDef (Entry))
           );
}



static int GetBoolRep (const ExprNode* N)
/* Get the boolean representation of a constant expression node */
{
    if (IsClassInt (N->Type)) {
	/* An integer constant */
	return (N->IVal != 0);
    } else if (IsClassFloat (N->Type)) {
	/* A float constant */
	return (N->FVal != 0.0);
    } else if (IsTypeArray (N->Type) && IsTypeChar (Indirect (N->Type))) {
	/* A string constant - useless but allowed */
	return 1;
    } else {
	Internal ("GetBoolRep: Unknown type");
	/* NOTREACHED */
	return 0;
    }
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
    N->IVal = Value;
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
    if (CurTok.Tok != TOK_SCONST) {

       	/* Print an error */
     	Error ("String literal expected");

	/* To be on the safe side later, insert an empty asm string */
	AppendItem (N, xstrdup (""));

    } else {

	/* Insert a copy of the string into the expression node */
	AppendItem (N, xstrdup (GetLiteral (CurTok.IVal)));

     	/* Reset the string pointer, effectivly clearing the string from the
     	 * string table. Since we're working with one token lookahead, this
     	 * will fail if the next token is also a string token, but that's a
     	 * syntax error anyway, because we expect a right paren.
     	 */
     	ResetLiteralOffs (CurTok.IVal);
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
    if (CurTok.Tok == TOK_LPAREN) {
       	NextToken ();
       	N = Expression ();
       	ConsumeRParen ();
       	return N;
    }

    /* Check for an integer or character constant */
    if (CurTok.Tok == TOK_ICONST || CurTok.Tok == TOK_CCONST) {

	/* Create the new node */
	N = AllocExprNode (NT_CONST, CurTok.Type, RVALUE);
       	N->IVal = CurTok.IVal;

	/* Skip the token and return the result */
    	NextToken ();
    	return N;
    }

    /* Check for a float constant */
    if (CurTok.Tok == TOK_FCONST) {

	/* Create the new node */
	N = AllocExprNode (NT_CONST, CurTok.Type, RVALUE);
       	N->FVal = CurTok.FVal;

	/* Skip the token and return the result */
    	NextToken ();
    	return N;
    }

    /* All others may only be used if the expression evaluation is not called
     * recursively by the preprocessor.
     */
    if (Preprocessing) {
       	/* Illegal expression in PP mode */
	Error ("Preprocessor expression expected");

	/* Skip the token for error recovery */
	NextToken ();

	/* Return an integer constant */
	return GetIntNode (0);
    }

    /* Identifier? */
    if (CurTok.Tok == TOK_IDENT) {

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
	       	Error ("Cannot use a label in an expression");
	       	return GetIntNode (0);
       	    } else if (Sym->Flags & SC_TYPE) {
	       	/* Cannot use type symbols */
	       	Error ("Cannot use a type in an expression");
	       	/* Assume an int type to make lval valid */
	   	return GetIntNode (0);
	    }

	    /* Handle constants including enum values */
       	    if ((Sym->Flags & SC_CONST) == SC_CONST) {

		N = AllocExprNode (NT_CONST, Sym->Type, RVALUE);
		N->IVal = Sym->V.ConstVal;

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
	    if (CurTok.Tok == TOK_LPAREN) {

	     	/* Warn about the use of a function without prototype */
	     	Warning ("Function call without a prototype");

	     	/* Declare a function returning int. For that purpose, prepare
	     	 * a function signature for a function having an empty param
	     	 * list and returning int.
	     	 */
	     	Sym = AddGlobalSym (Ident, GetImplicitFuncType(), SC_EXTERN | SC_REF | SC_FUNC);
		N   = AllocExprNode (NT_SYM, Sym->Type, RVALUE);
	     	SetNodeSym (N, Sym);

	    } else {

	     	/* Print an error about an undeclared variable */
	     	Error ("Undefined symbiol: `%s'", Ident);

	     	/* Undeclared Variable */
	     	Sym = AddLocalSym (Ident, type_int, SC_AUTO | SC_REF, 0);
		N   = AllocExprNode (NT_SYM, Sym->Type, LVALUE);
	     	SetNodeSym (N, Sym);

	    }

	}

    } else if (CurTok.Tok == TOK_SCONST) {

	/* String literal */
	N = AllocExprNode (NT_CONST, GetCharArrayType (strlen (GetLiteral (CurTok.IVal))), RVALUE);
       	N->IVal = CurTok.IVal;

    } else if (CurTok.Tok == TOK_ASM) {

	/* ASM statement? */
	N = DoAsm ();

    } else if (CurTok.Tok == TOK_A) {

	/* A register */
	N = AllocExprNode (NT_REG_A, type_uchar, LVALUE);

    } else if (CurTok.Tok == TOK_X) {

	/* X register */
       	N = AllocExprNode (NT_REG_X, type_uchar, LVALUE);

    } else if (CurTok.Tok == TOK_Y) {

	/* Y register */
       	N = AllocExprNode (NT_REG_Y, type_uchar, LVALUE);

    } else if (CurTok.Tok == TOK_AX) {

	/* AX pseudo register */
       	N = AllocExprNode (NT_REG_AX, type_uint, LVALUE);

    } else if (CurTok.Tok == TOK_EAX) {

	/* EAX pseudo register */
       	N = AllocExprNode (NT_REG_EAX, type_ulong, LVALUE);

    } else {

	/* Illegal primary. */
	Error ("Expression expected");
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
    Right = Expression ();

    /* Check the types.	As special "C" feature, accept a reversal of base and
     * index types: char C = 3["abcdefg"] is legal C!
     */
    if (IsClassPtr (Left->Type)) {
	/* Right side must be some sort of integer */
	if (!IsClassInt (Right->Type)) {
	    /* Print an error */
	    Error ("Invalid subscript");
	    /* To avoid problems later, create a new, legal subscript
	     * expression
	     */
	    Right = GetIntNode (0);
	}
    } else if (IsClassPtr (Right->Type)) {

	ExprNode* Tmp;

	/* Left side must be some sort of integer */
	if (!IsClassInt (Left->Type)) {
	    /* Print an error */
	    Error ("Invalid subscript");
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
	Error ("Invalid subscript");
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
    if (CurTok.Tok == TOK_PTR_REF) {
	NT = NT_STRUCTPTR_ACCESS;
	if (!IsTypePtr (StructType)) {
	    Error ("Struct pointer expected");
	    return GetIntNode (0);
	}
	StructType = Indirect (StructType);
    } else {
	NT = NT_STRUCT_ACCESS;
    }
    if (!IsClassStruct (StructType)) {
	Error ("Struct expected");
	return GetIntNode (0);
    }

    /* Skip the token and check for an identifier */
    NextToken ();
    if (CurTok.Tok != TOK_IDENT) {
    	/* Print an error */
    	Error ("Identifier expected");
    	/* Return an integer expression instead */
    	return GetIntNode (0);
    }

    /* Get the symbol table entry and check for a struct field */
    strcpy (Ident, CurTok.Ident);
    NextToken ();
    Field = FindStructField (StructType, Ident);
    if (Field == 0) {
    	/* Struct field not found */
     	Error ("Struct/union has no field named `%s'", Ident);
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
	Error ("Illegal function call");

	/* Free the old tree */
	FreeExprTree (Left);

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
    while (CurTok.Tok != TOK_RPAREN) {

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
     	      	Error ("Too many function arguments");
	    }
	    /* Assume an ellipsis even in case of errors to avoid an error
	     * message for each other argument.
	     */
	    Ellipsis = 1;
     	}

       	/* Get the parameter value expression tree and add it to the parameter
	 * list.
	 */
	AppendItem (Root, AssignExpr ());

	/* Check for end of argument list */
     	if (CurTok.Tok != TOK_COMMA) {
     	    break;
	}
     	NextToken ();
    }

    /* We need the closing bracket here */
    ConsumeRParen ();

    /* Check if we had enough parameters */
    if (ParamCount < Func->ParamCount) {
      	Error ("Too few function arguments");
    }

    /* Return the function call node */
    return Root;
}



static ExprNode* DoPostIncDec (ExprNode* Left)
/* Handle postincrement and postdecrement */
{
    ExprNode* Root;

    /* Determine the type of the node */
    nodetype_t NT = (CurTok.Tok == TOK_INC)? NT_POST_INC : NT_POST_DEC;

    /* Skip the operator token */
    NextToken ();

    /* The operand must be an lvalue */
    if (Left->LValue == 0) {

    	/* Print a diagnostics */
     	Error ("lvalue expected");

    	/* It is safe to return the operand expression and probably better
    	 * than returning an int, since the operand expression already has
    	 * the correct type as expected by the program at this place, and
    	 * it is even an rvalue.
    	 */
    	return Left;
    }

    /* Setup the expression tree */
    Root = AllocExprNode (NT, Left->Type, RVALUE);
    SetLeftNode (Root, Left);

    /* Return the new node */
    return Root;
}



static ExprNode* PostfixExpr (void)
{
    /* Get the lower level expression */
    ExprNode* Root = Primary ();

    /* */
    while (CurTok.Tok == TOK_LBRACK || CurTok.Tok == TOK_LPAREN  ||
	   CurTok.Tok == TOK_DOT    || CurTok.Tok == TOK_PTR_REF ||
	   CurTok.Tok == TOK_INC    || CurTok.Tok == TOK_DEC) {

	/* This is for us */
	switch (CurTok.Tok) {

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
 	    case TOK_DEC:
		Root = DoPostIncDec (Root);
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
    nodetype_t NT = (CurTok.Tok == TOK_INC)? NT_PRE_INC : NT_PRE_DEC;

    /* Skip the operator token */
    NextToken ();

    /* Get the expression to increment or decrement */
    Op = UnaryExpr ();

    /* The operand must be an lvalue */
    if (Op->LValue == 0) {

    	/* Print a diagnostics */
     	Error ("lvalue expected");

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
    token_t Tok = CurTok.Tok;
    NextToken ();

    /* Get the operand */
    Op = UnaryExpr ();

    /* Type check */
    if (!IsClassInt (Op->Type) && !IsClassFloat (Op->Type)) {

	/* Output diagnostic */
	Error ("Syntax error");

	/* Free the errorneous node */
	FreeExprTree (Op);

	/* Return something that makes sense later */
	return GetIntNode (0);
    }

    /* In case of PLUS, we must do nothing */
    if (Tok == TOK_PLUS) {

       	/* Return the operand unchanged */
	return Op;

    } else if (Op->NT == NT_CONST) {

	/* The value is constant, change it according to the insn */
	if (IsClassInt (Op->Type)) {
	    /* Integer */
	    Op->IVal = -Op->IVal;
	} else {
	    /* Float */
	    Op->FVal = -Op->FVal;
	}

	/* Return the operand itself */
	return Op;

    } else {

       	/* Non constant value, setup the expression tree */
	Root = AllocExprNode (NT_UNARY_MINUS, Op->Type, RVALUE);
	SetLeftNode (Root, Op);

    }

    /* Return the new node */
    return Root;
}



static ExprNode* DoComplement (void)
/* Handle ~ */
{
    ExprNode* Op;
    ExprNode* Root;

    /* Skip the operator token */
    NextToken ();

    /* Get the operand */
    Op = UnaryExpr ();

    /* Type check */
    if (!IsClassInt (Op->Type)) {

	/* Display diagnostic */
	Error ("Operation not allowed on this type");

	/* Free the errorneous node */
	FreeExprTree (Op);

	/* Return something that makes sense later */
	return GetIntNode (0);
    }

    /* If the operand is constant, handle the operation directly */
    if (Op->NT == NT_CONST) {

	/* Change the value and return the operand node */
	Op->IVal = ~Op->IVal;
	return Op;

    } else {

	/* Setup the expression tree and return the new node */
	Root = AllocExprNode (NT_COMPLEMENT, Op->Type, RVALUE);
	SetLeftNode (Root, Op);
	return Root;
    }
}



static ExprNode* DoBoolNot (void)
/* Handle ! */
{
    ExprNode* Op;
    ExprNode* Root;

    /* Skip the operator token */
    NextToken ();

    /* Get the operand */
    Op = UnaryExpr ();

    /* The boolean NOT operator eats anything - no need for a type check. */

    /* Setup the expression tree and return the new node */
    Root = AllocExprNode (NT_BOOL_NOT, type_int, RVALUE);
    SetLeftNode (Root, Op);
    return Root;
}



static ExprNode* DoAddress (void)
/* Handle the address operator & */
{
    ExprNode* Op;

    /* Skip the operator */
    NextToken ();

    /* Get the operand */
    Op = UnaryExpr ();

    /* Accept using the address operator with arrays. This is harmless, it
     * will just be as using the array without the operator.
     */
    if (IsTypeArray (Op->Type)) {
	return Op;
    }

    /* We cannot operate on rvalues */
    if (Op->LValue == 0) {

	ExprNode* Root;

    	/* Print diagnostics */
    	Error ("Cannot take address of rvalue");

    	/* Free the problematic tree */
    	FreeExprTree (Op);

    	/* Return something that is safe later */
    	Root = AllocExprNode (NT_CONST, PointerTo (type_void), 0);
    	return Root;

    }

    /* Create the operator node and return it */
    return AllocExprNode (NT_ADDRESS, PointerTo (Op->Type), RVALUE);
}



static ExprNode* DoIndirect (void)
/* Handle the indirection operaror * */
{
    ExprNode* Op;
    type*     ResultType;
    int	      LVal;

    /* Skip the operator */
    NextToken ();

    /* Get the operand */
    Op = UnaryExpr ();

    /* Type check */
    if (!IsClassPtr (Op->Type)) {

    	/* Print diagnostics */
    	Error ("Illegal indirection");

    	/* Free the problematic tree */
    	FreeExprTree (Op);

    	/* Return something that is safe later ### */
	return GetIntNode (0);

    }

    /* Get the type of the result */
    ResultType = Indirect (Op->Type);

    /* The result is an lvalue if it is not an array */
    LVal = IsTypeArray (ResultType)? RVALUE : LVALUE;

    /* Create the operator node and return it */
    return AllocExprNode (NT_INDIRECT, ResultType, LVal);
}



static ExprNode* DoSizeOf (void)
/* Handle the sizeof operator */
{
    ExprNode*     N;
    unsigned long Size;

    /* Skip the left paren */
    NextToken ();

    /* A type or an actual variable access may follow */
    if (IsTypeExpr ()) {

       	type	Type[MAXTYPELEN];

       	/* A type in parenthesis. Skip the left paren. */
       	NextToken ();

       	/* Read the type and calculate the size. */
       	Size = SizeOf (ParseType (Type));

       	/* Closing paren must follow */
       	ConsumeRParen ();

    } else {

       	/* Some other entity */
       	N = UnaryExpr ();

       	/* Get the size */
       	Size = SizeOf (N->Type);

       	/* Free the node */
       	FreeExprTree (N);

    }

    /* Create a constant node with type size_t and return it */
    N = AllocExprNode (NT_CONST, type_size_t, RVALUE);
    N->IVal = Size;
    return N;
}



static ExprNode* DoTypeCast (void)
/* Handle type casts */
{
    type      TargetType[MAXTYPELEN];
    ExprNode* Op;
    ExprNode* Root;

    /* Skip the left paren */
    NextToken ();

    /* Read the type */
    ParseType (TargetType);

    /* Closing paren */
    ConsumeRParen ();

    /* Read the expression we have to cast */
    Op = UnaryExpr ();

    /* As a minor optimization, check if the type is already correct. If so,
     * do nothing.
     */
    if (TypeCmp (TargetType, Op->Type) >= TC_EQUAL) {

	/* Just return the operand as is */
	return Op;

    } else {

    	/* Must be casted. Setup the expression tree and return the new node */
    	Root = AllocExprNode (NT_BOOL_NOT, TargetType, RVALUE);
	SetLeftNode (Root, Op);
	return Root;

    }
}



static ExprNode* UnaryExpr (void)
{
    /* */
    if (CurTok.Tok == TOK_INC    || CurTok.Tok == TOK_DEC      ||
       	CurTok.Tok == TOK_PLUS   || CurTok.Tok == TOK_MINUS    ||
       	CurTok.Tok == TOK_COMP   || CurTok.Tok == TOK_BOOL_NOT ||
       	CurTok.Tok == TOK_AND    || CurTok.Tok == TOK_STAR     ||
       	CurTok.Tok == TOK_SIZEOF || IsTypeExpr ()) {

     	/* Check the token */
       	switch (CurTok.Tok) {

     	    case TOK_INC:
     	    case TOK_DEC:
     		return DoPreIncDec ();

     	    case TOK_PLUS:
     	    case TOK_MINUS:
       		return DoUnaryPlusMinus ();

     	    case TOK_COMP:
		return DoComplement ();

     	    case TOK_BOOL_NOT:
		return DoBoolNot ();

     	    case TOK_AND:
     		return DoAddress ();

     	    case TOK_STAR:
     		return DoIndirect ();

	    case TOK_SIZEOF:
	       	return DoSizeOf ();

    	    default:
    	     	/* Type cast */
    	     	return DoTypeCast ();

    	}

    } else {

    	/* Call the lower level */
    	return PostfixExpr ();

    }
}



static ExprNode* DoMul (ExprNode* Left)
/* Handle multiplication */
{
    type      TargetType[MAXTYPELEN];
    ExprNode* Right;
    ExprNode* Root;

    /* Check the type of the left operand */
    if (!IsClassInt (Left->Type) && !IsClassFloat (Left->Type)) {
     	Error ("Invalid left operand to binary operator `*'");
    	FreeExprTree (Left);
    	Left = GetIntNode (0);
    }

    /* Skip the operator token */
    NextToken ();

    /* Read the right expression */
    Right = UnaryExpr ();

    /* Check the type of the right operand */
    if (!IsClassInt (Right->Type) && !IsClassFloat (Right->Type)) {
     	Error ("Invalid right operand to binary operator `*'");
	FreeExprTree (Right);
	Right = GetIntNode (0);
    }

    /* Make the root node */
    Root = AllocExprNode (NT_BOOL_NOT, TargetType, RVALUE);
    SetLeftNode (Root, Left);
    SetRightNode (Root, Right);

    return Root;
}



static ExprNode* MultExpr (void)
/* Handle multiplicative expressions: '*' '/' and '%' */
{
    /* Get the left leave */
    ExprNode* Root = UnaryExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_MUL || CurTok.Tok == TOK_DIV || CurTok.Tok == TOK_MOD) {

     	switch (CurTok.Tok) {

     	    case TOK_MUL:
		Root = DoMul (Root);
		break;

     	    case TOK_DIV:
		break;

     	    case TOK_MOD:
		break;

     	    default:
     		Internal ("Unexpected token");
     	}

    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* AddExpr (void)
/* Handle additive expressions: '+' and '-' */
{
    /* Get the left leave */
    ExprNode* Root = MultExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_PLUS || CurTok.Tok == TOK_MINUS) {

     	switch (CurTok.Tok) {

     	    case TOK_PLUS:
		break;

     	    case TOK_MINUS:
		break;

     	    default:
     		Internal ("Unexpected token");
     	}

    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* ShiftExpr (void)
/* Handle shift expressions: '<<' and '>>' */
{
    /* Get the left leave */
    ExprNode* Root = AddExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_SHL || CurTok.Tok == TOK_SHR) {

     	switch (CurTok.Tok) {

     	    case TOK_SHL:
		break;

     	    case TOK_SHR:
		break;

     	    default:
     		Internal ("Unexpected token");
     	}

    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* RelationalExpr (void)
/* Handle relational expressions: '<=', '<', '>=' and '>' */
{
    /* Get the left leave */
    ExprNode* Root = ShiftExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_LE || CurTok.Tok == TOK_LT ||
	   CurTok.Tok == TOK_GE || CurTok.Tok == TOK_GT) {

     	switch (CurTok.Tok) {

     	    case TOK_LE:
		break;

     	    case TOK_LT:
		break;

     	    case TOK_GE:
		break;

     	    case TOK_GT:
		break;

     	    default:
     		Internal ("Unexpected token");
     	}

    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* EqualityExpr (void)
/* Handle equality expressions: '==' and '!=' */
{
    /* Get the left leave */
    ExprNode* Root = RelationalExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_EQ || CurTok.Tok == TOK_NE) {

     	switch (CurTok.Tok) {

     	    case TOK_EQ:
		break;

     	    case TOK_NE:
		break;

     	    default:
     		Internal ("Unexpected token");
     	}

    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* AndExpr (void)
/* Handle and expressions: '&' */
{
    /* Get the left leave */
    ExprNode* Root = EqualityExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_AND) {

	ExprNode* Left = Root;
	ExprNode* Right;

	/* Skip the token */
	NextToken ();

	/* Get the right operand */
	Right = EqualityExpr ();

	/* Type check */
	if (!IsClassInt (Left->Type) || !IsClassInt (Right->Type)) {

	    /* Print a diagnostic */
	    Error ("Operation not allowed for these types");

	    /* Remove the unneeded nodes */
	    FreeExprTree (Right);
	    FreeExprTree (Left);

	    /* Create something safe */
	    Root = GetIntNode (0);

	} else {

	    /* Check if both operands are constant */
	    if (Left->NT == NT_CONST && Right->NT == NT_CONST) {

		/* Get the constant result */
		int Result = GetBoolRep (Left) & GetBoolRep (Right);

		/* Remove the unneeded nodes */
		FreeExprTree (Right);
		FreeExprTree (Left);

		/* Create a constant result */
		Root = GetIntNode (Result);

	    } else {

		/* Make an operator node */
	 	Root = AllocExprNode (NT_AND, type_int, RVALUE);
	 	SetRightNode (Root, Right);
	 	SetLeftNode (Root, Left);

	    }
	}
    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* XorExpr (void)
/* Handle xor expressions: '^' */
{
    /* Get the left leave */
    ExprNode* Root = AndExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_XOR) {

	ExprNode* Left = Root;
	ExprNode* Right;

	/* Skip the token */
	NextToken ();

	/* Get the right operand */
	Right = AndExpr ();

	/* Type check */
	if (!IsClassInt (Left->Type) || !IsClassInt (Right->Type)) {

	    /* Print a diagnostic */
	    Error ("Operation not allowed for these types");

	    /* Remove the unneeded nodes */
	    FreeExprTree (Right);
	    FreeExprTree (Left);

	    /* Create something safe */
	    Root = GetIntNode (0);

	} else {

	    /* Check if both operands are constant */
	    if (Left->NT == NT_CONST && Right->NT == NT_CONST) {

		/* Get the constant result */
		int Result = GetBoolRep (Left) ^ GetBoolRep (Right);

		/* Remove the unneeded nodes */
		FreeExprTree (Right);
		FreeExprTree (Left);

		/* Create a constant result */
		Root = GetIntNode (Result);

	    } else {

		/* Make an operator node */
	 	Root = AllocExprNode (NT_XOR, type_int, RVALUE);
	 	SetRightNode (Root, Right);
	 	SetLeftNode (Root, Left);

	    }
	}
    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* OrExpr (void)
/* Handle or expressions: '|' */
{
    /* Get the left leave */
    ExprNode* Root = XorExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_OR) {

	ExprNode* Left = Root;
	ExprNode* Right;

	/* Skip the token */
	NextToken ();

	/* Get the right operand */
	Right = XorExpr ();

	/* Type check */
	if (!IsClassInt (Left->Type) || !IsClassInt (Right->Type)) {

	    /* Print a diagnostic */
	    Error ("Operation not allowed for these types");

	    /* Remove the unneeded nodes */
	    FreeExprTree (Right);
	    FreeExprTree (Left);

	    /* Create something safe */
	    Root = GetIntNode (0);

	} else {

	    /* Check if both operands are constant */
	    if (Left->NT == NT_CONST && Right->NT == NT_CONST) {

		/* Get the constant result */
		int Result = GetBoolRep (Left) | GetBoolRep (Right);

		/* Remove the unneeded nodes */
		FreeExprTree (Right);
		FreeExprTree (Left);

		/* Create a constant result */
		Root = GetIntNode (Result);

	    } else {

		/* Make an operator node */
	 	Root = AllocExprNode (NT_OR, type_int, RVALUE);
	 	SetRightNode (Root, Right);
	 	SetLeftNode (Root, Left);

	    }
	}
    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* BoolAndExpr (void)
/* Handle boolean and expressions: '&&' */
{
    /* Get the left leave */
    ExprNode* Root = OrExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_BOOL_AND) {

	ExprNode* Left = Root;
	ExprNode* Right;

	/* Skip the token */
	NextToken ();

	/* Get the right operand */
	Right = OrExpr ();

	/* Check if both operands are constant */
	if (Left->NT == NT_CONST && Right->NT == NT_CONST) {

	    /* Get the constant result */
	    int Result = GetBoolRep (Left) && GetBoolRep (Right);

	    /* Remove the unneeded nodes */
	    FreeExprTree (Right);
	    FreeExprTree (Left);

	    /* Create a constant result */
	    Root = GetIntNode (Result);

	} else {

	    /* Make an operator node */
	    Root = AllocExprNode (NT_BOOL_AND, type_int, RVALUE);
	    SetRightNode (Root, Right);
	    SetLeftNode (Root, Left);

	}

    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* BoolOrExpr (void)
/* Handle boolean or expressions: '||' */
{
    /* Get the left leave */
    ExprNode* Root = BoolAndExpr ();

    /* Check if this is for us */
    while (CurTok.Tok == TOK_BOOL_OR) {

	ExprNode* Left = Root;
	ExprNode* Right;

	/* Skip the token */
	NextToken ();

	/* Get the right operand */
	Right = BoolAndExpr ();

	/* Check if both operands are constant */
	if (Left->NT == NT_CONST && Right->NT == NT_CONST) {

	    /* Get the constant result */
	    int Result = GetBoolRep (Left) && GetBoolRep (Right);

	    /* Remove the unneeded nodes */
	    FreeExprTree (Right);
	    FreeExprTree (Left);

	    /* Create a constant result */
	    Root = GetIntNode (Result);

	} else {

	    /* Make an operator node */
	    Root = AllocExprNode (NT_BOOL_OR, type_int, RVALUE);
	    SetRightNode (Root, Right);
	    SetLeftNode (Root, Left);

	}

    }

    /* Return the resulting expression */
    return Root;
}



static ExprNode* ConditionalExpr (void)
/* Handle the ternary operator: ':?' */
{
    /* Get the left leave */
    ExprNode* Cond = BoolOrExpr ();

    /* Check if this is for us */
    if (CurTok.Tok == TOK_QUEST) {

	ExprNode* Expr1;
	ExprNode* Expr2;
	ExprNode* Root;
	type*	  Type;


	/* Skip the token */
	NextToken ();

	/* Get the first expression */
	Expr1 = Expression ();

	/* Colon must follow */
	ConsumeColon ();

	/* Get the second expression */
	Expr2 = ConditionalExpr ();

	/* Get the common type of the two expressions */
	Type = CommonType (Expr1->Type, Expr2->Type);

	/* Create a new ternary token node */
	Root = AllocExprNode (NT_TERNARY, Type, RVALUE);
	AppendItem (Root, Cond);
	AppendItem (Root, Expr1);
	AppendItem (Root, Expr2);

	/* Return the result */
	return Root;

    } else {

	/* Just return the lower level expression */
	return Cond;

    }
}




