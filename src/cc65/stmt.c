/*****************************************************************************/
/*                                                                           */
/*				    stmt.c                                   */
/*                                                                           */
/*			       Parse a statement                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2001 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "coll.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
#include "asmlabel.h"
#include "codegen.h"
#include "datatype.h"
#include "error.h"
#include "expr.h"
#include "function.h"
#include "global.h"
#include "goto.h"
#include "litpool.h"
#include "locals.h"
#include "loop.h"
#include "pragma.h"
#include "scanner.h"
#include "swstmt.h"
#include "symtab.h"
#include "stmt.h"
#include "typeconv.h"



/*****************************************************************************/
/*			       Helper functions                              */
/*****************************************************************************/



static void CheckTok (token_t Tok, const char* Msg, int* PendingToken)
/* Helper function for Statement. Will check for Tok and print Msg if not
 * found. If PendingToken is NULL, it will the skip the token, otherwise
 * it will store one to PendingToken.
 */
{
    if (CurTok.Tok != Tok) {
   	Error (Msg);
    } else if (PendingToken) {
   	*PendingToken = 1;
    } else {
   	NextToken ();
    }
}



static void CheckSemi (int* PendingToken)
/* Helper function for Statement. Will check for a semicolon and print an
 * error message if not found (plus some error recovery). If PendingToken is
 * NULL, it will the skip the token, otherwise it will store one to
 * PendingToken.
 * This function is a special version of CheckTok with the addition of the
 * error recovery.
 */
{
    int HaveToken = (CurTok.Tok == TOK_SEMI);
    if (!HaveToken) {
   	Error ("`;' expected");
        /* Try to be smart about errors */
        if (CurTok.Tok == TOK_COLON || CurTok.Tok == TOK_COMMA) {
            HaveToken = 1;
        }
    }
    if (HaveToken) {
        if (PendingToken) {
            *PendingToken = 1;
        } else {
            NextToken ();
        }
    }
}



static void SkipPending (int PendingToken)
/* Skip the pending token if we have one */
{
    if (PendingToken) {
	NextToken ();
    }
}



/*****************************************************************************/
/*	  	  		     Code		     		     */
/*****************************************************************************/



static int IfStatement (void)
/* Handle an 'if' statement */
{
    unsigned Label1;
    int GotBreak;

    /* Skip the if */
    NextToken ();

    /* Generate a jump label and parse the condition */
    Label1 = GetLocalLabel ();
    TestInParens (Label1, 0);

    /* Parse the if body */
    GotBreak = Statement (0);

    /* Else clause present? */
    if (CurTok.Tok != TOK_ELSE) {

      	g_defcodelabel (Label1);

     	/* Since there's no else clause, we're not sure, if the a break
     	 * statement is really executed.
     	 */
      	return 0;

    } else {

	/* Generate a jump around the else branch */
     	unsigned Label2 = GetLocalLabel ();
	g_jump (Label2);

	/* Skip the else */
     	NextToken ();

	/* Define the target for the first test */
	g_defcodelabel (Label1);

	/* Total break only if both branches had a break. */
     	GotBreak &= Statement (0);

     	/* Generate the label for the else clause */
	g_defcodelabel (Label2);

     	/* Done */
     	return GotBreak;
    }
}



static void DoStatement (void)
/* Handle the 'do' statement */
{
    /* Get the loop control labels */
    unsigned loop = GetLocalLabel ();
    unsigned lab = GetLocalLabel ();

    /* Skip the while token */
    NextToken ();

    /* Add the loop to the loop stack */
    AddLoop (oursp, loop, lab, 0, 0);

    /* Define the head label */
    g_defcodelabel (loop);

    /* Parse the loop body */
    Statement (0);

    /* Parse the end condition */
    Consume (TOK_WHILE, "`while' expected");
    TestInParens (loop, 1);
    ConsumeSemi ();

    /* Define the break label */
    g_defcodelabel (lab);

    /* Remove the loop from the loop stack */
    DelLoop ();
}



