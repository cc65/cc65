/*****************************************************************************/
/*                                                                           */
/*				    expr.c				     */
/*                                                                           */
/*	       Expression evaluation for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2002 Ullrich von Bassewitz                                       */
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



#include <string.h>

/* common */
#include "check.h"
#include "exprdefs.h"
#include "print.h"
#include "tgttrans.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "global.h"
#include "instr.h"
#include "nexttok.h"
#include "objcode.h"
#include "objfile.h"
#include "symtab.h"
#include "toklist.h"
#include "ulabel.h"
#include "expr.h"



/*****************************************************************************/
/*     	      	    	    	     Data				     */
/*****************************************************************************/



/* Since all expressions are first packed into expression trees, and each
 * expression tree node is allocated on the heap, we add some type of special
 * purpose memory allocation here: Instead of freeing the nodes, we save some
 * number of freed nodes for later and remember them in a single linked list
 * using the Left link.
 */
#define	MAX_FREE_NODES	64
static ExprNode*	FreeExprNodes = 0;
static unsigned		FreeNodeCount = 0;



/*****************************************************************************/
/*	    			    Helpers				     */
/*****************************************************************************/



static ExprNode* NewExprNode (void)
/* Create a new expression node */
{
    ExprNode* N;

    /* Do we have some nodes in the list already? */
    if (FreeExprNodes) {
	/* Use first node from list */
	N = FreeExprNodes;
	FreeExprNodes = N->Left;
    } else {
	/* Allocate fresh memory */
        N = xmalloc (sizeof (ExprNode));
    }
    N->Op = EXPR_NULL;
    N->Left = N->Right = 0;
    N->Obj = 0;

    return N;
}



static void FreeExprNode (ExprNode* E)
/* Free a node */
{
    if (E) {
	if (FreeNodeCount < MAX_FREE_NODES) {
	    /* Remember this node for later */
	    E->Left = FreeExprNodes;
	    FreeExprNodes = E;
	} else {
	    /* Free the memory */
	    xfree (E);
	}
    }
}



/*****************************************************************************/
/*     	      	    	       	     Code				     */
/*****************************************************************************/



static ExprNode* Expr0 (void);



int IsByteRange (long Val)
/* Return true if this is a byte value */
{
    return (Val & ~0xFFL) == 0;
}



int IsWordRange (long Val)
/* Return true if this is a word value */
{
    return (Val & ~0xFFFF) == 0;
}



static int FuncBlank (void)
/* Handle the .BLANK builtin function */
{
    /* Assume no tokens if the closing brace follows (this is not correct in
     * all cases, since the token may be the closing brace, but this will
     * give a syntax error anyway and may not be handled by .BLANK.
     */
    if (Tok == TOK_RPAREN) {
	/* No tokens */
	return 1;
    } else {
	/* Skip any tokens */
	int Braces = 0;
	while (Tok != TOK_SEP && Tok != TOK_EOF) {
	    if (Tok == TOK_LPAREN) {
		++Braces;
	    } else if (Tok == TOK_RPAREN) {
		if (Braces == 0) {
	    	    /* Done */
		    break;
		} else {
		    --Braces;
		}
	    }
	    NextTok ();
     	}
	return 0;
    }
}



static int FuncConst (void)
/* Handle the .CONST builtin function */
{
    /* Read an expression */
    ExprNode* Expr = Expression ();

    /* Check the constness of the expression */
    int Result = IsConstExpr (Expr);

    /* Free the expression */
    FreeExpr (Expr);

    /* Done */
    return Result;
}



static int FuncDefined (void)
/* Handle the .DEFINED builtin function */
{
    static const char* Keys[] = {
       	"ANY",
	"GLOBAL",
        "LOCAL",
    };

    char Name [sizeof (SVal)];
    int Result = 0;
    int Scope;

    /* First argument is a symbol name */
    if (Tok != TOK_IDENT) {
	Error (ERR_IDENT_EXPECTED);
	if (Tok != TOK_RPAREN) {
	    NextTok ();
	}
        return 0;
    }

    /* Remember the name, then skip it */
    strcpy (Name, SVal);
    NextTok ();

    /* Comma and scope spec may follow */
    if (Tok == TOK_COMMA) {

        /* Skip the comma */
        NextTok ();

        /* An identifier must follow */
        if (Tok != TOK_IDENT) {
            Error (ERR_IDENT_EXPECTED);
            return 0;
        }

        /* Get the scope, then skip it */
        Scope = GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]));
        NextTok ();

        /* Check if we got a valid keyword */
        if (Scope < 0) {
            Error (ERR_ILLEGAL_SCOPE);
            return 0;
        }

        /* Map the scope */
        switch (Scope) {
            case 0:     Scope = SCOPE_ANY;    break;
            case 1:     Scope = SCOPE_GLOBAL; break;
            case 2:     Scope = SCOPE_LOCAL;  break;
            default:    Internal ("Invalid scope: %d", Scope);
        }

    } else {

        /* Any scope */
        Scope = SCOPE_ANY;

    }

    /* Search for the symbol */
    Result = SymIsDef (SVal, Scope);

    /* Done */
    return Result;
}



