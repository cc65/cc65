/*****************************************************************************/
/*                                                                           */
/*                                  stmt.c                                   */
/*                                                                           */
/*                             Parse a statement                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2010, Ullrich von Bassewitz                                      */
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
#include "loadexpr.h"
#include "locals.h"
#include "loop.h"
#include "pragma.h"
#include "scanner.h"
#include "seqpoint.h"
#include "stackptr.h"
#include "stmt.h"
#include "swstmt.h"
#include "symtab.h"
#include "testexpr.h"
#include "typeconv.h"



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static int CheckLabelWithoutStatement (void)
/* Called from Statement() after a label definition. Will check for a
** following closing curly brace. This means that a label is not followed
** by a statement which is required by the standard. Output an error if so.
*/
{
    if (CurTok.Tok == TOK_RCURLY) {
        Error ("Label at end of compound statement");
        return 1;
    } else {
        return 0;
    }
}



static void CheckTok (token_t Tok, const char* Msg, int* PendingToken)
/* Helper function for Statement. Will check for Tok and print Msg if not
** found. If PendingToken is NULL, it will the skip the token, otherwise
** it will store one to PendingToken.
*/
{
    if (CurTok.Tok != Tok) {
        Error ("%s", Msg);
    } else if (PendingToken) {
        *PendingToken = 1;
    } else {
        NextToken ();
    }
}