static void WhileStatement (void)
/* Handle the 'while' statement */
{
    int PendingToken;

    /* Get the loop control labels */
    unsigned loop = GetLocalLabel ();
    unsigned lab = GetLocalLabel ();

    /* Skip the while token */
    NextToken ();

    /* Add the loop to the loop stack */
    AddLoop (oursp, loop, lab, 0, 0);

    /* Define the head label */
    g_defcodelabel (loop);

    /* Test the loop condition */
    TestInParens (lab, 0);

    /* Loop body */
    Statement (&PendingToken);

    /* Jump back to loop top */
    g_jump (loop);

    /* Exit label */
    g_defcodelabel (lab);

    /* Eat remaining tokens that were delayed because of line info
     * correctness
     */
    SkipPending (PendingToken);

    /* Remove the loop from the loop stack */
    DelLoop ();
}



static void ReturnStatement (void)
/* Handle the 'return' statement */
{
    ExprDesc Expr;
    int k;

    NextToken ();
    if (CurTok.Tok != TOK_SEMI) {

        /* Check if the function has a return value declared */
       	if (F_HasVoidReturn (CurrentFunc)) {
       	    Error ("Returning a value in function with return type void");
       	}

	/* Evaluate the return expression */
	k = hie0 (InitExprDesc (&Expr));

	/* Ignore the return expression if the function returns void */
    	if (!F_HasVoidReturn (CurrentFunc)) {

	    /* Convert the return value to the type of the function result */
	    TypeConversion (&Expr, k, F_GetReturnType (CurrentFunc));

	    /* Load the value into the primary */
	    exprhs (CF_NONE, k, &Expr);
    	}

    } else if (!F_HasVoidReturn (CurrentFunc) && !F_HasOldStyleIntRet (CurrentFunc)) {
    	Error ("Function `%s' must return a value", F_GetFuncName (CurrentFunc));
    }

    /* Cleanup the stack in case we're inside a block with locals */
    g_space (oursp - F_GetTopLevelSP (CurrentFunc));

    /* Output a jump to the function exit code */
    g_jump (F_GetRetLab (CurrentFunc));
}



static void BreakStatement (void)
/* Handle the 'break' statement */
{
    LoopDesc* L;

    /* Skip the break */
    NextToken ();

    /* Get the current loop descriptor */
    L = CurrentLoop ();

    /* Check if we are inside a loop */
    if (L == 0) {
     	/* Error: No current loop */
     	Error ("`break' statement not within loop or switch");
     	return;
    }

    /* Correct the stack pointer if needed */
    g_space (oursp - L->StackPtr);

    /* Jump to the exit label of the loop */
    g_jump (L->Label);
}



static void ContinueStatement (void)
/* Handle the 'continue' statement */
{
    LoopDesc* L;

    /* Skip the continue */
    NextToken ();

    /* Get the current loop descriptor */
    L = CurrentLoop ();
    if (L) {
     	/* Search for the correct loop */
     	do {
     	    if (L->Loop) {
     		break;
     	    }
     	    L = L->Next;
     	} while (L);
    }

    /* Did we find it? */
    if (L == 0) {
     	Error ("`continue' statement not within a loop");
     	return;
    }

    /* Correct the stackpointer if needed */
    g_space (oursp - L->StackPtr);

    /* Output the loop code */
    if (L->linc) {
       	g_jump (L->linc);
    } else {
       	g_jump (L->Loop);
    }
}



static void ForStatement (void)
/* Handle a 'for' statement */
{
    ExprDesc lval1;
    ExprDesc lval3;
    int HaveIncExpr;
    CodeMark IncExprStart;
    CodeMark IncExprEnd;
    int PendingToken;

    /* Get several local labels needed later */
    unsigned TestLabel = GetLocalLabel ();
    unsigned lab       = GetLocalLabel ();
    unsigned IncLabel  = GetLocalLabel ();
    unsigned lstat     = GetLocalLabel ();

    /* Skip the FOR token */
    NextToken ();

    /* Add the loop to the loop stack */
    AddLoop (oursp, TestLabel, lab, IncLabel, lstat);

    /* Skip the opening paren */
    ConsumeLParen ();

    /* Parse the initializer expression */
    if (CurTok.Tok != TOK_SEMI) {
	expression (&lval1);
    }
    ConsumeSemi ();

    /* Label for the test expressions */
    g_defcodelabel (TestLabel);

    /* Parse the test expression */
    if (CurTok.Tok != TOK_SEMI) {
        Test (lstat, 1);
    	g_jump (lab);
    } else {
    	g_jump (lstat);
    }
    ConsumeSemi ();

    /* Remember the start of the increment expression */
    IncExprStart = GetCodePos();

    /* Label for the increment expression */
    g_defcodelabel (IncLabel);

    /* Parse the increment expression */
    HaveIncExpr = (CurTok.Tok != TOK_RPAREN);
    if (HaveIncExpr) {
    	expression (&lval3);
    }

    /* Jump to the test */
    g_jump (TestLabel);

    /* Remember the end of the increment expression */
    IncExprEnd = GetCodePos();

    /* Skip the closing paren */
    ConsumeRParen ();

    /* Loop body */
    g_defcodelabel (lstat);
    Statement (&PendingToken);

    /* If we had an increment expression, move the code to the bottom of
     * the loop. In this case we don't need to jump there at the end of
     * the loop body.
     */
    if (HaveIncExpr) {
	MoveCode (IncExprStart, IncExprEnd, GetCodePos());
    } else {
	/* Jump back to the increment expression */
	g_jump (IncLabel);
    }

    /* Skip a pending token if we have one */
    SkipPending (PendingToken);

    /* Declare the break label */
    g_defcodelabel (lab);

    /* Remove the loop from the loop stack */
    DelLoop ();
}



