/*****************************************************************************/
/*                                                                           */
/*				    expr.c				     */
/*                                                                           */
/*	       Expression evaluation for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#include <string.h>
#include <time.h>

/* common */
#include "check.h"
#include "cpu.h"
#include "exprdefs.h"
#include "print.h"
#include "shift.h"
#include "strbuf.h"
#include "tgttrans.h"
#include "version.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "global.h"
#include "instr.h"
#include "nexttok.h"
#include "objfile.h"
#include "segment.h"
#include "sizeof.h"
#include "studyexpr.h"
#include "symbol.h"
#include "symtab.h"
#include "toklist.h"
#include "ulabel.h"



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
static ExprNode*   	FreeExprNodes = 0;
static unsigned	      	FreeNodeCount = 0;



/*****************************************************************************/
/*	    	      		    Helpers				     */
/*****************************************************************************/



static ExprNode* NewExprNode (unsigned Op)
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
    N->Op = Op;
    N->Left = N->Right = 0;
    N->Obj = 0;

    return N;
}



static void FreeExprNode (ExprNode* E)
/* Free a node */
{
    if (E) {
        if (E->Op == EXPR_SYMBOL) {
            /* Remove the symbol reference */
            SymDelExprRef (E->V.Sym, E);
        }
        /* Place the symbol into the free nodes list if possible */
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
    return (Val & ~0xFFFFL) == 0;
}



int IsFarRange (long Val)
/* Return true if this is a far (24 bit) value */
{
    return (Val & ~0xFFFFFFL) == 0;
}



static int IsEasyConst (const ExprNode* E, long* Val)
/* Do some light checking if the given node is a constant. Don't care if E is
 * a complex expression. If E is a constant, return true and place its value
 * into Val, provided that Val is not NULL.
 */
{
    /* Resolve symbols, follow symbol chains */
    while (E->Op == EXPR_SYMBOL) {
        E = SymResolve (E->V.Sym);
        if (E == 0) {
            /* Could not resolve */
            return 0;
        }
    }

    /* Symbols resolved, check for a literal */
    if (E->Op == EXPR_LITERAL) {
        if (Val) {
            *Val = E->V.Val;
        }
        return 1;
    }

    /* Not found to be a const according to our tests */
    return 0;
}



static ExprNode* Symbol (SymEntry* S)
/* Reference a symbol and return an expression for it */
{
    if (S == 0) {
        /* Some weird error happened before */
        return GenLiteralExpr (0);
    } else {
        /* Mark the symbol as referenced */
        SymRef (S);
        /* Create symbol node */
        return GenSymExpr (S);
    }
}



static ExprNode* FuncBlank (void)
/* Handle the .BLANK builtin function */
{
    int Result = 1;

    /* Assume no tokens if the closing brace follows (this is not correct in
     * all cases, since the token may be the closing brace, but this will
     * give a syntax error anyway and may not be handled by .BLANK.
     */
    if (Tok != TOK_RPAREN) {
	/* Skip any tokens */
	int Braces = 0;
	while (!TokIsSep (Tok)) {
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
    return GenLiteralExpr (Result);
}



static ExprNode* FuncConst (void)
/* Handle the .CONST builtin function */
{
    /* Read an expression */
    ExprNode* Expr = Expression ();

    /* Check the constness of the expression */
    ExprNode* Result = GenLiteralExpr (IsConstExpr (Expr, 0));

    /* Free the expression */
    FreeExpr (Expr);

    /* Done */
    return Result;
}



static ExprNode* FuncDefined (void)
/* Handle the .DEFINED builtin function */
{
    /* Parse the symbol name and search for the symbol */
    SymEntry* Sym = ParseScopedSymName (SYM_FIND_EXISTING);

    /* Check if the symbol is defined */
    return GenLiteralExpr (Sym != 0 && SymIsDef (Sym));
}



static ExprNode* DoMatch (enum TC EqualityLevel)
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
    	if (TokIsSep (Tok)) {
    	    Error ("Unexpected end of line");
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
    	if (TokIsSep (Tok)) {
    	    Error ("Unexpected end of line");
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
    return GenLiteralExpr (Result);
}



static ExprNode* FuncMatch (void)
/* Handle the .MATCH function */
{
    return DoMatch (tcSameToken);
}



static ExprNode* FuncReferenced (void)
/* Handle the .REFERENCED builtin function */
{
    /* Parse the symbol name and search for the symbol */
    SymEntry* Sym = ParseScopedSymName (SYM_FIND_EXISTING);

    /* Check if the symbol is referenced */
    return GenLiteralExpr (Sym != 0 && SymIsRef (Sym));
}



static ExprNode* FuncSizeOf (void)
/* Handle the .SIZEOF function */
{
    StrBuf    FullName = AUTO_STRBUF_INITIALIZER;
    char      Name[sizeof (SVal)];
    SymTable* Scope;
    SymEntry* Sym;
    SymEntry* SizeSym;
    long      Size;


    if (Tok == TOK_LOCAL_IDENT) {

        /* Cheap local symbol, special handling */
        Sym = SymFindLocal (SymLast, SVal, SYM_FIND_EXISTING);
        if (Sym == 0) {
            Error ("Unknown symbol or scope: `%s'", SB_GetConstBuf (&FullName));
            return GenLiteralExpr (0);
        } else {
            SizeSym = GetSizeOfSymbol (Sym);
        }

    } else {

        /* Parse the scope and the name */
        SymTable* ParentScope = ParseScopedIdent (Name, &FullName);
    
        /* Check if the parent scope is valid */
        if (ParentScope == 0) {
            /* No such scope */
            DoneStrBuf (&FullName);
            return GenLiteralExpr (0);
        }
    
        /* The scope is valid, search first for a child scope, then for a symbol */
        if ((Scope = SymFindScope (ParentScope, Name, SYM_FIND_EXISTING)) != 0) {
            /* Yep, it's a scope */
            SizeSym = GetSizeOfScope (Scope);
        } else if ((Sym = SymFind (ParentScope, Name, SYM_FIND_EXISTING)) != 0) {
            SizeSym = GetSizeOfSymbol (Sym);
        } else {
            Error ("Unknown symbol or scope: `%s'", SB_GetConstBuf (&FullName));
            return GenLiteralExpr (0);
        }
    }

    /* Check if we have a size */
    if (SizeSym == 0 || !SymIsConst (SizeSym, &Size)) {
        Error ("Size of `%s' is unknown", SB_GetConstBuf (&FullName));
        return GenLiteralExpr (0);
    }

    /* Return the size */
    return GenLiteralExpr (Size);
}



static ExprNode* FuncStrAt (void)
/* Handle the .STRAT function */
{
    char Str [sizeof(SVal)];
    long Index;
    unsigned char C;

    /* String constant expected */
    if (Tok != TOK_STRCON) {
      	Error ("String constant expected");
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
	Error ("Range error");
	return 0;
    }

    /* Get the char, handle as unsigned. Be sure to translate it into
     * the target character set.
     */
    C = TgtTranslateChar (Str [(size_t)Index]);

    /* Return the char expression */
    return GenLiteralExpr (C);
}



static ExprNode* FuncStrLen (void)
/* Handle the .STRLEN function */
{
    int Len;

    /* String constant expected */
    if (Tok != TOK_STRCON) {

     	Error ("String constant expected");
    	/* Smart error recovery */
     	if (Tok != TOK_RPAREN) {
     	    NextTok ();
     	}
       	Len = 0;

    } else {

        /* Get the length of the string */
     	Len = strlen (SVal);

	/* Skip the string */
	NextTok ();
    }

    /* Return the length */
    return GenLiteralExpr (Len);
}



static ExprNode* FuncTCount (void)
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
     	if (TokIsSep (Tok)) {
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
    return GenLiteralExpr (Count);
}



static ExprNode* FuncXMatch (void)
/* Handle the .XMATCH function */
{
    return DoMatch (tcIdentical);
}



static ExprNode* Function (ExprNode* (*F) (void))
/* Handle builtin functions */
{
    ExprNode* E;

    /* Skip the keyword */
    NextTok ();

    /* Expression must be enclosed in braces */
    if (Tok != TOK_LPAREN) {
     	Error ("'(' expected");
     	SkipUntilSep ();
	return GenLiteralExpr (0);
    }
    NextTok ();

    /* Call the function itself */
    E = F ();

    /* Closing brace must follow */
    ConsumeRParen ();

    /* Return the result of the actual function */
    return E;
}



static ExprNode* Factor (void)
{
    ExprNode* L;
    ExprNode* N;
    long      Val;

    switch (Tok) {

	case TOK_INTCON:
    	    N = GenLiteralExpr (IVal);
       	    NextTok ();
	    break;

	case TOK_CHARCON:
    	    N = GenLiteralExpr (TgtTranslateChar (IVal));
       	    NextTok ();
	    break;

	case TOK_NAMESPACE:
	case TOK_IDENT:
            N = Symbol (ParseScopedSymName (SYM_ALLOC_NEW));
	    break;

        case TOK_LOCAL_IDENT:
            N = Symbol (SymFindLocal (SymLast, SVal, SYM_ALLOC_NEW));
            NextTok ();
            break;

	case TOK_ULABEL:
	    N = ULabRef (IVal);
	    NextTok ();
	    break;

	case TOK_MINUS:
	    NextTok ();
            L = Factor ();
            if (IsEasyConst (L, &Val)) {
                FreeExpr (L);
                N = GenLiteralExpr (-Val);
            } else {
                N = NewExprNode (EXPR_UNARY_MINUS);
       	        N->Left = L;
            }
     	    break;

     	case TOK_NOT:
     	    NextTok ();
            L = Factor ();
            if (IsEasyConst (L, &Val)) {
                FreeExpr (L);
                N = GenLiteralExpr (~Val);
            } else {
                N = NewExprNode (EXPR_NOT);
                N->Left = L;
            }
     	    break;

     	case TOK_STAR:
	case TOK_PC:
     	    NextTok ();
       	    N = GenCurrentPC ();
	    break;

	case TOK_LT:
	    NextTok ();
            L = Factor ();
            if (IsEasyConst (L, &Val)) {
                FreeExpr (L);
                N = GenLiteralExpr (Val & 0xFF);
            } else {
                N = NewExprNode (EXPR_BYTE0);
                N->Left = L;
            }
	    break;

	case TOK_GT:
	    NextTok ();
            L = Factor ();
            if (IsEasyConst (L, &Val)) {
                FreeExpr (L);
                N = GenLiteralExpr ((Val >> 8) & 0xFF);
            } else {
                N = NewExprNode (EXPR_BYTE1);
                N->Left = L;
            }
	    break;

        case TOK_BANK:
            NextTok ();
            L = Factor ();
            if (IsEasyConst (L, &Val)) {
                FreeExpr (L);
                N = GenLiteralExpr ((Val >> 16) & 0xFF);
            } else {
                N = NewExprNode (EXPR_BYTE2);
                N->Left = L;
            }
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
	    N = GenLiteralExpr (CPUIsets[CPU]);
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

        case TOK_SIZEOF:
            N = Function (FuncSizeOf);
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

	case TOK_TIME:
	    N = GenLiteralExpr (time (0));
	    NextTok ();
	    break;

        case TOK_VERSION:
            N = GenLiteralExpr (VERSION);
            NextTok ();
            break;

	case TOK_XMATCH:
	    N = Function (FuncXMatch);
	    break;

	default:
	    if (LooseCharTerm && Tok == TOK_STRCON && strlen(SVal) == 1) {
		/* A character constant */
		N = GenLiteralExpr (TgtTranslateChar (SVal[0]));
	    } else {
		N = GenLiteralExpr (0);	/* Dummy */
		Error ("Syntax error");
	    }
	    NextTok ();
	    break;
    }
    return N;
}



static ExprNode* Term (void)
{
    /* Read left hand side */
    ExprNode* Root = Factor ();

    /* Handle multiplicative operations */
    while (Tok == TOK_MUL || Tok == TOK_DIV || Tok == TOK_MOD ||
	   Tok == TOK_AND || Tok == TOK_XOR || Tok == TOK_SHL ||
	   Tok == TOK_SHR) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        enum Token T = Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = Factor ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_MUL:
                    Val = LVal * RVal;
                    break;

                case TOK_DIV:
                    if (RVal == 0) {
                        Error ("Division by zero");
                        Val = 1;
                    } else {
                        Val = LVal / RVal;
                    }
                    break;

                case TOK_MOD:
                    if (RVal == 0) {
                        Error ("Modulo operation with zero");
                        Val = 1;
                    } else {
                        Val = LVal % RVal;
                    }
                    break;

                case TOK_AND:
                    Val = LVal & RVal;
                    break;

                case TOK_XOR:
                    Val = LVal ^ RVal;
                    break;

                case TOK_SHL:
                    Val = shl_l (LVal, RVal);
                    break;

                case TOK_SHR:
                    Val = shr_l (LVal, RVal);
                    break;

                default:
                    Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
             * right sides.
             */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_MUL:   Op = EXPR_MUL;	break;
                case TOK_DIV:   Op = EXPR_DIV;  break;
                case TOK_MOD:   Op = EXPR_MOD;  break;
                case TOK_AND:   Op = EXPR_AND;  break;
                case TOK_XOR:   Op = EXPR_XOR;  break;
                case TOK_SHL:   Op = EXPR_SHL;  break;
                case TOK_SHR:   Op = EXPR_SHR;  break;
                default:       	Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }

    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* SimpleExpr (void)
{
    /* Read left hand side */
    ExprNode* Root = Term ();

    /* Handle additive operations */
    while (Tok == TOK_PLUS || Tok == TOK_MINUS || Tok == TOK_OR) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        enum Token T = Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = Term ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_PLUS:  Val = LVal + RVal;      break;
                case TOK_MINUS: Val = LVal - RVal;      break;
                case TOK_OR:    Val = LVal | RVal;      break;
                default:        Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
             * right sides.
             */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_PLUS:  Op = EXPR_PLUS;  break;
                case TOK_MINUS: Op = EXPR_MINUS; break;
                case TOK_OR:    Op = EXPR_OR;    break;
                default:       	Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }
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

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        enum Token T = Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = SimpleExpr ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_EQ:    Val = (LVal == RVal);   break;
                case TOK_NE:    Val = (LVal != RVal);   break;
                case TOK_LT:    Val = (LVal < RVal);    break;
                case TOK_GT:    Val = (LVal > RVal);    break;
                case TOK_LE:    Val = (LVal <= RVal);   break;
                case TOK_GE:    Val = (LVal >= RVal);   break;
                default:        Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
             * right sides.
             */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_EQ:    Op = EXPR_EQ;   break;
                case TOK_NE:    Op = EXPR_NE;   break;
                case TOK_LT:    Op = EXPR_LT;   break;
                case TOK_GT:    Op = EXPR_GT;   break;
                case TOK_LE:    Op = EXPR_LE;   break;
                case TOK_GE:    Op = EXPR_GE;   break;
                default:       	Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }
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
    while (Tok == TOK_BOOLAND || Tok == TOK_BOOLXOR) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        enum Token T = Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = BoolExpr ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_BOOLAND:   Val = ((LVal != 0) && (RVal != 0)); break;
                case TOK_BOOLXOR:   Val = ((LVal != 0) ^  (RVal != 0)); break;
                default:        Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
             * right sides.
             */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_BOOLAND:   Op = EXPR_BOOLAND; break;
                case TOK_BOOLXOR:   Op = EXPR_BOOLXOR; break;
                default:       	    Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }
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
    while (Tok == TOK_BOOLOR) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        enum Token T = Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = Expr2 ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_BOOLOR:    Val = ((LVal != 0) || (RVal != 0)); break;
                default:        Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
             * right sides.
             */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_BOOLOR:    Op = EXPR_BOOLOR;  break;
                default:       	    Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }
    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* Expr0 (void)
/* Boolean operators: NOT */
{
    ExprNode* Root;

    /* Handle booleans */
    if (Tok == TOK_BOOLNOT) {

        long Val;
        ExprNode* Left;

        /* Skip the operator token */
     	NextTok ();

        /* Read the argument */
        Left = Expr0 ();

        /* If the argument is const, evaluate it directly */
        if (IsEasyConst (Left, &Val)) {
            FreeExpr (Left);
            Root = GenLiteralExpr (!Val);
        } else {
            Root = NewExprNode (EXPR_BOOLNOT);
            Root->Left = Left;
        }

    } else {

     	/* Read left hand side */
     	Root = Expr1 ();

    }

    /* Return the expression tree we've created */
    return Root;
}



ExprNode* Expression (void)
/* Evaluate an expression, build the expression tree on the heap and return
 * a pointer to the root of the tree.
 */
{
    return Expr0 ();
}



long ConstExpression (void)
/* Parse an expression. Check if the expression is const, and print an error
 * message if not. Return the value of the expression, or a dummy, if it is
 * not constant.
 */
{
    long Val;

    /* Read the expression */
    ExprNode* Expr = Expression ();

    /* Study the expression */
    ExprDesc D;
    ED_Init (&D);
    StudyExpr (Expr, &D);

    /* Check if the expression is constant */
    if (ED_IsConst (&D)) {
        Val = D.Val;
    } else {
     	Error ("Constant expression expected");
     	Val = 0;
    }

    /* Free the expression tree and allocated memory for D */
    FreeExpr (Expr);
    ED_Done (&D);

    /* Return the value */
    return Val;
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



ExprNode* SimplifyExpr (ExprNode* Expr, const ExprDesc* D)
/* Try to simplify the given expression tree */
{
    if (Expr->Op != EXPR_LITERAL && ED_IsConst (D)) {
        /* No external references */
        FreeExpr (Expr);
        Expr = GenLiteralExpr (D->Val);
    }
    return Expr;
}



ExprNode* GenLiteralExpr (long Val)
/* Return an expression tree that encodes the given literal value */
{
    ExprNode* Expr = NewExprNode (EXPR_LITERAL);
    Expr->V.Val = Val;
    return Expr;
}



ExprNode* GenSymExpr (SymEntry* Sym)
/* Return an expression node that encodes the given symbol */
{
    ExprNode* Expr = NewExprNode (EXPR_SYMBOL);
    Expr->V.Sym = Sym;
    SymAddExprRef (Sym, Expr);
    return Expr;
}



static ExprNode* GenSectionExpr (unsigned SegNum)
/* Return an expression node for the given section */
{
    ExprNode* Expr = NewExprNode (EXPR_SECTION);
    Expr->V.SegNum = SegNum;
    return Expr;
}



ExprNode* GenAddExpr (ExprNode* Left, ExprNode* Right)
/* Generate an addition from the two operands */
{
    long Val;
    if (IsEasyConst (Left, &Val) && Val == 0) {
        FreeExpr (Left);
        return Right;
    } else if (IsEasyConst (Right, &Val) && Val == 0) {
        FreeExpr (Right);
        return Left;
    } else {
        ExprNode* Root = NewExprNode (EXPR_PLUS);
        Root->Left = Left;
        Root->Right = Right;
        return Root;
    }
}



ExprNode* GenCurrentPC (void)
/* Return the current program counter as expression */
{
    ExprNode* Root;

    if (RelocMode) {
	/* Create SegmentBase + Offset */
       	Root = GenAddExpr (GenSectionExpr (GetCurrentSegNum ()),
                           GenLiteralExpr (GetPC ()));
    } else {
     	/* Absolute mode, just return PC value */
	Root = GenLiteralExpr (GetPC ());
    }

    return Root;
}



ExprNode* GenSwapExpr (ExprNode* Expr)
/* Return an extended expression with lo and hi bytes swapped */
{
    ExprNode* N = NewExprNode (EXPR_SWAP);
    N->Left = Expr;
    return N;
}



ExprNode* GenBranchExpr (unsigned Offs)
/* Return an expression that encodes the difference between current PC plus
 * offset and the target expression (that is, Expression() - (*+Offs) ).
 */
{
    ExprNode* N;
    ExprNode* Root;
    long      Val;

    /* Read Expression() */
    N = Expression ();

    /* If the expression is a cheap constant, generate a simpler tree */
    if (IsEasyConst (N, &Val)) {

        /* Free the constant expression tree */
        FreeExpr (N);

        /* Generate the final expression:
         * Val - (* + Offs)
         * Val - ((Seg + PC) + Offs)
         * Val - Seg - PC - Offs
         * (Val - PC - Offs) - Seg
         */
        Root = GenLiteralExpr (Val - GetPC () - Offs);
        if (RelocMode) {
            N = Root;
            Root = NewExprNode (EXPR_MINUS);
            Root->Left  = N;
            Root->Right = GenSectionExpr (GetCurrentSegNum ());
        }

    } else {

        /* Generate the expression:
         * N - (* + Offs)
         * N - ((Seg + PC) + Offs)
         * N - Seg - PC - Offs
         * N - (PC + Offs) - Seg
         */
        Root = NewExprNode (EXPR_MINUS);
        Root->Left  = N;
        Root->Right = GenLiteralExpr (GetPC () + Offs);
        if (RelocMode) {
            N = Root;
            Root = NewExprNode (EXPR_MINUS);
            Root->Left  = N;
            Root->Right = GenSectionExpr (GetCurrentSegNum ());
        }
    }

    /* Return the result */
    return Root;
}



ExprNode* GenULabelExpr (unsigned Num)
/* Return an expression for an unnamed label with the given index */
{
    ExprNode* Node = NewExprNode (EXPR_ULABEL);
    Node->V.Val	= Num;

    /* Return the new node */
    return Node;
}



ExprNode* GenByteExpr (ExprNode* Expr)
/* Force the given expression into a byte and return the result */
{
    /* Use the low byte operator to force the expression into byte size */
    ExprNode* Root = NewExprNode (EXPR_BYTE0);
    Root->Left  = Expr;

    /* Return the result */
    return Root;
}



ExprNode* GenWordExpr (ExprNode* Expr)
/* Force the given expression into a word and return the result. */
{
    /* AND the expression by $FFFF to force it into word size */
    ExprNode* Root = NewExprNode (EXPR_AND);
    Root->Left  = Expr;
    Root->Right	= GenLiteralExpr (0xFFFF);

    /* Return the result */
    return Root;
}



ExprNode* GenNE (ExprNode* Expr, long Val)
/* Generate an expression that compares Expr and Val for inequality */
{
    /* Generate a compare node */
    ExprNode* Root = NewExprNode (EXPR_NE);
    Root->Left  = Expr;
    Root->Right	= GenLiteralExpr (Val);

    /* Return the result */
    return Root;
}



int IsConstExpr (ExprNode* Expr, long* Val)
/* Return true if the given expression is a constant expression, that is, one
 * with no references to external symbols. If Val is not NULL and the
 * expression is constant, the constant value is stored here.
 */
{
    int IsConst;

    /* Study the expression */
    ExprDesc D;
    ED_Init (&D);
    StudyExpr (Expr, &D);

    /* Check if the expression is constant */
    IsConst = ED_IsConst (&D);
    if (IsConst && Val != 0) {
        *Val = D.Val;
    }

    /* Delete allocated memory and return the result */
    ED_Done (&D);
    return IsConst;
}



static void CheckAddrSize (const ExprNode* N, unsigned char* AddrSize)
/* Internal routine that is recursively called to check for the address size
 * of the expression tree.
 */
{
    unsigned char A;
    unsigned char Left, Right;

    if (N) {
    	switch (N->Op & EXPR_TYPEMASK) {

    	    case EXPR_LEAFNODE:
	       	switch (N->Op) {

    	       	    case EXPR_SYMBOL:
    	       	    	if (SymIsZP (N->V.Sym)) {
    	       	    	    if (*AddrSize < ADDR_SIZE_ZP) {
                                *AddrSize = ADDR_SIZE_ZP;
                            }
    	       	       	} else if (SymHasExpr (N->V.Sym)) {
	       	    	    /* Check if this expression is a byte expression */
	       	    	    CheckAddrSize (GetSymExpr (N->V.Sym), AddrSize);
	       	    	} else {
                            /* Undefined symbol, use absolute */
                            if (*AddrSize < ADDR_SIZE_ABS) {
                                *AddrSize = ADDR_SIZE_ABS;
                            }
                        }
	       	    	break;

	       	    case EXPR_SECTION:
                        A = GetSegAddrSize (N->V.SegNum);
                        if (A > *AddrSize) {
                            *AddrSize = A;
                        }
	       		break;

	       	}
    	       	break;

    	    case EXPR_UNARYNODE:
                switch (N->Op) {

                    case EXPR_BYTE0:
                    case EXPR_BYTE1:
                    case EXPR_BYTE2:
                    case EXPR_BYTE3:
                        /* No need to look at the expression */
                        *AddrSize = ADDR_SIZE_ZP;
                        break;

                    case EXPR_WORD0:
                    case EXPR_WORD1:
                        /* No need to look at the expression */
                        *AddrSize = ADDR_SIZE_ABS;
                        break;

                    default:
                        CheckAddrSize (N->Left, AddrSize);
                        break;
                }
                break;

    	    case EXPR_BINARYNODE:
                Left = Right = ADDR_SIZE_DEFAULT;
    	       	CheckAddrSize (N->Left, &Left);
    	       	CheckAddrSize (N->Right, &Right);
                A = (Left > Right)? Left : Right;
                if (A > *AddrSize) {
                    *AddrSize = A;
                }
    	       	break;

    	    default:
    	       	Internal ("Unknown expression op: %02X", N->Op);
    	}
    }
}



int IsByteExpr (ExprNode* Root)
/* Return true if this is a byte expression */
{
    long Val;

    if (IsConstExpr (Root, &Val)) {
       	return IsByteRange (Val);
    } else {
        unsigned char AddrSize = ADDR_SIZE_DEFAULT;
        CheckAddrSize (Root, &AddrSize);
        return (AddrSize == ADDR_SIZE_ZP);
    }
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

    /* Clone the node */
    switch (Expr->Op) {

	case EXPR_LITERAL:
            Clone = GenLiteralExpr (Expr->V.Val);
            break;

	case EXPR_ULABEL:
	    Clone = GenULabelExpr (Expr->V.Val);
	    break;

	case EXPR_SYMBOL:
	    Clone = GenSymExpr (Expr->V.Sym);
	    break;

	case EXPR_SECTION:
	    Clone = GenSectionExpr (Expr->V.SegNum);
	    break;

        default:
            /* Generate a new node */
            Clone = NewExprNode (Expr->Op);
            /* Clone the tree nodes */
            Clone->Left = CloneExpr (Expr->Left);
            Clone->Right = CloneExpr (Expr->Right);
            break;
    }

    /* Done */
    return Clone;
}



void WriteExpr (ExprNode* Expr)
/* Write the given expression to the object file */
{
    /* Null expressions are encoded by a type byte of zero */
    if (Expr == 0) {
	ObjWrite8 (EXPR_NULL);
      	return;
    }

    /* If the is a leafnode, write the expression attribute, otherwise
     * write the expression operands.
     */
    switch (Expr->Op) {

        case EXPR_LITERAL:
            ObjWrite8 (EXPR_LITERAL);
	    ObjWrite32 (Expr->V.Val);
	    break;

        case EXPR_SYMBOL:
	    if (SymIsImport (Expr->V.Sym)) {
                ObjWrite8 (EXPR_SYMBOL);
                ObjWriteVar (GetSymIndex (Expr->V.Sym));
            } else {
                WriteExpr (GetSymExpr (Expr->V.Sym));
            }
	    break;

        case EXPR_SECTION:
            ObjWrite8 (EXPR_SECTION);
	    ObjWrite8 (Expr->V.SegNum);
	    break;

	case EXPR_ULABEL:
            WriteExpr (ULabResolve (Expr->V.Val));
	    break;

        default:
	    /* Not a leaf node */
            ObjWrite8 (Expr->Op);
	    WriteExpr (Expr->Left);
	    WriteExpr (Expr->Right);
	    break;

    }
}




