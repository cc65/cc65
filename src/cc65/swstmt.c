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
/*	  	    		     Code		     		     */
/*****************************************************************************/



void SwitchStatement (void)
/* Handle a switch statement for chars with a cmp cascade for the selector */
{
    Collection* Nodes;          /* CaseNode tree */
    ExprDesc SwitchExpr;       	/* Switch statement expression */
    ExprDesc CaseExpr;          /* Case label expression */
    type SwitchExprType;        /* Basic switch expression type */
    CodeMark CaseCodeStart;     /* Start of code marker */
    unsigned Depth;             /* Number of bytes the selector type has */
    unsigned ExitLabel;	       	/* Exit label */
    unsigned CaseLabel;         /* Label for case */
    unsigned DefaultLabel;      /* Label for the default branch */
    long Val;	       		/* Case label value */


    /* Eat the "switch" token */
    NextToken ();

    /* Read the switch expression */
    ConsumeLParen ();
    intexpr (&SwitchExpr);
    ConsumeRParen ();

    /* Opening curly brace */
    ConsumeLCurly ();

    /* Remember the current code position */
    CaseCodeStart = GetCodePos();

    /* Get the unqualified type of the switch expression */
    SwitchExprType = UnqualifiedType (SwitchExpr.Type[0]);

    /* Get the number of bytes the selector type has */
    Depth = SizeOf (SwitchExpr.Type);
    CHECK (Depth == 1 || Depth == 2 || Depth == 4);

    /* Get the exit label for the switch statement */
    ExitLabel = GetLocalLabel ();

    /* Create a loop so we may use break. */
    AddLoop (oursp, 0, ExitLabel, 0, 0);

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
		constexpr (&CaseExpr);
		if (!IsClassInt (CaseExpr.Type)) {
		    Error ("Switch quantity not an integer");
		}

		/* Check the range of the expression */
		Val = CaseExpr.ConstVal;
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
       	    Statement (0);
     	}
    }

    /* Check if we had any labels */
    if (CollCount (Nodes) == 0 && DefaultLabel == 0) {

	Warning ("No case labels");

    } else {

	/* Remember the current position */
	CodeMark SwitchCodeStart = GetCodePos();

	/* Generate code */
       	g_switch (Nodes, DefaultLabel? DefaultLabel : ExitLabel, Depth);

	/* Move the code to the front */
	MoveCode (SwitchCodeStart, GetCodePos(), CaseCodeStart);

    }

    /* Define the exit label */
    g_defcodelabel (ExitLabel);

    /* Eat the closing curly brace */
    NextToken ();
		    
    /* Free the case value tree */
    FreeCaseNodeColl (Nodes);

    /* End the loop */
    DelLoop ();
}