static int CompoundStatement (void)
/* Compound statement. Allow any number of statements inside braces. The
 * function returns true if the last statement was a break or return.
 */
{
    int GotBreak;

    /* Remember the stack at block entry */
    int OldStack = oursp;

    /* Enter a new lexical level */
    EnterBlockLevel ();

    /* Parse local variable declarations if any */
    DeclareLocals ();

    /* Now process statements in this block */
    GotBreak = 0;
    while (CurTok.Tok != TOK_RCURLY) {
     	if (CurTok.Tok != TOK_CEOF) {
     	    GotBreak = Statement (0);
     	} else {
     	    break;
     	}
    }

    /* Clean up the stack. */
    if (!GotBreak) {
	g_space (oursp - OldStack);
    }
    oursp = OldStack;

    /* Emit references to imports/exports for this block */
    EmitExternals ();

    /* Leave the lexical level */
    LeaveBlockLevel ();

    return GotBreak;
}



int Statement (int* PendingToken)
/* Statement parser. Returns 1 if the statement does a return/break, returns
 * 0 otherwise. If the PendingToken pointer is not NULL, the function will
 * not skip the terminating token of the statement (closing brace or
 * semicolon), but store true if there is a pending token, and false if there
 * is none. The token is always checked, so there is no need for the caller to
 * check this token, it must be skipped, however. If the argument pointer is
 * NULL, the function will skip the token.
 */
{
    ExprDesc lval;
    int GotBreak;

    /* Assume no pending token */
    if (PendingToken) {
	*PendingToken = 0;
    }

    /* Check for a label */
    if (CurTok.Tok == TOK_IDENT && NextTok.Tok == TOK_COLON) {

	/* Special handling for a label */
	DoLabel ();

    } else {

     	switch (CurTok.Tok) {

     	    case TOK_LCURLY:
	        NextToken ();
		GotBreak = CompoundStatement ();
	        CheckTok (TOK_RCURLY, "`{' expected", PendingToken);
	        return GotBreak;

     	    case TOK_IF:
     	    	return IfStatement ();

     	    case TOK_WHILE:
     	    	WhileStatement ();
	    	break;

	    case TOK_DO:
	    	DoStatement ();
	    	break;

	    case TOK_SWITCH:
	    	SwitchStatement ();
	    	break;

	    case TOK_RETURN:
	    	ReturnStatement ();
	    	CheckSemi (PendingToken);
	    	return 1;

	    case TOK_BREAK:
		BreakStatement ();
	    	CheckSemi (PendingToken);
		return 1;

	    case TOK_CONTINUE:
		ContinueStatement ();
	    	CheckSemi (PendingToken);
		return 1;

	    case TOK_FOR:
		ForStatement ();
		break;

	    case TOK_GOTO:
		GotoStatement ();
	    	CheckSemi (PendingToken);
		return 1;

	    case TOK_SEMI:
		/* Ignore it */
		CheckSemi (PendingToken);
		break;

	    case TOK_PRAGMA:
		DoPragma ();
     		break;

	    default:
	        /* Actual statement */
		expression (&lval);
	    	CheckSemi (PendingToken);
	}
    }
    return 0;
}