static int DoMatch (enum TC EqualityLevel)
/* Handle the .MATCH and .XMATCH builtin functions */
{
    int Result;
    TokNode* Root = 0;
    TokNode* Last = 0;
    TokNode* Node = 0;

    /* A list of tokens follows. Read this list and remember it building a
     * single linked list of tokens including attributes. The list is
     * terminated by a comma.
     */
    while (Tok != TOK_COMMA) {

    	/* We may not end-of-line of end-of-file here */
    	if (Tok == TOK_SEP || Tok == TOK_EOF) {
    	    Error (ERR_UNEXPECTED_EOL);
    	    return 0;
    	}

	/* Get a node with this token */
	Node = NewTokNode ();

	/* Insert the node into the list */
	if (Last == 0) {
       	    Root = Node;
	} else {
	    Last->Next = Node;
	}
	Last = Node;

	/* Skip the token */
	NextTok ();
    }

    /* Skip the comma */
    NextTok ();

    /* Read the second list which is terminated by the right parenthesis and
     * compare each token against the one in the first list.
     */
    Result = 1;
    Node = Root;
    while (Tok != TOK_RPAREN) {

    	/* We may not end-of-line of end-of-file here */
    	if (Tok == TOK_SEP || Tok == TOK_EOF) {
    	    Error (ERR_UNEXPECTED_EOL);
    	    return 0;
    	}

	/* Compare the tokens if the result is not already known */
	if (Result != 0) {
	    if (Node == 0) {
		/* The second list is larger than the first one */
		Result = 0;
	    } else if (TokCmp (Node) < EqualityLevel) {
	 	/* Tokens do not match */
	 	Result = 0;
	    }
	}

	/* Next token in first list */
	if (Node) {
	    Node = Node->Next;
	}

       	/* Next token in current list */
	NextTok ();
    }

    /* Check if there are remaining tokens in the first list */
    if (Node != 0) {
	Result = 0;
    }

    /* Free the token list */
    while (Root) {
	Node = Root;
	Root = Root->Next;
	FreeTokNode (Node);
    }

    /* Done, return the result */
    return Result;
}



static int FuncMatch (void)
/* Handle the .MATCH function */
{
    return DoMatch (tcSameToken);
}



static int FuncReferenced (void)
/* Handle the .REFERENCED builtin function */
{
    int Result = 0;

    if (Tok != TOK_IDENT) {
	Error (ERR_IDENT_EXPECTED);
	if (Tok != TOK_RPAREN) {
	    NextTok ();
	}
    } else {
	Result = SymIsRef (SVal, SCOPE_ANY);
	NextTok ();
    }

    /* Done */
    return Result;
}



static int FuncStrAt (void)
/* Handle the .STRAT function */
{
    char Str [sizeof(SVal)];
    long Index;

    /* String constant expected */
    if (Tok != TOK_STRCON) {
      	Error (ERR_STRCON_EXPECTED);
      	NextTok ();
       	return 0;

    }

    /* Remember the string and skip it */
    strcpy (Str, SVal);
    NextTok ();

    /* Comma must follow */
    ConsumeComma ();

    /* Expression expected */
    Index = ConstExpression ();

    /* Must be a valid index */
    if (Index >= (long) strlen (Str)) {
	Error (ERR_RANGE);
	return 0;
    }

    /* Return the char, handle as unsigned. Be sure to translate it into
     * the target character set.
     */
    return (unsigned char) TgtTranslateChar (Str [(size_t)Index]);
}



static int FuncStrLen (void)
/* Handle the .STRLEN function */
{
    /* String constant expected */
    if (Tok != TOK_STRCON) {

     	Error (ERR_STRCON_EXPECTED);
    	/* Smart error recovery */
     	if (Tok != TOK_RPAREN) {
     	    NextTok ();
     	}
       	return 0;

    } else {

        /* Get the length of the string */
     	int Len = strlen (SVal);

	/* Skip the string */
	NextTok ();

	/* Return the length */
	return Len;

    }
}