static void CheckSemi (int* PendingToken)
/* Helper function for Statement. Will check for a semicolon and print an
** error message if not found (plus some error recovery). If PendingToken is
** NULL, it will the skip the token, otherwise it will store one to
** PendingToken.
** This function is a special version of CheckTok with the addition of the
** error recovery.
*/
{
    int HaveToken = (CurTok.Tok == TOK_SEMI);
    if (!HaveToken) {
        Error ("';' expected");
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
/*                                   Code                                    */
/*****************************************************************************/



static int IfStatement (void)
/* Handle an 'if' statement */
{
    unsigned Label1;
    unsigned TestResult;
    int StmtFlags;

    /* Skip the if */
    NextToken ();

    /* Generate a jump label and parse the condition */
    Label1 = GetLocalLabel ();
    TestResult = TestInParens (Label1, 0);

    /* Output a warning if the condition is always false */
    if (TestResult == TESTEXPR_FALSE) {
        UnreachableCodeWarning ();
    }

    /* Parse the if body */
    StmtFlags = AnyStatement (0, 0);

    /* Else clause present? */
    if (CurTok.Tok != TOK_ELSE) {

        g_defcodelabel (Label1);

        /* If the test result is always true, any special statement (return,
        ** break etc.) is always executed. If not, we cannot be sure.
        */
        if (TestResult != TESTEXPR_TRUE) {
            /* There's no else so we're not sure, if any special statements
            ** are really executed.
            */
            StmtFlags &= ~SF_MASK_UNREACH;
        }

    } else {

        int StmtFlags2;

        /* Generate a jump around the else branch */
        unsigned Label2 = GetLocalLabel ();
        g_jump (Label2);

        /* Skip the else */
        NextToken ();

        /* If the if expression was always true, the code in the else branch
        ** is never executed. Output a warning if this is the case.
        */
        if (TestResult == TESTEXPR_TRUE) {
            UnreachableCodeWarning ();
        }

        /* Define the target for the first test */
        g_defcodelabel (Label1);

        /* Parse the else body and evaluate special statements */
        StmtFlags2 = AnyStatement (0, 0);
        if (TestResult == TESTEXPR_FALSE) {
            /* The "else" part is always executed */
            StmtFlags = StmtFlags2;
        } else if (TestResult != TESTEXPR_TRUE) {
            /* If both branches have a special statement the following code is
            ** unreachable and we combine the flags. Otherwise the code
            ** following the "if" is always reachable.
            */
            StmtFlags |= SF_Any (StmtFlags2);
            if (SF_Unreach (StmtFlags) && SF_Unreach (StmtFlags2)) {
                StmtFlags |= SF_Unreach (StmtFlags2);
            } else {
                StmtFlags &= ~SF_MASK_UNREACH;
            }
        }

        /* Generate the label for the else clause */
        g_defcodelabel (Label2);
    }

    /* Done. We will never return "empty" for an if statement because of side
    ** effects when evaluating the condition.
    */
    return StmtFlags & ~SF_EMPTY;
}



static int DoStatement (void)
/* Handle the 'do' statement */
{
    int StmtFlags;
    unsigned TestResult;

    /* Get the loop control labels */
    unsigned LoopLabel      = GetLocalLabel ();
    unsigned BreakLabel     = GetLocalLabel ();
    unsigned ContinueLabel  = GetLocalLabel ();

    /* Skip the while token */
    NextToken ();

    /* Add the loop to the loop stack */
    AddLoop (BreakLabel, ContinueLabel);

    /* Define the loop label */
    g_defcodelabel (LoopLabel);

    /* Parse the loop body */
    StmtFlags = AnyStatement (0, 0);

    /* Output the label for a continue */
    g_defcodelabel (ContinueLabel);

    /* Parse the end condition */
    Consume (TOK_WHILE, "'while' expected");
    TestResult = TestInParens (LoopLabel, 1);
    ConsumeSemi ();

    /* Define the break label */
    g_defcodelabel (BreakLabel);

    /* Remove the loop from the loop stack */
    DelLoop ();

    /* Fix the flags for the loop. */
    if (TestResult == TESTEXPR_TRUE) {
        /* If the loop condition is always true, and we do not have a
        ** "break" statement, the loop won't terminate. So the only valid
        ** "unreach" flag is that for the endless loop. Otherwise - if there
        ** is a "break" statement, the code after the loop is reachable.
        */
        StmtFlags &= ~SF_MASK_UNREACH;
        if (!SF_Any_Break (StmtFlags)) {
            StmtFlags |= (SF_OTHER | SF_ANY_OTHER);
        }
    } else if (SF_Any_Break (StmtFlags)) {
        /* If the loop condition is not always true, but we have a "break"
        ** anywhere, the following code is reachable.
        */
        StmtFlags &= ~SF_MASK_UNREACH;
    } else {
        /* Otherwise the last statement in the loop determines the status for
        ** the following code, so we have to change "continue" into "other"
        ** but apart from that flags stay as they are.
        */
        if (SF_Continue (StmtFlags)) {
            StmtFlags &= ~SF_CONTINUE;
            StmtFlags |= (SF_OTHER | SF_ANY_OTHER);
        }
    }

    /* "break" and "continue" are not relevant for the following code. "empty"
    ** is removed because of side effects when evaluating the condition.
    */
    StmtFlags &= ~(SF_ANY_BREAK | SF_ANY_CONTINUE | SF_EMPTY);

    /* Done */
    return StmtFlags;
}



static int WhileStatement (void)
/* Handle the 'while' statement */
{
    int         PendingToken;
    CodeMark    CondCodeStart;  /* Start of condition evaluation code */
    CodeMark    CondCodeEnd;    /* End of condition evaluation code */
    CodeMark    Here;           /* "Here" location of code */
    unsigned    TestResult;     /* Result of the while loop test expression */
    int         StmtFlags;

    /* Get the loop control labels */
    unsigned LoopLabel  = GetLocalLabel ();
    unsigned BreakLabel = GetLocalLabel ();
    unsigned CondLabel  = GetLocalLabel ();

    /* Skip the while token */
    NextToken ();

    /* Add the loop to the loop stack. In case of a while loop, the condition
    ** label is used for continue statements.
    */
    AddLoop (BreakLabel, CondLabel);

    /* We will move the code that evaluates the while condition to the end of
    ** the loop, so generate a jump here.
    */
    g_jump (CondLabel);

    /* Remember the current position */
    GetCodePos (&CondCodeStart);

    /* Test the loop condition. While loops are somewhat different from other
    ** loops: While an "always true" condition is used often for endless loops
    ** (or loops left by "break"), an "always false" condition doesn't make
    ** sense, so we check that here and warn about it.
    */
    TestResult = TestInParens (LoopLabel, 1);
    if (TestResult == TESTEXPR_FALSE) {
        UnreachableCodeWarning ();
    }

    /* Remember the end of the condition evaluation code */
    GetCodePos (&CondCodeEnd);

    /* Define the head label */
    g_defcodelabel (LoopLabel);

    /* Loop body */
    StmtFlags = AnyStatement (&PendingToken, 0);

    /* Emit the while condition label */
    g_defcodelabel (CondLabel);

    /* Move the test code here */
    GetCodePos (&Here);
    MoveCode (&CondCodeStart, &CondCodeEnd, &Here);

    /* Exit label */
    g_defcodelabel (BreakLabel);

    /* Eat remaining tokens that were delayed because of line info
    ** correctness
    */
    SkipPending (PendingToken);

    /* Remove the loop from the loop stack */
    DelLoop ();

    /* Fix the flags for the loop. */
    if (TestResult == TESTEXPR_TRUE) {
        /* If the loop condition is always true, and we do not have a
        ** "break" statement, the loop won't terminate. So the only valid
        ** "unreach" flag is that for the endless loop. Otherwise - if there
        ** is a "break" statement, the code after the loop is reachable.
        */
        StmtFlags &= ~SF_MASK_UNREACH;
        if (!SF_Any_Break (StmtFlags)) {
            StmtFlags |= (SF_OTHER | SF_ANY_OTHER);
        }
    } else {
        /* If the loop condition is not always true, the code after the loop
        ** is always reachable.
        */
        StmtFlags &= ~SF_MASK_UNREACH;
    }
    /* "break" and "continue" are not relevant for the following code. "empty"
    ** is removed because of side effects when evaluating the condition.
    */
    StmtFlags &= ~(SF_ANY_BREAK | SF_ANY_CONTINUE | SF_EMPTY);

    /* Done */
    return StmtFlags;
}



static int ReturnStatement (void)
/* Handle the 'return' statement */
{
    ExprDesc    Expr;

    ED_Init (&Expr);
    NextToken ();
    if (CurTok.Tok != TOK_SEMI) {

        /* Evaluate the return expression */
        hie0 (&Expr);

        /* If we return something in a function with void or incomplete return
        ** type, print an error and ignore the value. Otherwise convert the
        ** value to the type of the return.
        */
        if (F_HasVoidReturn (CurrentFunc)) {
            Error ("Returning a value in function with return type 'void'");
        } else {
            /* Check the return type first */
            const Type* ReturnType = F_GetReturnType (CurrentFunc);

            /* Convert the return value to the type of the function result */
            TypeConversion (&Expr, ReturnType);

            /* Load the value into the primary */
            if (IsClassStruct (Expr.Type)) {
                /* Handle struct/union specially */
                LoadExpr (CG_TypeOf (GetStructReplacementType (ReturnType)), &Expr);
            } else {
                /* Load the value into the primary */
                LoadExpr (CF_NONE, &Expr);
            }

            /* Append deferred inc/dec at sequence point */
            DoDeferred (SQP_KEEP_EAX, &Expr);
        }

    } else if (!F_HasVoidReturn (CurrentFunc) && !F_HasOldStyleIntRet (CurrentFunc)) {
        Error ("Function '%s' must return a value", F_GetFuncName (CurrentFunc));
    }

    /* Mark the function as having a return statement */
    F_ReturnFound (CurrentFunc);

    /* Cleanup the stack in case we're inside a block with locals */
    g_space (StackPtr - F_GetTopLevelSP (CurrentFunc));

    /* Output a jump to the function exit code */
    g_jump (F_GetRetLab (CurrentFunc));

    /* Done */
    return SF_RETURN | SF_ANY_RETURN;
}



static int BreakStatement (void)
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
        Error ("'break' statement not within loop or switch");
        return SF_NONE;
    }

    /* Correct the stack pointer if needed */
    g_space (StackPtr - L->StackPtr);

    /* Jump to the exit label of the loop */
    g_jump (L->BreakLabel);

    /* Done */
    return SF_BREAK | SF_ANY_BREAK;
}



