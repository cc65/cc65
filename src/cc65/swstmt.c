/*****************************************************************************/
/*                                                                           */
/*				   swstmt.c                                  */
/*                                                                           */
/*			  Parse the switch statement                         */
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



/* common */
#include "coll.h"
#include "xmalloc.h"

/* cc65 */
#include "asmlabel.h"
#include "codegen.h"
#include "datatype.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "loop.h"
#include "scanner.h"
#include "stmt.h"
#include "swstmt.h"



/*****************************************************************************/
/*	  	  		     Code		     		     */
/*****************************************************************************/



static void CascadeSwitch (ExprDesc* Expr)
/* Handle a switch statement for chars with a cmp cascade for the selector */
{
    unsigned ExitLab;  	     	/* Exit label */
    unsigned NextLab;  	     	/* Next case label */
    unsigned CodeLab;		/* Label that starts the actual selector code */
    int HaveBreak;   		/* Remember if we exited with break */
    int HaveDefault;		/* Remember if we had a default label */
    int lcount;	       	       	/* Label count */
    unsigned Flags;    		/* Code generator flags */
    ExprDesc lval;		/* Case label expression */
    long Val;	       		/* Case label value */


    /* Get the unqualified type of the switch expression */
    type ExprType = UnqualifiedType (Expr->Type[0]);

    /* Create a loop so we may break out, init labels */
    ExitLab = GetLocalLabel ();
    AddLoop (oursp, 0, ExitLab, 0, 0);

    /* Setup some variables needed in the loop  below */
    Flags = TypeOf (Expr->Type) | CF_CONST | CF_FORCECHAR;
    CodeLab = NextLab = 0;
    HaveBreak = 1;
    HaveDefault = 0;

    /* Parse the labels */
    lcount = 0;
    while (CurTok.Tok != TOK_RCURLY) {

	if (CurTok.Tok == TOK_CASE || CurTok.Tok == TOK_DEFAULT) {

	    /* If the code for the previous selector did not end with a
	     * break statement, we must jump over the next selector test.
	     */
	    if (!HaveBreak) {
		/* Define a label for the code */
		if (CodeLab == 0) {
		    CodeLab = GetLocalLabel ();
		}
	     	g_jump (CodeLab);
	    }

	    /* If we have a cascade label, emit it */
	    if (NextLab) {
	     	g_defcodelabel (NextLab);
	     	NextLab = 0;
	    }

	    while (CurTok.Tok == TOK_CASE || CurTok.Tok == TOK_DEFAULT) {

	     	/* Parse the selector */
	     	if (CurTok.Tok == TOK_CASE) {

	     	    /* Count labels */
	     	    ++lcount;

	     	    /* Skip the "case" token */
		    NextToken ();

		    /* Read the selector expression */
		    constexpr (&lval);
		    if (!IsClassInt (lval.Type)) {
			Error ("Switch quantity not an integer");
		    }

		    /* Check the range of the expression */
		    Val = lval.ConstVal;
		    switch (ExprType) {

			case T_SCHAR:
			    /* Signed char */
			    if (Val < -128 || Val > 127) {
		     	  	Error ("Range error");
		     	    }
		     	    break;

		     	case T_UCHAR:
		     	    if (Val < 0 || Val > 255) {
		     	  	Error ("Range error");
		     	    }
		     	    break;

		     	case T_INT:
		     	    if (Val < -32768 || Val > 32767) {
		     	  	Error ("Range error");
		     	    }
		     	    break;

		     	case T_UINT:
		     	    if (Val < 0 || Val > 65535) {
		     	  	Error ("Range error");
		     	    }
		     	    break;

		     	default:
		     	    Internal ("Invalid type: %04X", ExprType);
		    }

		    /* Emit a compare */
	     	    g_cmp (Flags, Val);

	    	    /* If another case follows after the colon (which is
		     * currently pending and cannot be skipped since otherwise
		     * the debug infos will get wrong), we will jump to the
		     * code if the condition is true.
	    	     */
       	    	    if (NextTok.Tok == TOK_CASE) {
		     	/* Create a code label if needed */
		     	if (CodeLab == 0) {
		     	    CodeLab = GetLocalLabel ();
		     	}
		     	g_falsejump (CF_NONE, CodeLab);
		    } else if (NextTok.Tok != TOK_DEFAULT) {
		  	/* No case follows, jump to next selector */
		  	if (NextLab == 0) {
		  	    NextLab = GetLocalLabel ();
		  	}
		     	g_truejump (CF_NONE, NextLab);
		    }

		    /* Skip the colon */
		    ConsumeColon ();

		} else {

		    /* Default case */
		    NextToken ();

		    /* Handle the pathologic case: DEFAULT followed by CASE */
       	       	    if (NextTok.Tok == TOK_CASE) {
		  	if (CodeLab == 0) {
		   	    CodeLab = GetLocalLabel ();
		  	}
		    	g_jump (CodeLab);
		    }

		    /* Skip the colon */
		    ConsumeColon ();

		    /* Remember that we had a default label */
		    HaveDefault = 1;
		}

	    }

        }

	/* Emit a code label if we have one */
	if (CodeLab) {
	    g_defcodelabel (CodeLab);
	    CodeLab = 0;
	}

	/* Parse statements */
	if (CurTok.Tok != TOK_RCURLY) {
       	    HaveBreak = Statement (0);
	}
    }

    /* Check if we have any labels */
    if (lcount == 0 && !HaveDefault) {
     	Warning ("No case labels");
    }

    /* Define the exit label and, if there's a next label left, create this
     * one, too.
     */
    if (NextLab) {
	g_defcodelabel (NextLab);
    }
    g_defcodelabel (ExitLab);

    /* Eat the closing curly brace */
    NextToken ();

    /* End the loop */
    DelLoop ();
}