static int FuncTCount (void)
/* Handle the .TCOUNT function */
{
    /* We have a list of tokens that ends with the closing paren. Skip
     * the tokens, handling nested braces and count them.
     */
    int      Count  = 0;
    unsigned Parens = 0;
    while (Parens != 0 || Tok != TOK_RPAREN) {

     	/* Check for end of line or end of input. Since the calling function
	 * will check for the closing paren, we don't need to print an error
	 * here, just bail out.
	 */
     	if (Tok == TOK_SEP || Tok == TOK_EOF) {
	    break;
     	}

	/* One more token */
	++Count;

	/* Keep track of the nesting level */
	switch (Tok) {
	    case TOK_LPAREN:	++Parens;	break;
	    case TOK_RPAREN:	--Parens;	break;
	    default:		  		break;
	}

	/* Skip the token */
	NextTok ();
    }

    /* Return the number of tokens */
    return Count;
}



static int FuncXMatch (void)
/* Handle the .XMATCH function */
{
    return DoMatch (tcIdentical);
}



static ExprNode* Function (int (*F) (void))
/* Handle builtin functions */
{
    long Result;

    /* Skip the keyword */
    NextTok ();

    /* Expression must be enclosed in braces */
    if (Tok != TOK_LPAREN) {
     	Error (ERR_LPAREN_EXPECTED);
     	SkipUntilSep ();
	return LiteralExpr (0);
    }
    NextTok ();

    /* Call the function itself */
    Result = F ();

    /* Closing brace must follow */
    ConsumeRParen ();

    /* Return an expression node with the boolean code */
    return LiteralExpr (Result);
}



static ExprNode* Factor (void)
{
    ExprNode* N;
    SymEntry* S;

    switch (Tok) {

	case TOK_INTCON:
    	    N = LiteralExpr (IVal);
       	    NextTok ();
	    break;

	case TOK_CHARCON:
    	    N = LiteralExpr (TgtTranslateChar (IVal));
       	    NextTok ();
	    break;

        case TOK_NAMESPACE:
	    NextTok ();
	    if (Tok != TOK_IDENT) {
	     	Error (ERR_IDENT_EXPECTED);
		N = LiteralExpr (0);	/* Dummy */
	    } else {
		S = SymRef (SVal, SCOPE_GLOBAL);
		if (SymIsConst (S)) {
		    /* Use the literal value instead */
		    N = LiteralExpr (GetSymVal (S));
		} else {
	    	    /* Create symbol node */
		    N = NewExprNode ();
		    N->Op    = EXPR_SYMBOL;
	   	    N->V.Sym = S;
		}
		NextTok ();
	    }
	    break;

        case TOK_IDENT:
	    S = SymRef (SVal, SCOPE_LOCAL);
	    if (SymIsConst (S)) {
	     	/* Use the literal value instead */
	     	N = LiteralExpr (GetSymVal (S));
	    } else {
	     	/* Create symbol node */
	     	N = NewExprNode ();
	     	N->Op    = EXPR_SYMBOL;
	     	N->V.Sym = S;
	    }
	    NextTok ();
	    break;

	case TOK_ULABEL:
	    N = ULabRef (IVal);
	    NextTok ();
	    break;

	case TOK_MINUS:
	    NextTok ();
	    N = NewExprNode ();
       	    N->Left = Factor ();
       	    N->Op   = EXPR_UNARY_MINUS;
     	    break;

     	case TOK_NOT:
     	    NextTok ();
     	    N = NewExprNode ();
     	    N->Left = Factor ();
     	    N->Op   = EXPR_NOT;
     	    break;

     	case TOK_STAR:
	case TOK_PC:
     	    NextTok ();
       	    N = CurrentPC ();
	    break;

	case TOK_LT:
	    NextTok ();
	    N = NewExprNode ();
	    N->Left = Factor ();
	    N->Op   = EXPR_BYTE0;
	    break;

	case TOK_GT:
	    NextTok ();
	    N = NewExprNode ();
	    N->Left = Factor ();
	    N->Op   = EXPR_BYTE1;
	    break;

	case TOK_LPAREN:
	    NextTok ();
	    N = Expr0 ();
       	    ConsumeRParen ();
	    break;

        case TOK_BLANK:
	    N = Function (FuncBlank);
	    break;

	case TOK_CONST:
	    N = Function (FuncConst);
	    break;

	case TOK_CPU:
	    N = LiteralExpr (GetCPU());
	    NextTok ();
	    break;

        case TOK_DEFINED:
	    N = Function (FuncDefined);
	    break;

	case TOK_MATCH:
	    N = Function (FuncMatch);
	    break;

        case TOK_REFERENCED:
	    N = Function (FuncReferenced);
	    break;

	case TOK_STRAT:
	    N = Function (FuncStrAt);
	    break;

	case TOK_STRLEN:
	    N = Function (FuncStrLen);
	    break;

	case TOK_TCOUNT:
	    N = Function (FuncTCount);
	    break;

	case TOK_XMATCH:
	    N = Function (FuncXMatch);
	    break;

	default:
	    if (LooseCharTerm && Tok == TOK_STRCON && strlen(SVal) == 1) {
		/* A character constant */
		N = LiteralExpr (TgtTranslateChar (SVal[0]));
	    } else {
		N = LiteralExpr (0);	/* Dummy */
		Error (ERR_SYNTAX);
	    }
	    NextTok ();
	    break;
    }
    return N;
}