static int ContinueStatement (void)
/* Handle the 'continue' statement */
{
    LoopDesc* L;

    /* Skip the continue */
    NextToken ();

    /* Get the current loop descriptor */
    L = CurrentLoop ();
    if (L) {
        /* Search for a loop that has a continue label. */
        do {
            if (L->ContinueLabel) {
                break;
            }
            L = L->Next;
        } while (L);
    }

    /* Did we find it? */
    if (L == 0) {
        Error ("'continue' statement not within a loop");
        return SF_NONE;
    }

    /* Correct the stackpointer if needed */
    g_space (StackPtr - L->StackPtr);

    /* Jump to next loop iteration */
    g_jump (L->ContinueLabel);

    /* Done */
    return SF_CONTINUE | SF_ANY_CONTINUE;
}



static int ForStatement (void)
/* Handle a 'for' statement */
{
    int HaveIncExpr;
    CodeMark IncExprStart;
    CodeMark IncExprEnd;
    int PendingToken;
    unsigned TestResult;
    int StmtFlags;

    /* Get several local labels needed later */
    unsigned TestLabel    = GetLocalLabel ();
    unsigned BreakLabel   = GetLocalLabel ();
    unsigned IncLabel     = GetLocalLabel ();
    unsigned BodyLabel    = GetLocalLabel ();

    /* Skip the FOR token */
    NextToken ();

    /* Add the loop to the loop stack. A continue jumps to the start of the
    ** the increment condition.
    */
    AddLoop (BreakLabel, IncLabel);

    /* Skip the opening paren */
    ConsumeLParen ();

    /* Parse the initializer expression */
    if (CurTok.Tok != TOK_SEMI) {
        /* The value of the expression is unused */
        ExprDesc lval1;
        ED_Init (&lval1);
        lval1.Flags = E_NEED_NONE;
        Expression0 (&lval1);
    }
    ConsumeSemi ();

    /* Label for the test expressions */
    g_defcodelabel (TestLabel);

    /* Parse the test expression */
    if (CurTok.Tok != TOK_SEMI) {
        TestResult = Test (BodyLabel, 1);
        g_jump (BreakLabel);
    } else {
        TestResult = TESTEXPR_TRUE;
        g_jump (BodyLabel);
    }
    ConsumeSemi ();

    /* Remember the start of the increment expression */
    GetCodePos (&IncExprStart);

    /* Label for the increment expression */
    g_defcodelabel (IncLabel);

    /* Parse the increment expression */
    HaveIncExpr = (CurTok.Tok != TOK_RPAREN);
    if (HaveIncExpr) {
        /* The value of the expression is unused */
        ExprDesc lval3;
        ED_Init (&lval3);
        lval3.Flags = E_NEED_NONE;
        Expression0 (&lval3);
    }

    /* Jump to the test */
    g_jump (TestLabel);

    /* Remember the end of the increment expression */
    GetCodePos (&IncExprEnd);

    /* Skip the closing paren */
    ConsumeRParen ();

    /* Output a warning if the loop body is never executed */
    if (TestResult == TESTEXPR_FALSE) {
        UnreachableCodeWarning ();
    }

    /* Loop body */
    g_defcodelabel (BodyLabel);
    StmtFlags = AnyStatement (&PendingToken, 0);

    /* If we had an increment expression, move the code to the bottom of
    ** the loop. In this case we don't need to jump there at the end of
    ** the loop body.
    */
    if (HaveIncExpr) {
        CodeMark Here;
        GetCodePos (&Here);
        MoveCode (&IncExprStart, &IncExprEnd, &Here);
    } else {
        /* Jump back to the increment expression */
        g_jump (IncLabel);
    }

    /* Skip a pending token if we have one */
    SkipPending (PendingToken);

    /* Declare the break label */
    g_defcodelabel (BreakLabel);

    /* Remove the loop from the loop stack */
    DelLoop ();

    /* Fix the flags for the loop. */
    if (TestResult == TESTEXPR_TRUE) {
        /* If the loop condition is always true, and we do not have a
        ** "break" statement, the loop won't terminate. So the only valid
        ** "unreach" flag is that for the endless loop. Otherwise - if there
        ** is a "break" statement, the code after the loop is reachable.
        */
        StmtFlags &= ~SF_MASK_UNREACH;
        if (!SF_Any_Break (StmtFlags)) {
            StmtFlags |= (SF_OTHER | SF_ANY_OTHER);
        }
    } else {
        /* If the loop condition is not always true, the code after the loop
        ** is always reachable.
        */
        StmtFlags &= ~SF_MASK_UNREACH;
    }
    /* "break" and "continue" are not relevant for the following code. "empty"
    ** is removed because of side effects when evaluating the condition.
    */
    StmtFlags &= ~(SF_ANY_BREAK | SF_ANY_CONTINUE | SF_EMPTY);

    /* Done */
    return StmtFlags;
}



