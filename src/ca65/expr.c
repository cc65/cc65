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
static unsigned	   	FreeNodeCount = 0;

/* Structure for parsing expression trees */
typedef struct ExprDesc ExprDesc;
struct ExprDesc {
    long        Val;		/* The offset value */
    long        Left;           /* Left value for StudyBinaryExpr */
    int	       	TooComplex;     /* Expression is too complex to evaluate */
    long        SymCount;       /* Symbol reference count */
    long        SecCount;       /* Section reference count */
    SymEntry*   SymRef;         /* Symbol reference if any */
    unsigned    SecRef;		/* Section reference if any */
};




/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void StudyExpr (ExprNode* Expr, ExprDesc* D, int Sign);
/* Study an expression tree and place the contents into D */



/*****************************************************************************/
/*	    			    Helpers				     */
/*****************************************************************************/



static ExprDesc* InitExprDesc (ExprDesc* ED)
/* Initialize an ExprDesc structure for use with StudyExpr */
{
    ED->Val  	   = 0;
    ED->TooComplex = 0;
    ED->SymCount   = 0;
    ED->SecCount   = 0;
    return ED;
}



static int ExprDescIsConst (const ExprDesc* ED)
/* Return true if the expression is constant */
{
    return (ED->TooComplex == 0 && ED->SymCount == 0 && ED->SecCount == 0);
}



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
    return (Val & ~0xFFFF) == 0;
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
}



static int FuncConst (void)
/* Handle the .CONST builtin function */
{
    /* Read an expression */
    ExprNode* Expr = Expression ();

    /* Check the constness of the expression */
    int Result = IsConstExpr (Expr, 0);

    /* Free the expression */
    FreeExpr (Expr);

    /* Done */
    return Result;
}



static int FuncDefined (void)
/* Handle the .DEFINED builtin function */
{
    /* Parse the symbol name and search for the symbol */
    SymEntry* Sym = ParseScopedSymName (SYM_FIND_EXISTING);

    /* Check if the symbol is defined */
    return (Sym != 0 && SymIsDef (Sym));
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
    /* Parse the symbol name and search for the symbol */
    SymEntry* Sym = ParseScopedSymName (SYM_FIND_EXISTING);

    /* Check if the symbol is referenced */
    return (Sym != 0 && SymIsRef (Sym));
}



static int FuncStrAt (void)
/* Handle the .STRAT function */
{
    char Str [sizeof(SVal)];
    long Index;

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

     	Error ("String constant expected");
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
     	Error ("'(' expected");
     	SkipUntilSep ();
	return GenLiteralExpr (0);
    }
    NextTok ();

    /* Call the function itself */
    Result = F ();

    /* Closing brace must follow */
    ConsumeRParen ();

    /* Return an expression node with the boolean code */
    return GenLiteralExpr (Result);
}



static ExprNode* Factor (void)
{
    ExprNode* L;
    ExprNode* N;
    SymEntry* S;
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
	    /* Search for the symbol */
	    S = ParseScopedSymName (SYM_ALLOC_NEW);
	    if (S == 0) {
		/* Some weird error happened before */
		N = GenLiteralExpr (0);
	    } else {
		/* Mark the symbol as referenced */
		SymRef (S);
                /* Remove the symbol if possible */
                if (SymHasExpr (S)) {
                    N = CloneExpr (GetSymExpr (S));
                } else {
                    /* Create symbol node */
                    N = GenSymExpr (S);
                }
	    }
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



static void StudyExpr (ExprNode* Expr, ExprDesc* D, int Sign)
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



ExprNode* Expression (void)
/* Evaluate an expression, build the expression tree on the heap and return
 * a pointer to the root of the tree.
 */
{
#if 1
    return SimplifyExpr (Expr0 ());
#else
    /* Test code */
    ExprNode* Expr = Expr0 ();
    printf ("Before: "); DumpExpr (Expr, SymResolve);
    Expr = SimplifyExpr (Expr);
    printf ("After:  "); DumpExpr (Expr, SymResolve);
    return Expr;
#endif
}



long ConstExpression (void)
/* Parse an expression. Check if the expression is const, and print an error
 * message if not. Return the value of the expression, or a dummy, if it is
 * not constant.
 */
{
#if 1
    /* Read the expression */
    ExprNode* Expr = Expr0 ();
#else
    /* Test code */
    ExprNode* Expr = Expression ();
#endif

    /* Study the expression */
    ExprDesc D;
    InitExprDesc (&D);
    StudyExpr (Expr, &D, 1);

    /* Check if the expression is constant */
    if (!ExprDescIsConst (&D)) {
     	Error ("Constant expression expected");
     	D.Val = 0;
    }

    /* Free the expression tree and return the value */
    FreeExpr (Expr);
    return D.Val;
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



ExprNode* SimplifyExpr (ExprNode* Expr)
/* Try to simplify the given expression tree */
{
    if (Expr && Expr->Op != EXPR_LITERAL) {

        /* Create an expression description and initialize it */
        ExprDesc D;
        InitExprDesc (&D);

        /* Study the expression */
        StudyExpr (Expr, &D, 1);

        /* Now check if we can generate a literal value */
        if (ExprDescIsConst (&D)) {
            /* No external references */
            FreeExpr (Expr);
            Expr = GenLiteralExpr (D.Val);
        }
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
    ExprNode* Root = NewExprNode (EXPR_PLUS);
    Root->Left = Left;
    Root->Right = Right;
    return Root;
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
    /* Study the expression */
    ExprDesc D;
    InitExprDesc (&D);
    StudyExpr (Expr, &D, 1);

    /* Check if the expression is constant */
    if (ExprDescIsConst (&D)) {
        if (Val) {
            *Val = D.Val;
        }
        return 1;
    } else {
        return 0;
    }
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
                    case EXPR_FORCEWORD:
                        /* No need to look at the expression */
                        *AddrSize = ADDR_SIZE_ABS;
                        break;

                    case EXPR_FORCEFAR:
                        /* No need to look at the expression */
                        *AddrSize = ADDR_SIZE_FAR;
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