static ExprNode* Term (void)
{
    ExprNode* Root;

    /* Read left hand side */
    Root = Factor ();

    /* Handle multiplicative operations */
    while (Tok == TOK_MUL || Tok == TOK_DIV || Tok == TOK_MOD ||
	   Tok == TOK_AND || Tok == TOK_XOR || Tok == TOK_SHL ||
	   Tok == TOK_SHR) {

	/* Create a new node and insert the left expression */
	ExprNode* Left = Root;
	Root = NewExprNode ();
	Root->Left = Left;

	/* Determine the operator token */
	switch (Tok) {
       	    case TOK_MUL:      	Root->Op = EXPR_MUL;	break;
	    case TOK_DIV:    	Root->Op = EXPR_DIV;   	break;
	    case TOK_MOD:      	Root->Op = EXPR_MOD;  	break;
       	    case TOK_AND:      	Root->Op = EXPR_AND;  	break;
       	    case TOK_XOR:      	Root->Op = EXPR_XOR;  	break;
       	    case TOK_SHL:      	Root->Op = EXPR_SHL;  	break;
       	    case TOK_SHR:      	Root->Op = EXPR_SHR;  	break;
	    default:   	    	Internal ("Invalid token");
      	}
	NextTok ();

	/* Parse the right hand side */
	Root->Right = Factor ();

    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* SimpleExpr (void)
{
    ExprNode* Root;

    /* Read left hand side */
    Root = Term ();

    /* Handle additive operations */
    while (Tok == TOK_PLUS || Tok == TOK_MINUS || Tok == TOK_OR) {

	/* Create a new node and insert the left expression */
	ExprNode* Left = Root;
	Root = NewExprNode ();
	Root->Left = Left;

	/* Determine the operator token */
	switch (Tok) {
       	    case TOK_PLUS:     	Root->Op = EXPR_PLUS;	break;
	    case TOK_MINUS:  	Root->Op = EXPR_MINUS; 	break;
	    case TOK_OR:  	Root->Op = EXPR_OR;  	break;
	    default:   	  	Internal ("Invalid token");
      	}
	NextTok ();

	/* Parse the right hand side */
	Root->Right = Term ();

    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* BoolExpr (void)
/* Evaluate a boolean expression */
{
    /* Read left hand side */
    ExprNode* Root = SimpleExpr ();

    /* Handle booleans */
    while (Tok == TOK_EQ || Tok == TOK_NE || Tok == TOK_LT ||
	   Tok == TOK_GT || Tok == TOK_LE || Tok == TOK_GE) {

	/* Create a new node and insert the left expression */
	ExprNode* Left = Root;
	Root = NewExprNode ();
	Root->Left = Left;

	/* Determine the operator token */
	switch (Tok) {
	    case TOK_EQ:   	Root->Op = EXPR_EQ;	break;
	    case TOK_NE:       	Root->Op = EXPR_NE;	break;
	    case TOK_LT:   	Root->Op = EXPR_LT;	break;
	    case TOK_GT:   	Root->Op = EXPR_GT;	break;
	    case TOK_LE:   	Root->Op = EXPR_LE;	break;
	    case TOK_GE:   	Root->Op = EXPR_GE;	break;
	    default:	   	Internal ("Invalid token");
      	}
	NextTok ();

	/* Parse the right hand side */
	Root->Right = SimpleExpr ();

    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* Expr2 (void)
/* Boolean operators: AND and XOR */
{
    /* Read left hand side */
    ExprNode* Root = BoolExpr ();

    /* Handle booleans */
    while (Tok == TOK_BAND || Tok == TOK_BXOR) {

	/* Create a new node and insert the left expression */
	ExprNode* Left = Root;
	Root = NewExprNode ();
	Root->Left = Left;

	/* Determine the operator token */
	switch (Tok) {
	    case TOK_BAND:	Root->Op = EXPR_BAND;	break;
	    case TOK_BXOR:	Root->Op = EXPR_BXOR; 	break;
	    default:	   	Internal ("Invalid token");
      	}
	NextTok ();

	/* Parse the right hand side */
	Root->Right = BoolExpr ();

    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* Expr1 (void)
/* Boolean operators: OR */
{
    /* Read left hand side */
    ExprNode* Root = Expr2 ();

    /* Handle booleans */
    while (Tok == TOK_BOR) {

	/* Create a new node and insert the left expression */
	ExprNode* Left = Root;
	Root = NewExprNode ();
	Root->Left = Left;

	/* Determine the operator token */
	switch (Tok) {
	    case TOK_BOR: 	Root->Op = EXPR_BOR;  	break;
	    default:  	   	Internal ("Invalid token");
      	}
	NextTok ();

	/* Parse the right hand side */
	Root->Right = Expr2 ();

    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* Expr0 (void)
/* Boolean operators: NOT */
{
    ExprNode* Root;

    /* Handle booleans */
    if (Tok == TOK_BNOT) {

       	/* Create a new node */
     	Root = NewExprNode ();

     	/* Determine the operator token */
     	switch (Tok) {
     	    case TOK_BNOT: 	Root->Op = EXPR_BNOT;	break;
     	    default:  	   	Internal ("Invalid token");
      	}
     	NextTok ();

     	/* Parse the left hand side, allow more BNOTs */
     	Root->Left = Expr0 ();

    } else {

     	/* Read left hand side */
     	Root = Expr1 ();

    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* SimplifyExpr (ExprNode* Root)
/* Try to simplify the given expression tree */
{
    if (Root) {
 	SimplifyExpr (Root->Left);
 	SimplifyExpr (Root->Right);
	if (IsConstExpr (Root)) {
	    /* The complete expression is constant */
	    Root->V.Val = GetExprVal (Root);
	    Root->Op = EXPR_LITERAL;
	    FreeExpr (Root->Left);
	    FreeExpr (Root->Right);
	    Root->Left = Root->Right = 0;
       	}
    }
    return Root;
}



ExprNode* Expression (void)
/* Evaluate an expression, build the expression tree on the heap and return
 * a pointer to the root of the tree.
 */
{
    return SimplifyExpr (Expr0 ());
}



long ConstExpression (void)
/* Parse an expression. Check if the expression is const, and print an error
 * message if not. Return the value of the expression, or a dummy, if it is
 * not constant.
 */
{
    /* Read the expression, and call finalize (exception here, since we
     * expect a const).
     */
    ExprNode* Expr = FinalizeExpr (Expression ());

    /* Return the value */
    if (IsConstExpr (Expr)) {
     	return GetExprVal (Expr);
    } else {
     	Error (ERR_CONSTEXPR_EXPECTED);
     	return 0;
    }
}



ExprNode* LiteralExpr (long Val)
/* Return an expression tree that encodes the given literal value */
{
    ExprNode* Expr = NewExprNode ();
    Expr->Op = EXPR_LITERAL;
    Expr->V.Val = Val;
    return Expr;
}



ExprNode* CurrentPC (void)
/* Return the current program counter as expression */
{
    ExprNode* Left;
    ExprNode* Root;

    if (RelocMode) {
	/* Create SegmentBase + Offset */
	Left = NewExprNode ();
	Left->Op = EXPR_SEGMENT;
	Left->V.SegNum = GetSegNum ();

	Root = NewExprNode ();
	Root->Left  = Left;
	Root->Right = LiteralExpr (GetPC ());
	Root->Op = EXPR_PLUS;
    } else {
	/* Absolute mode, just return PC value */
	Root = LiteralExpr (GetPC ());
    }

    return Root;
}



ExprNode* SwapExpr (ExprNode* Expr)
/* Return an extended expression with lo and hi bytes swapped */
{
    ExprNode* N = NewExprNode ();
    N->Op = EXPR_SWAP;
    N->Left = Expr;
    return N;
}



ExprNode* BranchExpr (unsigned Offs)
/* Return an expression that encodes the difference between current PC plus
 * offset and the target expression (that is, Expression() - (*+Offs) ).
 */
{
    ExprNode* N;
    ExprNode* Root;
    ExprNode* Left;

    /* Create *+Offs */
    if (RelocMode) {
	Left = NewExprNode ();
	Left->Op = EXPR_SEGMENT;
	Left->V.SegNum = GetSegNum ();

	N = NewExprNode ();
	N->Left  = Left;
	N->Right = LiteralExpr (GetPC () + Offs);
	N->Op = EXPR_PLUS;
    } else {
	N = LiteralExpr (GetPC () + Offs);
    }

    /* Create the root node */
    Root = NewExprNode ();
    Root->Left = Expression ();
    Root->Right = N;
    Root->Op = EXPR_MINUS;

    /* Return the result */
    return SimplifyExpr (Root);
}



ExprNode* ULabelExpr (unsigned Num)
/* Return an expression for an unnamed label with the given index */
{
    /* Get an expression node */
    ExprNode* Node = NewExprNode ();

    /* Set the values */
    Node->Op 	= EXPR_ULABEL;
    Node->V.Val	= Num;

    /* Return the new node */
    return Node;
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



ExprNode* ForceWordExpr (ExprNode* Expr)
/* Force the given expression into a word and return the result. */
{
    /* And the expression by $FFFF to force it into word size */
    ExprNode* Root = NewExprNode ();
    Root->Left  = Expr;
    Root->Op    = EXPR_AND;
    Root->Right	= LiteralExpr (0xFFFF);

    /* Return the result */
    return Root;
}



int IsConstExpr (ExprNode* Root)
/* Return true if the given expression is a constant expression, that is, one
 * with no references to external symbols.
 */
{
    int Const;
    SymEntry* Sym;

    if (EXPR_IS_LEAF (Root->Op)) {
	switch (Root->Op) {

	    case EXPR_LITERAL:
	      	return 1;

	    case EXPR_SYMBOL:
	 	Sym = Root->V.Sym;
	       	if (SymHasUserMark (Sym)) {
		    if (Verbosity > 0) {
		      	DumpExpr (Root);
		    }
		    PError (GetSymPos (Sym), ERR_CIRCULAR_REFERENCE);
		    Const = 0;
		} else {
		    SymMarkUser (Sym);
		    Const = SymIsConst (Sym);
		    SymUnmarkUser (Sym);
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



static void CheckByteExpr (const ExprNode* N, int* IsByte)
/* Internal routine that is recursively called to check if there is a zeropage
 * symbol in the expression tree.
 */
{
    if (N) {
	switch (N->Op & EXPR_TYPEMASK) {

    	    case EXPR_LEAFNODE:
		switch (N->Op) {

    		    case EXPR_SYMBOL:
    			if (SymIsZP (N->V.Sym)) {
    			    *IsByte = 1;
    		       	} else if (SymHasExpr (N->V.Sym)) {
			    /* Check if this expression is a byte expression */
			    *IsByte = IsByteExpr (GetSymExpr (N->V.Sym));
			}
			break;

		    case EXPR_SEGMENT:
			if (GetSegType (N->V.SegNum) == SEGTYPE_ZP) {
			    *IsByte = 1;
			}
			break;

		}
    		break;

    	    case EXPR_UNARYNODE:
    		CheckByteExpr (N->Left, IsByte);
    		break;

    	    case EXPR_BINARYNODE:
    		CheckByteExpr (N->Left, IsByte);
    		CheckByteExpr (N->Right, IsByte);
    		break;

    	    default:
    		Internal ("Unknown expression op: %02X", N->Op);
    	}
    }
}



int IsByteExpr (ExprNode* Root)
/* Return true if this is a byte expression */
{
    int IsByte;

    if (IsConstExpr (Root)) {
    	if (Root->Op != EXPR_LITERAL) {
    	    SimplifyExpr (Root);
     	}
       	return IsByteRange (GetExprVal (Root));
    } else if (Root->Op == EXPR_BYTE0 || Root->Op == EXPR_BYTE1 ||
	       Root->Op == EXPR_BYTE2 || Root->Op == EXPR_BYTE3) {
    	/* Symbol forced to have byte range */
       	IsByte = 1;
    } else {
    	/* We have undefined symbols in the expression. Assume that the
    	 * expression is a byte expression if there is at least one symbol
    	 * declared as zeropage in it. Being wrong here is not a very big
    	 * problem since the linker knows about all symbols and detects
    	 * error like mixing absolute and zeropage labels.
    	 */
    	IsByte = 0;
    	CheckByteExpr (Root, &IsByte);
    }
    return IsByte;
}



long GetExprVal (ExprNode* Expr)
/* Get the value of a constant expression */
{
    long Right, Left;

    switch (Expr->Op) {

       	case EXPR_LITERAL:
    	    return Expr->V.Val;

       	case EXPR_SYMBOL:
    	    return GetSymVal (Expr->V.Sym);

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
		Error (ERR_DIV_BY_ZERO);
	    	return 0;
	    }
	    return Left / Right;

       	case EXPR_MOD:
     	    Left  = GetExprVal (Expr->Left);
	    Right = GetExprVal (Expr->Right);
	    if (Right == 0) {
		Error (ERR_MOD_BY_ZERO);
	    	return 0;
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

       	case EXPR_BYTE0:
	    return GetExprVal (Expr->Left) & 0xFF;

       	case EXPR_BYTE1:
	    return (GetExprVal (Expr->Left) >> 8) & 0xFF;

       	case EXPR_BYTE2:
	    return (GetExprVal (Expr->Left) >> 16) & 0xFF;

       	case EXPR_BYTE3:
	    return (GetExprVal (Expr->Left) >> 24) & 0xFF;

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

	case EXPR_ULABEL:
	    Internal ("GetExprVal called for EXPR_ULABEL");
      	    /* NOTREACHED */
	    return 0;

        default:
	    Internal ("Unknown Op type: %u", Expr->Op);
      	    /* NOTREACHED */
    	    return 0;
    }
}



static ExprNode* RemoveSyms (ExprNode* Expr, int MustClone)
/* Remove resolved symbols from the tree by cloning symbol expressions */
{
    /* Accept NULL pointers */
    if (Expr == 0) {
     	return 0;
    }

    /* Special node handling */
    switch (Expr->Op) {

 	case EXPR_SYMBOL:
	    if (SymHasExpr (Expr->V.Sym)) {
		/* The symbol has an expression tree */
		SymEntry* Sym = Expr->V.Sym;
		if (SymHasUserMark (Sym)) {
		    /* Circular definition */
		    if (Verbosity) {
			DumpExpr (Expr);
		    }
		    PError (GetSymPos (Sym), ERR_CIRCULAR_REFERENCE);
		    return LiteralExpr (0);		/* Return a dummy value */
		}
		SymMarkUser (Sym);
		Expr = RemoveSyms (GetSymExpr (Sym), 1);
		SymUnmarkUser (Sym);
		return Expr;
	    } else if (SymIsConst (Expr->V.Sym)) {
		/* The symbol is a constant */
		return LiteralExpr (GetSymVal (Expr->V.Sym));
	    }
	    break;

	case EXPR_ULABEL:
	    if (ULabCanResolve ()) {
		ExprNode* NewExpr = ULabResolve (Expr->V.Val);
		FreeExpr (Expr);
		Expr = NewExpr;
	    }
	    break;

    }

    /* Clone the current node if needed */
    if (MustClone) {

     	/* Create a new node */
       	ExprNode* Clone = NewExprNode ();

     	/* Clone the operation */
     	Clone->Op = Expr->Op;

	/* Clone the attribute if needed */
	switch (Expr->Op) {

     	    case EXPR_LITERAL:
	    case EXPR_ULABEL:
	   	Clone->V.Val = Expr->V.Val;
	   	break;

	    case EXPR_SYMBOL:
     	   	Clone->V.Sym = Expr->V.Sym;
	   	break;

	    case EXPR_SEGMENT:
	   	Clone->V.SegNum = Expr->V.SegNum;
	   	break;

      	}

	/* Clone the tree nodes */
       	Clone->Left = RemoveSyms (Expr->Left, MustClone);
	Clone->Right = RemoveSyms (Expr->Right, MustClone);

	/* Done */
	return Clone;

    } else {

 	/* Nothing to clone */
     	Expr->Left = RemoveSyms (Expr->Left, MustClone);
     	Expr->Right = RemoveSyms (Expr->Right, MustClone);

     	/* Done */
     	return Expr;

    }
}



static ExprNode* ConstExtract (ExprNode* Expr, long* Val, int Sign)
/* Extract and evaluate all constant factors in an subtree that has only
 * additions and subtractions.
 */
{
    if (Expr->Op == EXPR_LITERAL) {
     	if (Sign < 0) {
     	    *Val -= Expr->V.Val;
     	} else {
     	    *Val += Expr->V.Val;
     	}
       	FreeExprNode (Expr);
     	return 0;
    }

    if (Expr->Op == EXPR_PLUS || Expr->Op == EXPR_MINUS) {
     	ExprNode* Left;
     	ExprNode* Right;
     	Left = ConstExtract (Expr->Left, Val, Sign);
     	if (Expr->Op == EXPR_MINUS) {
     	    Sign = -Sign;
     	}
     	Right = ConstExtract (Expr->Right, Val, Sign);
     	if (Left == 0 && Right == 0) {
     	    FreeExprNode (Expr);
     	    return 0;
     	} else if (Left == 0) {
     	    FreeExprNode (Expr);
     	    return Right;
     	} else if (Right == 0) {
     	    FreeExprNode (Expr);
     	    return Left;
     	} else {
     	    /* Check for SEG - SEG which is now possible */
     	    if (Left->Op == EXPR_SEGMENT && Right->Op == EXPR_SEGMENT &&
	 	Left->V.SegNum == Right->V.SegNum) {
     	     	/* SEG - SEG, remove it completely */
     	       	FreeExprNode (Left);
     	 	FreeExprNode (Right);
     	 	FreeExprNode (Expr);
     	     	return 0;
     	    } else {
		Expr->Left  = Left;
		Expr->Right = Right;
     	       	return Expr;
     	    }
     	}
    }

    /* Some other sort of node, finalize the terms */
    if (Expr->Left) {
	Expr->Left = FinalizeExpr (Expr->Left);
    }
    if (Expr->Right) {
	Expr->Right = FinalizeExpr (Expr->Right);
    }

    return Expr;
}



ExprNode* FinalizeExpr (ExprNode* Expr)
/* Resolve any symbols by cloning the symbol expression tree instead of the
 * symbol reference, then try to simplify the expression as much as possible.
 * This function must only be called if all symbols are resolved (no undefined
 * symbol errors).
 */
{
    long Val = 0;
    ExprNode* N;

    Expr = RemoveSyms (Expr, 0);
    Expr = ConstExtract (Expr, &Val, 1);
    if (Expr == 0) {
     	/* Reduced to a literal value */
	Expr = LiteralExpr (Val);
    } else if (Val) {
     	/* Extracted a value */
     	N = NewExprNode ();
     	N->Op = EXPR_PLUS;
     	N->Left = Expr;
     	N->Right = LiteralExpr (Val);
     	Expr = N;
    }
    return Expr;
}



ExprNode* CloneExpr (ExprNode* Expr)
/* Clone the given expression tree. The function will simply clone symbol
 * nodes, it will not resolve them.
 */
{
    ExprNode* Clone;

    /* Accept NULL pointers */
    if (Expr == 0) {
        return 0;
    }

    /* Get a new node */
    Clone = NewExprNode ();

    /* Clone the operation */
    Clone->Op = Expr->Op;

    /* Clone the attribute if needed */
    switch (Expr->Op) {

	case EXPR_LITERAL:
	case EXPR_ULABEL:
	    Clone->V.Val = Expr->V.Val;
	    break;

	case EXPR_SYMBOL:
	    Clone->V.Sym = Expr->V.Sym;
	    break;

	case EXPR_SEGMENT:
	    Clone->V.SegNum = Expr->V.SegNum;
	    break;

    }

    /* Clone the tree nodes */
    Clone->Left = CloneExpr (Expr->Left);
    Clone->Right = CloneExpr (Expr->Right);

    /* Done */
    return Clone;
}



void WriteExpr (ExprNode* Expr)
/* Write the given expression to the object file */
{
    /* Null expressions are encoded by a type byte of zero */
    if (Expr == 0) {
	ObjWrite8 (0);
      	return;
    }

    /* Write the expression code */
    ObjWrite8 (Expr->Op);

    /* If the is a leafnode, write the expression attribute, otherwise
     * write the expression operands.
     */
    switch (Expr->Op) {

        case EXPR_LITERAL:
	    ObjWrite32 (Expr->V.Val);
	    break;

        case EXPR_SYMBOL:
	    /* Maybe we should use a code here? */
	    CHECK (SymIsImport (Expr->V.Sym));	/* Safety */
	    ObjWrite16 (GetSymIndex (Expr->V.Sym));
	    break;

        case EXPR_SEGMENT:
	    ObjWrite8 (Expr->V.SegNum);
	    break;

	case EXPR_ULABEL:
	    Internal ("WriteExpr: Cannot write EXPR_ULABEL nodes");
	    break;

        default:
	    /* Not a leaf node */
	    WriteExpr (Expr->Left);
	    WriteExpr (Expr->Right);
	    break;

    }
}