static int CompoundStatement (int* PendingToken, struct SwitchCtrl* Switch)
/* Compound statement. Allow any number of statements inside braces. The
** function returns true if the last statement was a break or return.
*/
{
    int OldStack;
    unsigned OldBlockStackSize;
    int StmtFlags;

    /* Skip '{' */
    NextToken ();

    /* If the closing curly bracket follows we have an empty statement */
    if (CurTok.Tok == TOK_RCURLY) {
        NextToken ();
        return SF_EMPTY;
    }

    /* Remember the stack at block entry */
    OldStack = StackPtr;
    OldBlockStackSize = CollCount (&CurrentFunc->LocalsBlockStack);

    /* Enter a new lexical level */
    EnterBlockLevel ();

    /* Parse local variable declarations if any */
    DeclareLocals ();

    /* Now process statements in this block checking for unreachable code */
    StmtFlags = StatementBlock (Switch);

    /* Clean up the stack if the codeflow may reach the end */
    if ((StmtFlags & SF_MASK_UNREACH) == SF_NONE) {
        g_space (StackPtr - OldStack);
    }

    /* If the segment had autoinited variables, let's pop it of a stack
    ** of such blocks.
    */
    if (OldBlockStackSize != CollCount (&CurrentFunc->LocalsBlockStack)) {
        CollPop (&CurrentFunc->LocalsBlockStack);
    }
    StackPtr = OldStack;

    /* Emit references to imports/exports for this block */
    EmitExternals ();

    /* Leave the lexical level */
    LeaveBlockLevel ();

    /* Skip '}' */
    CheckTok (TOK_RCURLY, "'}' expected", PendingToken);

    /* Done */
    return StmtFlags;
}



