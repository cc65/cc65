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
    const Type* ExprType;       /* Switch controlling expression type */
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

    ED_Init (&SwitchExpr);
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
    SwitchData.ExprType     = SwitchExpr.Type;
    SwitchData.Depth        = SizeOf (SwitchExpr.Type);
    SwitchData.DefaultLabel = 0;
    OldSwitch = Switch;
    Switch = &SwitchData;

    /* Get the exit label for the switch statement */
    ExitLabel = GetLocalLabel ();

    /* Create a loop so we may use break. */
    AddLoop (ExitLabel, 0);

    /* Parse the following statement, which may actually be a compound
    ** statement if there is a curly brace at the current input position
    */
    HaveBreak = AnyStatement (&RCurlyBrace);

    /* Check if we had any labels */
    if (CollCount (SwitchData.Nodes) == 0 && SwitchData.DefaultLabel == 0) {
        Warning ("No reachable case labels for switch");
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

    /* If the case statement was terminated by a closing curly
    ** brace, skip it now.
    */
    if (RCurlyBrace) {
        NextToken ();
    }
}



void CaseLabel (void)
/* Handle a case label */
{
    ExprDesc CaseExpr;          /* Case label expression */

    /* Skip the "case" token */
    NextToken ();

    /* Read the selector expression */
    CaseExpr = NoCodeConstAbsIntExpr (hie1);

    /* Now check if we're inside a switch statement */
    if (Switch != 0) {

        /* Check the range of the expression */
        const Type* CaseT       = CaseExpr.Type;
        long        CaseVal     = CaseExpr.IVal;
        int         OutOfRange  = 0;
        const char* DiagMsg     = 0;

        CaseExpr.Type = IntPromotion (Switch->ExprType);
        LimitExprValue (&CaseExpr, 1);

        if (CaseVal != CaseExpr.IVal ||
            (IsSignSigned (CaseT) != IsSignSigned (CaseExpr.Type) &&
            (IsSignSigned (CaseT) ? CaseVal < 0 : CaseExpr.IVal < 0))) {
            Warning (IsSignSigned (CaseT) ?
                     IsSignSigned (CaseExpr.Type) ?
                     "Case value is implicitly converted (%ld to %ld)" :
                     "Case value is implicitly converted (%ld to %lu)" :
                     IsSignSigned (CaseExpr.Type) ?
                     "Case value is implicitly converted (%lu to %ld)" :
                     "Case value is implicitly converted (%lu to %lu)",
                     CaseVal, CaseExpr.IVal);
        }

        /* Check the range of the expression */
        if (IsSignSigned (CaseExpr.Type)) {
            if (CaseExpr.IVal < GetIntegerTypeMin (Switch->ExprType)) {
                DiagMsg = "Case value (%ld) out of range for switch condition type";
                OutOfRange = 1;
            } else if (IsSignSigned (Switch->ExprType) ?
                       CaseExpr.IVal > (long)GetIntegerTypeMax (Switch->ExprType) :
                       SizeOf (CaseExpr.Type) > SizeOf (Switch->ExprType) &&
                       (unsigned long)CaseExpr.IVal > GetIntegerTypeMax (Switch->ExprType)) {
                DiagMsg = "Case value (%ld) out of range for switch condition type";
                OutOfRange = 1;
            }
        } else if ((unsigned long)CaseExpr.IVal > GetIntegerTypeMax (Switch->ExprType)) {
            DiagMsg = "Case value (%lu) out of range for switch condition type";
            OutOfRange = 1;
        }

        if (OutOfRange == 0) {
            /* Insert the case selector into the selector table */
            unsigned CodeLabel = InsertCaseValue (Switch->Nodes, CaseExpr.IVal, Switch->Depth);

            /* Define this label */
            g_defcodelabel (CodeLabel);
        } else {
            Warning (DiagMsg, CaseExpr.IVal);
        }

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
        Error ("'default' label not within a switch statement");

    }

    /* Skip the colon */
    ConsumeColon ();
}