static void TableSwitch (ExprDesc* Expr)
/* Handle a switch statement via table based selector */
{
    /* Entry for one case in a switch statement */
    typedef struct {
    	long     Value;	        /* selector value */
       	unsigned Label; 	/* label for this selector */
    } SwitchEntry;

    unsigned DefaultLabel;      /* Label for default case */
    unsigned ExitLabel;	   	/* exit label */
    int lcase; 	       	       	/* label for compares */
    int HaveBreak;     	     	/* Last statement has a break */
    unsigned Flags;    	     	/* Code generator flags */
    ExprDesc lval;     	     	/* Case label expression */
    unsigned I;
    SwitchEntry* P;
    Collection SwitchTab;

    /* Initialize the collection for the switch entries */
    InitCollection (&SwitchTab);

    /* Create a look so we may break out, init labels */
    HaveBreak    = 0;     		/* Keep gcc silent */
    DefaultLabel = 0;  	       	        /* No default case until now */
    ExitLabel    = GetLocalLabel ();	/* get exit */
    AddLoop (oursp, 0, ExitLabel, 0, 0);

    /* Jump behind the code for the CASE labels */
    g_jump (lcase = GetLocalLabel ());
    while (CurTok.Tok != TOK_RCURLY) {
    	if (CurTok.Tok == TOK_CASE || CurTok.Tok == TOK_DEFAULT) {
    	    do {
    	    	if (CurTok.Tok == TOK_CASE) {
       	    	    NextToken ();
    	    	    constexpr (&lval);
	    	    if (!IsClassInt (lval.Type)) {
	    	     	Error ("Switch quantity not an integer");
	      	    }
		    P = xmalloc (sizeof (SwitchEntry));
     	    	    P->Value = lval.ConstVal;
    	    	    P->Label = GetLocalLabel ();
		    CollAppend (&SwitchTab, P);
		    g_defcodelabel (P->Label);
    	    	} else if (DefaultLabel == 0) {
    	    	    NextToken ();
     	    	    DefaultLabel = GetLocalLabel ();
		    g_defcodelabel (DefaultLabel);
    	    	} else {
		    /* We already had a default label */
		    Error ("Multiple default labels in one switch");
		    /* Try to recover */
		    NextToken ();
		}
    	    	ConsumeColon ();
    	    } while (CurTok.Tok == TOK_CASE || CurTok.Tok == TOK_DEFAULT);
	    HaveBreak = 0;
    	}
    	if (CurTok.Tok != TOK_RCURLY) {
    	    HaveBreak = Statement (0);
    	}
    }

    /* Check if we have any labels */
    if (CollCount(&SwitchTab) == 0 && DefaultLabel == 0) {
     	Warning ("No case labels");
    }

    /* Eat the closing curly brace */
    NextToken ();

    /* If the last statement doesn't have a break or return, add one */
    if (!HaveBreak) {
        g_jump (ExitLabel);
    }

    /* Actual selector code goes here */
    g_defcodelabel (lcase);

    /* Create the call to the switch subroutine */
    Flags = TypeOf (Expr->Type);
    g_switch (Flags);

    /* First entry is negative of label count */
    g_defdata (CF_INT | CF_CONST, -((int)CollCount(&SwitchTab))-1, 0);

    /* Create the case selector table */
    for (I = 0; I < CollCount (&SwitchTab); ++I) {
	P = CollAt (&SwitchTab, I);
       	g_case (Flags, P->Label, P->Value);	/* Create one label */
    }

    if (DefaultLabel != 0) {
       	g_jump (DefaultLabel);
    }
    g_defcodelabel (ExitLabel);
    DelLoop ();

    /* Free the allocated space for the labels */
    for (I = 0; I < CollCount (&SwitchTab); ++I) {
	xfree (CollAt (&SwitchTab, I));
    }

    /* Free the collection itself */
    DoneCollection (&SwitchTab);
}



void SwitchStatement (void)
/* Handle a 'switch' statement */
{
    ExprDesc Expr;	       	/* Switch statement expression */

    /* Eat the "switch" */
    NextToken ();

    /* Read the switch expression */
    ConsumeLParen ();
    intexpr (&Expr);
    ConsumeRParen ();

    /* result of expr is in P */
    ConsumeLCurly ();

    /* Now decide which sort of switch we will create: */
    if (IsTypeChar (Expr.Type) || (CodeSizeFactor >= 200 && IsClassInt (Expr.Type))) {
       	CascadeSwitch (&Expr);
    } else {
      	TableSwitch (&Expr);
    }
}



