/*****************************************************************************/
/*                                                                           */
/*                                 swstmt.c                                  */
/*                                                                           */
/*                        Parse the switch statement                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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
/*                                   Data                                    */
/*****************************************************************************/



typedef struct SwitchCtrl SwitchCtrl;
struct SwitchCtrl {
    Collection* Nodes;          /* CaseNode tree */
    TypeCode    ExprType;       /* Basic switch expression type */
    unsigned    Depth;          /* Number of bytes the selector type has */
    unsigned    DefaultLabel;   /* Label for the default branch */



};

/* Pointer to current switch control struct */
static SwitchCtrl* Switch = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void SwitchStatement (void)
/* Handle a switch statement for chars with a cmp cascade for the selector */
{
    ExprDesc    SwitchExpr;     /* Switch statement expression */
    CodeMark    CaseCodeStart;  /* Start of code marker */
    CodeMark    SwitchCodeStart;/* Start of switch code */
    CodeMark    SwitchCodeEnd;  /* End of switch code */
    unsigned    ExitLabel;      /* Exit label */
    unsigned    SwitchCodeLabel;/* Label for the switch code */
    int         HaveBreak = 0;  /* True if the last statement had a break */
    int         RCurlyBrace;    /* True if last token is right curly brace */
    SwitchCtrl* OldSwitch;      /* Pointer to old switch control data */
    SwitchCtrl  SwitchData;     /* New switch data */


    /* Eat the "switch" token */
    NextToken ();

    /* Read the switch expression and load it into the primary. It must have
    ** integer type.
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
    ** because the switch code will moved up just behind the switch
    ** expression. However, in rare cases, there's a label at the end of
    ** the switch expression. This label will not get moved, so the code
    ** jumps around the switch code, and after moving the switch code,
    ** things look really weird. If we add a jump here, we will never have
    ** a label attached to the current code position, and the jump itself
    ** will get removed by the optimizer if it is unnecessary.
    */
    SwitchCodeLabel = GetLocalLabel ();
    g_jump (SwitchCodeLabel);

    /* Remember the current code position. We will move the switch code
    ** to this position later.
    */
    GetCodePos (&CaseCodeStart);

    /* Setup the control structure, save the old and activate the new one */
    SwitchData.Nodes        = NewCollection ();
    SwitchData.ExprType     = UnqualifiedType (SwitchExpr.Type[0].C);
    SwitchData.Depth        = SizeOf (SwitchExpr.Type);
    SwitchData.DefaultLabel = 0;
    OldSwitch = Switch;
    Switch = &SwitchData;

    /* Get the exit label for the switch statement */
    ExitLabel = GetLocalLabel ();

    /* Create a loop so we may use break. */
    AddLoop (ExitLabel, 0);

    /* Make sure a curly brace follows */
    if (CurTok.Tok != TOK_LCURLY) {
        Error ("`{' expected");
    }

    /* Parse the following statement, which will actually be a compound
    ** statement because of the curly brace at the current input position
    */
    HaveBreak = Statement (&RCurlyBrace);

    /* Check if we had any labels */
    if (CollCount (SwitchData.Nodes) == 0 && SwitchData.DefaultLabel == 0) {
        Warning ("No case labels");
    }

    /* If the last statement did not have a break, we may have an open
    ** label (maybe from an if or similar). Emitting code and then moving
    ** this code to the top will also move the label to the top which is
    ** wrong. So if the last statement did not have a break (which would
    ** carry the label), add a jump to the exit. If it is useless, the
    ** optimizer will remove it later.
    */
    if (!HaveBreak) {
        g_jump (ExitLabel);
    }

    /* Remember the current position */
    GetCodePos (&SwitchCodeStart);

    /* Output the switch code label */
    g_defcodelabel (SwitchCodeLabel);

    /* Generate code */
    if (SwitchData.DefaultLabel == 0) {
        /* No default label, use switch exit */
        SwitchData.DefaultLabel = ExitLabel;
    }
    g_switch (SwitchData.Nodes, SwitchData.DefaultLabel, SwitchData.Depth);

    /* Move the code to the front */
    GetCodePos (&SwitchCodeEnd);
    MoveCode (&SwitchCodeStart, &SwitchCodeEnd, &CaseCodeStart);

    /* Define the exit label */
    g_defcodelabel (ExitLabel);

    /* Exit the loop */
    DelLoop ();

    /* Switch back to the enclosing switch statement if any */
    Switch = OldSwitch;

    /* Free the case value tree */
    FreeCaseNodeColl (SwitchData.Nodes);

    /* If the case statement was (correctly) terminated by a closing curly
    ** brace, skip it now.
    */
    if (RCurlyBrace) {
        NextToken ();
    }
}



void CaseLabel (void)
/* Handle a case sabel */
{
    ExprDesc CaseExpr;          /* Case label expression */
    long     Val;               /* Case label value */
    unsigned CodeLabel;         /* Code label for this case */


    /* Skip the "case" token */
    NextToken ();

    /* Read the selector expression */
    ConstAbsIntExpr (hie1, &CaseExpr);
    Val = CaseExpr.IVal;

    /* Now check if we're inside a switch statement */
    if (Switch != 0) {

        /* Check the range of the expression */
        switch (Switch->ExprType) {

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
                Internal ("Invalid type: %06lX", Switch->ExprType);
        }

        /* Insert the case selector into the selector table */
        CodeLabel = InsertCaseValue (Switch->Nodes, Val, Switch->Depth);

        /* Define this label */
        g_defcodelabel (CodeLabel);

    } else {

        /* case keyword outside a switch statement */
        Error ("Case label not within a switch statement");

    }

    /* Skip the colon */
    ConsumeColon ();
}



void DefaultLabel (void)
/* Handle a default label */
{
    /* Default case */
    NextToken ();

    /* Now check if we're inside a switch statement */
    if (Switch != 0) {

        /* Check if we do already have a default branch */
        if (Switch->DefaultLabel == 0) {

            /* Generate and emit the default label */
            Switch->DefaultLabel = GetLocalLabel ();
            g_defcodelabel (Switch->DefaultLabel);

        } else {
            /* We had the default label already */
            Error ("Multiple default labels in one switch");
        }

    } else {

        /* case keyword outside a switch statement */
        Error ("`default' label not within a switch statement");

    }

    /* Skip the colon */
    ConsumeColon ();
}