static int Statement (int* PendingToken)
/* Single-line statement */
{
    ExprDesc Expr;
    unsigned PrevErrorCount;
    CodeMark Start, End;

    /* Remember the current error count and code position */
    PrevErrorCount = ErrorCount;
    GetCodePos (&Start);

    /* Actual statement */
    ED_Init (&Expr);
    Expr.Flags |= E_NEED_NONE;
    Expression0 (&Expr);

    /* If the statement has no observable effect and isn't cast to type
    ** void, emit a warning and remove useless code if any.
    */
    GetCodePos (&End);
    if (CodeRangeIsEmpty (&Start, &End) ||
        (Expr.Flags & E_SIDE_EFFECTS) == 0) {

        if (!ED_MayHaveNoEffect (&Expr) &&
            IS_Get (&WarnNoEffect)      &&
            PrevErrorCount == ErrorCount) {
            Warning ("Statement has no effect");
        }

        /* Remove code with no effect */
        RemoveCode (&Start);
    }

    CheckSemi (PendingToken);

    /* Done. All special statements are handled in other subroutines. */
    return SF_NONE;
}



int StatementBlock (struct SwitchCtrl* Switch)
/* Parse multiple statements within curly braces checking for unreachable
** code. Returns the SF_xxx flags for the last statement.
*/
{
    /* We want to emit an "unreachable code" warning for statements following
    ** a "goto", "return" etc. But only - and this is what complicates it -
    ** if the following statement is not preceeded by a label. Since the latter
    ** means that a jump may go there so the statement is actually reachable.
    */
    if (CurTok.Tok != TOK_RCURLY && CurTok.Tok != TOK_CEOF) {
        LineInfo* LI1 = UseLineInfo (GetDiagnosticLI ());
        int StmtFlags1 = AnyStatement (0, Switch);
        int Unreachable = 0;            /* True if code is unreachable */
        int Warning = 0;                /* True if warning was output */
        while (CurTok.Tok != TOK_RCURLY && CurTok.Tok != TOK_CEOF) {
            LineInfo* LI2 = UseLineInfo (GetDiagnosticLI ());
            int StmtFlags2 = AnyStatement (0, Switch);

            /* If this statement is not already unreachable, check if the
            ** previous statement made it unreachable.
            */
            if (!Unreachable) {
                Unreachable = SF_Unreach (StmtFlags1);
            }
            /* If the previous statement made this one unreachable, but this
            ** one has a label, it is not unreachable.
            */
            if (Unreachable && SF_Label (StmtFlags2)) {
                Unreachable = 0;
            }
            /* If this statement is unreachable but not the empty statement,
            ** and we didn't give a warning before, to that now
            */
            if (Unreachable && !SF_Empty (StmtFlags2) && !Warning) {
                LIUnreachableCodeWarning (LI2);
                Warning = 1;
            }

            /* If the current statement wasn't unreachable update the flags */
            if (!Unreachable) {
                StmtFlags1 = SF_Any (StmtFlags1) | StmtFlags2;
            }

            /* Prepare for the next round */
            if (LI1) {
                ReleaseLineInfo (LI1);
            }
            LI1 = LI2;
        }
        if (LI1) {
            ReleaseLineInfo (LI1);
        }
        return StmtFlags1;
    } else {
        return SF_NONE;
    }
}



