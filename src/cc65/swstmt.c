/*****************************************************************************/
/*                                                                           */
/*				   swstmt.c                                  */
/*                                                                           */
/*			  Parse the switch statement                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
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



#include <limits.h>

/* common */
#include "coll.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
#include "asmlabel.h"
#include "casenode.h"
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
/*	  	    		     Code  		     		     */
/*****************************************************************************/



void SwitchStatement (void)
/* Handle a switch statement for chars with a cmp cascade for the selector */
{
    Collection* Nodes;          /* CaseNode tree */
    ExprDesc SwitchExpr;       	/* Switch statement expression */
    ExprDesc CaseExpr;          /* Case label expression */
    type SwitchExprType;        /* Basic switch expression type */
    CodeMark CaseCodeStart;     /* Start of code marker */
    CodeMark SwitchCodeStart;   /* Start of switch code */
    CodeMark SwitchCodeEnd;     /* End of switch code */
    unsigned Depth;             /* Number of bytes the selector type has */
    unsigned ExitLabel;	       	/* Exit label */
    unsigned CaseLabel;         /* Label for case */
    unsigned DefaultLabel;      /* Label for the default branch */
    unsigned SwitchCodeLabel;   /* Label for the switch code */
    long     Val;               /* Case label value */
    int      HaveBreak = 0;     /* True if the last statement had a break */


    /* Eat the "switch" token */
    NextToken ();

    /* Read the switch expression and load it into the primary. It must have
     * integer type.
     */
    ConsumeLParen ();
    Expression0 (&SwitchExpr);
    if (!IsClassInt (SwitchExpr.Type))  {
        Error ("Switch quantity is not an integer");
     	/* To avoid any compiler errors, make the expression a valid int */
     	ED_MakeConstAbsInt (&SwitchExpr, 1);
    }
    ConsumeRParen ();

    /* Add a jump to the switch code. This jump is usually unnecessary,
     * because the switch code will moved up just behind the switch
     * expression. However, in rare cases, there's a label at the end of
     * the switch expression. This label will not get moved, so the code
     * jumps around the switch code, and after moving the switch code,
     * things look really weird. If we add a jump here, we will never have
     * a label attached to the current code position, and the jump itself
     * will get removed by the optimizer if it is unnecessary.
     */
    SwitchCodeLabel = GetLocalLabel ();
    g_jump (SwitchCodeLabel);

    /* Remember the current code position. We will move the switch code
     * to this position later.
     */
    GetCodePos (&CaseCodeStart);

    /* Opening curly brace */
    ConsumeLCurly ();

    /* Get the unqualified type of the switch expression */
    SwitchExprType = UnqualifiedType (SwitchExpr.Type[0]);

    /* Get the number of bytes the selector type has */
    Depth = SizeOf (SwitchExpr.Type);
    CHECK (Depth == SIZEOF_CHAR || Depth == SIZEOF_INT || Depth == SIZEOF_LONG);

    /* Get the exit label for the switch statement */
    ExitLabel = GetLocalLabel ();

    /* Create a loop so we may use break. */
    AddLoop (ExitLabel, 0);

    /* Create the collection for the case node tree */
    Nodes = NewCollection ();

    /* Clear the label for the default branch */
    DefaultLabel = 0;

    /* Parse the labels */
    while (CurTok.Tok != TOK_RCURLY) {

	while (CurTok.Tok == TOK_CASE || CurTok.Tok == TOK_DEFAULT) {

	    /* Parse the selector */
	    if (CurTok.Tok == TOK_CASE) {

		/* Skip the "case" token */
		NextToken ();

		/* Read the selector expression */
		ConstAbsIntExpr (hie1, &CaseExpr);

		/* Check the range of the expression */
		Val = CaseExpr.IVal;
		switch (SwitchExprType) {

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

		    case T_SHORT:
		    case T_INT:
		      	if (Val < -32768 || Val > 32767) {
		      	    Error ("Range error");
		      	}
		      	break;

		    case T_USHORT:
		    case T_UINT:
		      	if (Val < 0 || Val > 65535) {
     		      	    Error ("Range error");
		      	}
		      	break;

		    case T_LONG:
		    case T_ULONG:
		        break;

	 	    default:
		      	Internal ("Invalid type: %04X", SwitchExprType);
		}

		/* Insert the case selector into the selector table */
       		CaseLabel = InsertCaseValue (Nodes, Val, Depth);

		/* Define this label */
		g_defcodelabel (CaseLabel);

		/* Skip the colon */
     		ConsumeColon ();

     	    } else {

     		/* Default case */
     		NextToken ();

     		/* Check if we do already have a default branch */
     		if (DefaultLabel == 0) {

     		    /* Generate and emit the default label */
     		    DefaultLabel = GetLocalLabel ();
     		    g_defcodelabel (DefaultLabel);

     		} else {
     		    /* We had the default label already */
     		    Error ("Duplicate `default' case");
     		}

     		/* Skip the colon */
     		ConsumeColon ();

     	    }

     	}

     	/* Parse statements */
     	if (CurTok.Tok != TOK_RCURLY) {
       	    HaveBreak = Statement (0);
     	}
    }

    /* Check if we had any labels */
    if (CollCount (Nodes) == 0 && DefaultLabel == 0) {

	Warning ("No case labels");

    }

    /* If the last statement did not have a break, we may have an open
     * label (maybe from an if or similar). Emitting code and then moving
     * this code to the top will also move the label to the top which is
     * wrong. So if the last statement did not have a break (which would
     * carry the label), add a jump to the exit. If it is useless, the
     * optimizer will remove it later.
     */
    if (!HaveBreak) {
        g_jump (ExitLabel);
    }

    /* Remember the current position */
    GetCodePos (&SwitchCodeStart);

    /* Output the switch code label */
    g_defcodelabel (SwitchCodeLabel);

    /* Generate code */
    g_switch (Nodes, DefaultLabel? DefaultLabel : ExitLabel, Depth);

    /* Move the code to the front */
    GetCodePos (&SwitchCodeEnd);
    MoveCode (&SwitchCodeStart, &SwitchCodeEnd, &CaseCodeStart);

    /* Define the exit label */
    g_defcodelabel (ExitLabel);

    /* Eat the closing curly brace */
    NextToken ();

    /* Free the case value tree */
    FreeCaseNodeColl (Nodes);

    /* End the loop */
    DelLoop ();
}