int AnyStatement (int* PendingToken, struct SwitchCtrl* Switch)
/* Statement parser. Returns one of the SF_xxx flags describing if the
** statement does a return/break. If the PendingToken pointer is not NULL,
** the function will not skip the terminating token of the statement (closing
** brace or semicolon), but store true if there is a pending token, and false
** if there is none. The token is always checked, so there is no need for the
** caller to check this token, it must be skipped, however. If the argument
** pointer is NULL, the function will skip the token. When called to parse a
** switch body, the switch control structure must be passed via the Switch
** argument. Otherwise it must be NULL.
*/
{
    int LabelFlags = SF_NONE;
    int StmtFlags;
    unsigned PrevErrorCount;
    LineInfo* LI;

    /* Assume no pending token */
    if (PendingToken) {
        *PendingToken = 0;
    }

    /* Handle any labels. A label is always part of a statement, it does not
    ** replace one. If we have errors parsing labels, return without reading
    ** the following statement.
    */
    PrevErrorCount = ErrorCount;
    while (1) {
        if (CurTok.Tok == TOK_IDENT && NextTok.Tok == TOK_COLON) {
            /* C 'goto' label */
            DoLabel ();
            LabelFlags |= SF_LABEL_GOTO;
        } else if (CurTok.Tok == TOK_CASE) {
            /* C 'case' label */
            CaseLabel ();
            LabelFlags |= SF_LABEL_CASE;
        } else if (CurTok.Tok == TOK_DEFAULT) {
            /* C 'default' label */
            DefaultLabel ();
            LabelFlags |= SF_LABEL_DEFAULT;
        } else {
            /* No labels */
            break;
        }
    }
    if (LabelFlags != SF_NONE) {
        /* We had labels, check for errors */
        if (PrevErrorCount != ErrorCount || CheckLabelWithoutStatement ()) {
            return SF_NONE;
        }
    }

    /* Remember the line info for the now following statement */
    LI = UseLineInfo (GetDiagnosticLI ());

    /* Now look at the actual statement. */
    switch (CurTok.Tok) {

        case TOK_IF:
            StmtFlags = IfStatement ();
            break;

        case TOK_SWITCH:
            StmtFlags = SwitchStatement ();
            break;

        case TOK_WHILE:
            StmtFlags = WhileStatement ();
            break;

        case TOK_DO:
            StmtFlags = DoStatement ();
            break;

        case TOK_FOR:
            StmtFlags = ForStatement ();
            break;

        case TOK_GOTO:
            StmtFlags = GotoStatement ();
            CheckSemi (PendingToken);
            break;

        case TOK_RETURN:
            StmtFlags = ReturnStatement ();
            CheckSemi (PendingToken);
            break;

        case TOK_BREAK:
            StmtFlags = BreakStatement ();
            CheckSemi (PendingToken);
            break;

        case TOK_CONTINUE:
            StmtFlags = ContinueStatement ();
            CheckSemi (PendingToken);
            break;

        case TOK_SEMI:
            /* Empty statement. Ignore it */
            CheckSemi (PendingToken);
            StmtFlags = SF_EMPTY;
            break;

        case TOK_LCURLY:
            StmtFlags = CompoundStatement (PendingToken, Switch);
            break;

        default:
            /* Simple statement */
            StmtFlags = Statement (PendingToken);
            break;
    }

    /* The flags for labels returned by subroutines are invalid in most cases,
    ** so we remove them and use the ones determined before.
    */
    StmtFlags = (StmtFlags & ~SF_MASK_LABEL) | LabelFlags;

    /* Reset SQP flags */
    SetSQPFlags (SQP_KEEP_NONE);

    /* If we're inside a switch, do tracking of special statements */
    if (Switch) {
        SwitchBodyStatement (Switch, LI, StmtFlags);
    }

    /* Release the line info we remembered above */
    ReleaseLineInfo (LI);

    /* Done */
    return StmtFlags;
}
