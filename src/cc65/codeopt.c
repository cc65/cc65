/*****************************************************************************/
/*                                                                           */
/*                                 codeopt.c                                 */
/*                                                                           */
/*                           Optimizer subroutines                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2012, Ullrich von Bassewitz                                      */
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



#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* common */
#include "abend.h"
#include "chartype.h"
#include "cpu.h"
#include "debugflag.h"
#include "print.h"
#include "strbuf.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "codeopt.h"
#include "coptadd.h"
#include "coptbool.h"
#include "coptc02.h"
#include "coptcmp.h"
#include "coptind.h"
#include "coptjmp.h"
#include "coptlong.h"
#include "coptmisc.h"
#include "coptptrload.h"
#include "coptptrstore.h"
#include "coptpush.h"
#include "coptshift.h"
#include "coptsize.h"
#include "coptstop.h"
#include "coptstore.h"
#include "coptsub.h"
#include "copttest.h"
#include "coptunary.h"
#include "error.h"
#include "global.h"
#include "output.h"



/*****************************************************************************/
/*                              struct OptFunc                               */
/*****************************************************************************/



typedef struct OptFunc OptFunc;
struct OptFunc {
    unsigned       (*Func) (CodeSeg*);  /* Optimizer function */
    const char*    Name;                /* Name of the function/group */
    unsigned       CodeSizeFactor;      /* Code size factor for this opt func */
    unsigned long  TotalRuns;           /* Total number of runs */
    unsigned long  LastRuns;            /* Last number of runs */
    unsigned long  TotalChanges;        /* Total number of changes */
    unsigned long  LastChanges;         /* Last number of changes */
    char           Disabled;            /* True if function disabled */
};

/* Optimizer step definition ("D" name prefix). */
#define OPTFUNCDEF(name, codesize)      \
    static OptFunc D##name = { name, #name, codesize, 0, 0, 0, 0, 0 }



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* A list of all the function descriptions */
/* CAUTION: should be sorted by "name" */
/* BEGIN DECL SORTED_CODEOPT.SH */
OPTFUNCDEF ( Opt65C02BitOps,              66 );
OPTFUNCDEF ( Opt65C02Ind,                100 );
OPTFUNCDEF ( Opt65C02Stores,             100 );
OPTFUNCDEF ( OptAXLoad,                   50 );
OPTFUNCDEF ( OptAXLoad2,                  66 );
OPTFUNCDEF ( OptAXOps,                    50 );
OPTFUNCDEF ( OptAdd1,                    125 );
OPTFUNCDEF ( OptAdd2,                    200 );
OPTFUNCDEF ( OptAdd3,                     65 );
OPTFUNCDEF ( OptAdd4,                     90 );
OPTFUNCDEF ( OptAdd5,                    100 );
OPTFUNCDEF ( OptAdd6,                     40 );
OPTFUNCDEF ( OptBNegA1,                  100 );
OPTFUNCDEF ( OptBNegA2,                  100 );
OPTFUNCDEF ( OptBNegAX1,                 100 );
OPTFUNCDEF ( OptBNegAX2,                 100 );
OPTFUNCDEF ( OptBNegAX3,                 100 );
OPTFUNCDEF ( OptBNegAX4,                 100 );
OPTFUNCDEF ( OptBinOps1,                   0 );
OPTFUNCDEF ( OptBinOps2,                   0 );
OPTFUNCDEF ( OptBoolCmp,                 100 );
OPTFUNCDEF ( OptBoolTrans,               100 );
OPTFUNCDEF ( OptBoolUnary1,               40 );
OPTFUNCDEF ( OptBoolUnary2,               40 );
OPTFUNCDEF ( OptBoolUnary3,               40 );
OPTFUNCDEF ( OptBranchDist,                0 );
OPTFUNCDEF ( OptBranchDist2,               0 );
OPTFUNCDEF ( OptCmp1,                     42 );
OPTFUNCDEF ( OptCmp2,                     85 );
OPTFUNCDEF ( OptCmp3,                     75 );
OPTFUNCDEF ( OptCmp4,                     75 );
OPTFUNCDEF ( OptCmp5,                    100 );
OPTFUNCDEF ( OptCmp6,                     33 );
OPTFUNCDEF ( OptCmp7,                     85 );
OPTFUNCDEF ( OptCmp8,                     50 );
OPTFUNCDEF ( OptCmp9,                     85 );
OPTFUNCDEF ( OptComplAX1,                 65 );
OPTFUNCDEF ( OptCondBranch1,              80 );
OPTFUNCDEF ( OptCondBranch2,              40 );
OPTFUNCDEF ( OptCondBranch3,              40 );
OPTFUNCDEF ( OptCondBranchC,               0 );
OPTFUNCDEF ( OptDeadCode,                100 );
OPTFUNCDEF ( OptDeadJumps,               100 );
OPTFUNCDEF ( OptDecouple,                100 );
OPTFUNCDEF ( OptDupLoads,                  0 );
OPTFUNCDEF ( OptGotoSPAdj,                 0 );
OPTFUNCDEF ( OptIndLoads1,                 0 );
OPTFUNCDEF ( OptIndLoads2,                 0 );
OPTFUNCDEF ( OptJumpCascades,            100 );
OPTFUNCDEF ( OptJumpTarget1,             100 );
OPTFUNCDEF ( OptJumpTarget2,             100 );
OPTFUNCDEF ( OptJumpTarget3,             100 );
OPTFUNCDEF ( OptLoad1,                   100 );
OPTFUNCDEF ( OptLoad2,                   200 );
OPTFUNCDEF ( OptLoad3,                     0 );
OPTFUNCDEF ( OptLoadStore1,                0 );
OPTFUNCDEF ( OptLoadStore2,                0 );
OPTFUNCDEF ( OptLoadStoreLoad,             0 );
OPTFUNCDEF ( OptLongAssign,              100 );
OPTFUNCDEF ( OptLongCopy,                100 );
OPTFUNCDEF ( OptNegAX1,                  165 );
OPTFUNCDEF ( OptNegAX2,                  200 );
OPTFUNCDEF ( OptPrecalc,                 100 );
OPTFUNCDEF ( OptPtrLoad1,                100 );
OPTFUNCDEF ( OptPtrLoad11,                92 );
OPTFUNCDEF ( OptPtrLoad12,                50 );
OPTFUNCDEF ( OptPtrLoad13,                65 );
OPTFUNCDEF ( OptPtrLoad14,               108 );
OPTFUNCDEF ( OptPtrLoad15,                86 );
OPTFUNCDEF ( OptPtrLoad16,               100 );
OPTFUNCDEF ( OptPtrLoad17,               190 );
OPTFUNCDEF ( OptPtrLoad18,               100 );
OPTFUNCDEF ( OptPtrLoad19,                65 );
OPTFUNCDEF ( OptPtrLoad2,                100 );
OPTFUNCDEF ( OptPtrLoad20,                90 );
OPTFUNCDEF ( OptPtrLoad3,                100 );
OPTFUNCDEF ( OptPtrLoad4,                100 );
OPTFUNCDEF ( OptPtrLoad5,                 50 );
OPTFUNCDEF ( OptPtrLoad6,                 60 );
OPTFUNCDEF ( OptPtrLoad7,                140 );
OPTFUNCDEF ( OptPtrStore1,                65 );
OPTFUNCDEF ( OptPtrStore2,                65 );
OPTFUNCDEF ( OptPtrStore3,               100 );
OPTFUNCDEF ( OptPush1,                    65 );
OPTFUNCDEF ( OptPush2,                    50 );
OPTFUNCDEF ( OptPushPop1,                  0 );
OPTFUNCDEF ( OptPushPop2,                  0 );
OPTFUNCDEF ( OptPushPop3,                  0 );
OPTFUNCDEF ( OptRTS,                     100 );
OPTFUNCDEF ( OptRTSJumps1,               100 );
OPTFUNCDEF ( OptRTSJumps2,               100 );
OPTFUNCDEF ( OptShift1,                  100 );
OPTFUNCDEF ( OptShift2,                  100 );
OPTFUNCDEF ( OptShift3,                   17 );
OPTFUNCDEF ( OptShift4,                  100 );
OPTFUNCDEF ( OptShift5,                  110 );
OPTFUNCDEF ( OptShift6,                  200 );
OPTFUNCDEF ( OptShiftBack,                 0 );
OPTFUNCDEF ( OptSignExtended,              0 );
OPTFUNCDEF ( OptSize1,                   100 );
OPTFUNCDEF ( OptSize2,                   100 );
OPTFUNCDEF ( OptStackOps,                100 );
OPTFUNCDEF ( OptStackPtrOps,              50 );
OPTFUNCDEF ( OptStore1,                   70 );
OPTFUNCDEF ( OptStore2,                  115 );
OPTFUNCDEF ( OptStore3,                  120 );
OPTFUNCDEF ( OptStore4,                   50 );
OPTFUNCDEF ( OptStore5,                  100 );
OPTFUNCDEF ( OptStoreLoad,                 0 );
OPTFUNCDEF ( OptSub1,                    100 );
OPTFUNCDEF ( OptSub2,                    100 );
OPTFUNCDEF ( OptSub3,                    100 );
OPTFUNCDEF ( OptTest1,                    65 );
OPTFUNCDEF ( OptTest2,                    50 );
OPTFUNCDEF ( OptTosLoadPop,               50 );
OPTFUNCDEF ( OptTosPushPop,               33 );
OPTFUNCDEF ( OptTransfers1,                0 );
OPTFUNCDEF ( OptTransfers2,               60 );
OPTFUNCDEF ( OptTransfers3,               65 );
OPTFUNCDEF ( OptTransfers4,               65 );
OPTFUNCDEF ( OptUnusedLoads,               0 );
OPTFUNCDEF ( OptUnusedStores,              0 );
/* END DECL SORTED_CODEOPT.SH */


/* Table containing all the steps in alphabetical order */
/* CAUTION: table must be sorted for bsearch */
static OptFunc* OptFuncs[] = {
/* BEGIN SORTED_CODEOPT.SH */
    &DOpt65C02BitOps,
    &DOpt65C02Ind,
    &DOpt65C02Stores,
    &DOptAXLoad,
    &DOptAXLoad2,
    &DOptAXOps,
    &DOptAdd1,
    &DOptAdd2,
    &DOptAdd3,
    &DOptAdd4,
    &DOptAdd5,
    &DOptAdd6,
    &DOptBNegA1,
    &DOptBNegA2,
    &DOptBNegAX1,
    &DOptBNegAX2,
    &DOptBNegAX3,
    &DOptBNegAX4,
    &DOptBinOps1,
    &DOptBinOps2,
    &DOptBoolCmp,
    &DOptBoolTrans,
    &DOptBoolUnary1,
    &DOptBoolUnary2,
    &DOptBoolUnary3,
    &DOptBranchDist,
    &DOptBranchDist2,
    &DOptCmp1,
    &DOptCmp2,
    &DOptCmp3,
    &DOptCmp4,
    &DOptCmp5,
    &DOptCmp6,
    &DOptCmp7,
    &DOptCmp8,
    &DOptCmp9,
    &DOptComplAX1,
    &DOptCondBranch1,
    &DOptCondBranch2,
    &DOptCondBranch3,
    &DOptCondBranchC,
    &DOptDeadCode,
    &DOptDeadJumps,
    &DOptDecouple,
    &DOptDupLoads,
    &DOptGotoSPAdj,
    &DOptIndLoads1,
    &DOptIndLoads2,
    &DOptJumpCascades,
    &DOptJumpTarget1,
    &DOptJumpTarget2,
    &DOptJumpTarget3,
    &DOptLoad1,
    &DOptLoad2,
    &DOptLoad3,
    &DOptLoadStore1,
    &DOptLoadStore2,
    &DOptLoadStoreLoad,
    &DOptLongAssign,
    &DOptLongCopy,
    &DOptNegAX1,
    &DOptNegAX2,
    &DOptPrecalc,
    &DOptPtrLoad1,
    &DOptPtrLoad11,
    &DOptPtrLoad12,
    &DOptPtrLoad13,
    &DOptPtrLoad14,
    &DOptPtrLoad15,
    &DOptPtrLoad16,
    &DOptPtrLoad17,
    &DOptPtrLoad18,
    &DOptPtrLoad19,
    &DOptPtrLoad2,
    &DOptPtrLoad20,
    &DOptPtrLoad3,
    &DOptPtrLoad4,
    &DOptPtrLoad5,
    &DOptPtrLoad6,
    &DOptPtrLoad7,
    &DOptPtrStore1,
    &DOptPtrStore2,
    &DOptPtrStore3,
    &DOptPush1,
    &DOptPush2,
    &DOptPushPop1,
    &DOptPushPop2,
    &DOptPushPop3,
    &DOptRTS,
    &DOptRTSJumps1,
    &DOptRTSJumps2,
    &DOptShift1,
    &DOptShift2,
    &DOptShift3,
    &DOptShift4,
    &DOptShift5,
    &DOptShift6,
    &DOptShiftBack,
    &DOptSignExtended,
    &DOptSize1,
    &DOptSize2,
    &DOptStackOps,
    &DOptStackPtrOps,
    &DOptStore1,
    &DOptStore2,
    &DOptStore3,
    &DOptStore4,
    &DOptStore5,
    &DOptStoreLoad,
    &DOptSub1,
    &DOptSub2,
    &DOptSub3,
    &DOptTest1,
    &DOptTest2,
    &DOptTosLoadPop,
    &DOptTosPushPop,
    &DOptTransfers1,
    &DOptTransfers2,
    &DOptTransfers3,
    &DOptTransfers4,
    &DOptUnusedLoads,
    &DOptUnusedStores,
/* END SORTED_CODEOPT.SH */
};
#define OPTFUNC_COUNT  (sizeof(OptFuncs) / sizeof(OptFuncs[0]))



static int CmpOptStep (const void* Key, const void* Func)
/* Compare function for bsearch */
{
    return strcmp (Key, (*(const OptFunc**)Func)->Name);
}



static OptFunc* FindOptFunc (const char* Name)
/* Find an optimizer step by name in the table and return a pointer. Return
** NULL if no such step is found.
*/
{
    /* Search for the function in the list */
    OptFunc** O = bsearch (Name, OptFuncs, OPTFUNC_COUNT, sizeof (OptFuncs[0]), CmpOptStep);
    return O? *O : 0;
}



static OptFunc* GetOptFunc (const char* Name)
/* Find an optimizer step by name in the table and return a pointer. Print an
** error and call AbEnd if not found.
*/
{
    /* Search for the function in the list */
    OptFunc* F = FindOptFunc (Name);
    if (F == 0) {
        /* Not found */
        AbEnd ("Optimization step '%s' not found", Name);
    }
    return F;
}



void DisableOpt (const char* Name)
/* Disable the optimization with the given name */
{
    if (strcmp (Name, "any") == 0) {
        unsigned I;
        for (I = 0; I < OPTFUNC_COUNT; ++I) {
            OptFuncs[I]->Disabled = 1;
        }
    } else {
        GetOptFunc(Name)->Disabled = 1;
    }
}



void EnableOpt (const char* Name)
/* Enable the optimization with the given name */
{
    if (strcmp (Name, "any") == 0) {
        unsigned I;
        for (I = 0; I < OPTFUNC_COUNT; ++I) {
            OptFuncs[I]->Disabled = 0;
        }
    } else {
        GetOptFunc(Name)->Disabled = 0;
    }
}



void ListOptSteps (FILE* F)
/* List all optimization steps */
{
    unsigned I;

    fprintf (F, "any\n");
    for (I = 0; I < OPTFUNC_COUNT; ++I) {
        if (OptFuncs[I]->Func != 0) {
            fprintf (F, "%s\n", OptFuncs[I]->Name);
        }
    }
}



static void ReadOptStats (const char* Name)
/* Read the optimizer statistics file */
{
    char Buf [256];

    /* Try to open the file */
    FILE* F = fopen (Name, "r");
    if (F == 0) {
        /* Ignore the error */
        return;
    }

    /* Read and parse the lines */
    while (fgets (Buf, sizeof (Buf), F) != 0) {

        char* B;
        unsigned Len;
        OptFunc* Func;

        /* Fields */
        char Name[32];
        unsigned long  TotalRuns;
        unsigned long  TotalChanges;

        /* Remove trailing white space including the line terminator */
        B = Buf;
        Len = strlen (B);
        while (Len > 0 && IsSpace (B[Len-1])) {
            --Len;
        }
        B[Len] = '\0';

        /* Remove leading whitespace */
        while (IsSpace (*B)) {
            ++B;
        }

        /* Check for empty and comment lines */
        if (*B == '\0' || *B == ';' || *B == '#') {
            continue;
        }

        /* Parse the line */
        if (sscanf (B, "%31s %lu %*u %lu %*u", Name, &TotalRuns, &TotalChanges) != 3) {
            /* Syntax error */
            continue;
        }

        /* Search for the optimizer step. */
        Func = FindOptFunc (Name);
        if (Func == 0) {
            /* Not found */
            continue;
        }

        /* Found the step, set the fields */
        Func->TotalRuns    = TotalRuns;
        Func->TotalChanges = TotalChanges;

    }

    /* Close the file, ignore errors here. */
    fclose (F);
}



static void WriteOptStats (const char* Name)
/* Write the optimizer statistics file */
{
    unsigned I;

    /* Try to open the file */
    FILE* F = fopen (Name, "w");
    if (F == 0) {
        /* Ignore the error */
        return;
    }

    /* Write a header */
    fprintf (F,
             "; Optimizer               Total      Last       Total      Last\n"
             ";   Step                  Runs       Runs        Chg       Chg\n");


    /* Write the data */
    for (I = 0; I < OPTFUNC_COUNT; ++I) {
        const OptFunc* O = OptFuncs[I];
        fprintf (F,
                 "%-20s %10lu %10lu %10lu %10lu\n",
                 O->Name,
                 O->TotalRuns,
                 O->LastRuns,
                 O->TotalChanges,
                 O->LastChanges);
    }

    /* Close the file, ignore errors here. */
    fclose (F);
}



static void OpenDebugFile (const CodeSeg* S)
/* Open the debug file for the given segment if the flag is on */
{
    if (DebugOptOutput) {
        StrBuf Name = AUTO_STRBUF_INITIALIZER;
        if (S->Func) {
            SB_CopyStr (&Name, S->Func->Name);
        } else {
            SB_CopyStr (&Name, "global");
        }
        SB_AppendStr (&Name, ".opt");
        SB_Terminate (&Name);
        OpenDebugOutputFile (SB_GetConstBuf (&Name));
        SB_Done (&Name);
    }
}



static void WriteDebugOutput (CodeSeg* S, const char* Step)
/* Write a separator line into the debug file if the flag is on */
{
    if (DebugOptOutput) {
        /* Output a separator */
        WriteOutput ("=========================================================================\n");

        /* Output a header line */
        if (Step == 0) {
            /* Initial output */
            WriteOutput ("Initial code for function '%s':\n",
                         S->Func? S->Func->Name : "<global>");
        } else {
            WriteOutput ("Code after applying '%s':\n", Step);
        }

        /* Output the code segment */
        CS_Output (S);
    }
}



static unsigned RunOptFunc (CodeSeg* S, OptFunc* F, unsigned Max)
/* Run one optimizer function Max times or until there are no more changes */
{
    unsigned Changes, C;

    /* Don't run the function if it is removed, disabled or prohibited by the
    ** code size factor
    */
    if (F->Func == 0 || F->Disabled || F->CodeSizeFactor > S->CodeSizeFactor) {
        return 0;
    }

    /* Run this until there are no more changes */
    Changes = 0;
    do {

        /* Run the function */
        C = F->Func (S);
        Changes += C;

        /* Do statistics */
        ++F->TotalRuns;
        ++F->LastRuns;
        F->TotalChanges += C;
        F->LastChanges  += C;

        /* If we had changes, output stuff and regenerate register info */
        if (C) {
            if (Debug) {
                printf ("Applied %s: %u changes\n", F->Name, C);
            }
            WriteDebugOutput (S, F->Name);
            CS_GenRegInfo (S);
        }

    } while (--Max && C > 0);

    /* Return the number of changes */
    return Changes;
}



static unsigned RunOptGroup1 (CodeSeg* S)
/* Run the first group of optimization steps. These steps translate known
** patterns emitted by the code generator into more optimal patterns. Order
** of the steps is important, because some of the steps done earlier cover
** the same patterns as later steps as subpatterns.
*/
{
    unsigned Changes = 0;

    Changes += RunOptFunc (S, &DOptGotoSPAdj, 1);
    Changes += RunOptFunc (S, &DOptStackPtrOps, 5);
    Changes += RunOptFunc (S, &DOptTosLoadPop, 5);
    Changes += RunOptFunc (S, &DOptAXOps, 5);
    Changes += RunOptFunc (S, &DOptAdd3, 1);    /* Before OptPtrLoad5! */
    Changes += RunOptFunc (S, &DOptPtrStore1, 1);
    Changes += RunOptFunc (S, &DOptPtrStore2, 1);
    Changes += RunOptFunc (S, &DOptPtrStore3, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad1, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad2, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad3, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad4, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad5, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad6, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad18, 1); /* Before OptPtrLoad7 */
    Changes += RunOptFunc (S, &DOptPtrLoad19, 1); /* Before OptPtrLoad7 */
    Changes += RunOptFunc (S, &DOptPtrLoad7, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad11, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad12, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad13, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad14, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad15, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad16, 1);
    Changes += RunOptFunc (S, &DOptPtrLoad17, 1);
    Changes += RunOptFunc (S, &DOptAdd1, 1);
    Changes += RunOptFunc (S, &DOptAdd2, 1);
    Changes += RunOptFunc (S, &DOptAdd4, 1);
    Changes += RunOptFunc (S, &DOptAdd5, 1);
    Changes += RunOptFunc (S, &DOptAdd6, 1);
    Changes += RunOptFunc (S, &DOptSub1, 1);
    Changes += RunOptFunc (S, &DOptSub3, 1);
    Changes += RunOptFunc (S, &DOptLongAssign, 1);
    Changes += RunOptFunc (S, &DOptLoadStore2, 1);
    Changes += RunOptFunc (S, &DOptStore4, 1);
    Changes += RunOptFunc (S, &DOptStore5, 1);
    Changes += RunOptFunc (S, &DOptShift1, 1);
    Changes += RunOptFunc (S, &DOptShift2, 1);
    Changes += RunOptFunc (S, &DOptShift5, 1);
    Changes += RunOptFunc (S, &DOptShift6, 1);
    Changes += RunOptFunc (S, &DOptStore1, 1);
    Changes += RunOptFunc (S, &DOptStore2, 5);
    Changes += RunOptFunc (S, &DOptStore3, 5);
    Changes += RunOptFunc (S, &DOptLongCopy, 1);

    /* Return the number of changes */
    return Changes;
}



static unsigned RunOptGroup2 (CodeSeg* S)
/* Run one group of optimization steps. This step involves just decoupling
** instructions by replacing them by instructions that do not depend on
** previous instructions. This makes it easier to find instructions that
** aren't used.
*/
{
    unsigned Changes = 0;

    Changes += RunOptFunc (S, &DOptDecouple, 1);

    /* Return the number of changes */
    return Changes;
}



static unsigned RunOptGroup3 (CodeSeg* S)
/* Run one group of optimization steps. These steps depend on each other,
** that means that one step may allow another step to do additional work,
** so we will repeat the steps as long as we see any changes.
*/
{
    unsigned Changes, C;

    Changes = 0;
    do {
        C = 0;

        C += RunOptFunc (S, &DOptNegAX1, 1);
        C += RunOptFunc (S, &DOptNegAX2, 1);
        C += RunOptFunc (S, &DOptStackOps, 3);      /* Before OptBoolUnary1 */
        C += RunOptFunc (S, &DOptCmp8, 1);          /* Before OptBoolUnary1 */
        C += RunOptFunc (S, &DOptBoolUnary1, 3);
        C += RunOptFunc (S, &DOptBoolUnary2, 3);
        C += RunOptFunc (S, &DOptBoolUnary3, 1);
        C += RunOptFunc (S, &DOptBNegA1, 1);
        C += RunOptFunc (S, &DOptBNegAX1, 1);       /* After OptBoolUnary2 */
        C += RunOptFunc (S, &DOptShift1, 1);
        C += RunOptFunc (S, &DOptShift4, 1);
        C += RunOptFunc (S, &DOptComplAX1, 1);
        C += RunOptFunc (S, &DOptSub1, 1);
        C += RunOptFunc (S, &DOptSub2, 1);
        C += RunOptFunc (S, &DOptSub3, 1);
        C += RunOptFunc (S, &DOptAdd5, 1);
        C += RunOptFunc (S, &DOptAdd6, 1);
        C += RunOptFunc (S, &DOptJumpCascades, 1);
        C += RunOptFunc (S, &DOptDeadJumps, 1);
        C += RunOptFunc (S, &DOptDeadCode, 1);
        C += RunOptFunc (S, &DOptJumpTarget1, 1);
        C += RunOptFunc (S, &DOptJumpTarget2, 1);
        C += RunOptFunc (S, &DOptCondBranch1, 1);
        C += RunOptFunc (S, &DOptCondBranch2, 1);
        C += RunOptFunc (S, &DOptCondBranch3, 1);
        C += RunOptFunc (S, &DOptCondBranchC, 1);
        C += RunOptFunc (S, &DOptRTSJumps1, 1);
        C += RunOptFunc (S, &DOptCmp6, 1);          /* After OptRTSJumps1 */
        C += RunOptFunc (S, &DOptBoolCmp, 1);
        C += RunOptFunc (S, &DOptBoolTrans, 1);
        C += RunOptFunc (S, &DOptBNegA2, 1);        /* After OptCondBranch's */
        C += RunOptFunc (S, &DOptBNegAX2, 1);       /* After OptCondBranch's */
        C += RunOptFunc (S, &DOptBNegAX3, 1);       /* After OptCondBranch's */
        C += RunOptFunc (S, &DOptBNegAX4, 1);       /* After OptCondBranch's */
        C += RunOptFunc (S, &DOptCmp1, 1);
        C += RunOptFunc (S, &DOptCmp2, 1);
        C += RunOptFunc (S, &DOptCmp8, 1);          /* Must run before OptCmp3 */
        C += RunOptFunc (S, &DOptCmp3, 1);
        C += RunOptFunc (S, &DOptCmp4, 1);
        C += RunOptFunc (S, &DOptCmp5, 1);
        C += RunOptFunc (S, &DOptCmp7, 1);
        C += RunOptFunc (S, &DOptCmp9, 1);
        C += RunOptFunc (S, &DOptTest1, 1);
        C += RunOptFunc (S, &DOptLoad1, 1);
        C += RunOptFunc (S, &DOptJumpTarget3, 1);   /* After OptCondBranches2 */
        C += RunOptFunc (S, &DOptUnusedLoads, 1);
        C += RunOptFunc (S, &DOptUnusedStores, 1);
        C += RunOptFunc (S, &DOptStoreLoad, 1);
        C += RunOptFunc (S, &DOptLoadStoreLoad, 1);
        C += RunOptFunc (S, &DOptDupLoads, 1);
        C += RunOptFunc (S, &DOptLoadStore1, 1);
        C += RunOptFunc (S, &DOptTransfers1, 1);
        C += RunOptFunc (S, &DOptTransfers3, 1);
        C += RunOptFunc (S, &DOptTransfers4, 1);
        C += RunOptFunc (S, &DOptStore1, 1);
        C += RunOptFunc (S, &DOptStore5, 1);
        C += RunOptFunc (S, &DOptPushPop1, 1);
        C += RunOptFunc (S, &DOptPushPop2, 1);
        C += RunOptFunc (S, &DOptPushPop3, 1);
        C += RunOptFunc (S, &DOptPrecalc, 1);
        C += RunOptFunc (S, &DOptShiftBack, 1);
        C += RunOptFunc (S, &DOptSignExtended, 1);
        C += RunOptFunc (S, &DOptBinOps1, 1);
        C += RunOptFunc (S, &DOptBinOps2, 1);

        Changes += C;

    } while (C);

    /* Return the number of changes */
    return Changes;
}



static unsigned RunOptGroup4 (CodeSeg* S)
/* Run another round of pattern replacements. These are done late, since there
** may be better replacements before.
*/
{
    unsigned Changes = 0;

    /* Repeat some of the steps here */
    Changes += RunOptFunc (S, &DOptShift3, 1);
    Changes += RunOptFunc (S, &DOptPush1, 1);
    Changes += RunOptFunc (S, &DOptPush2, 1);
    Changes += RunOptFunc (S, &DOptUnusedLoads, 1);
    Changes += RunOptFunc (S, &DOptTest2, 1);
    Changes += RunOptFunc (S, &DOptTransfers2, 1);
    Changes += RunOptFunc (S, &DOptLoad2, 1);
    Changes += RunOptFunc (S, &DOptLoad3, 1);
    Changes += RunOptFunc (S, &DOptDupLoads, 1);

    /* Return the number of changes */
    return Changes;
}



static unsigned RunOptGroup5 (CodeSeg* S)
/* 65C02 specific optimizations. */
{
    unsigned Changes = 0;

    if (CPUIsets[CPU] & CPU_ISET_65SC02) {
        Changes += RunOptFunc (S, &DOpt65C02BitOps, 1);
        Changes += RunOptFunc (S, &DOpt65C02Ind, 1);
        Changes += RunOptFunc (S, &DOpt65C02Stores, 1);
        if (Changes) {
            /* The 65C02 replacement codes do often make the use of a register
            ** value unnecessary, so if we have changes, run another load
            ** removal pass.
            */
            Changes += RunOptFunc (S, &DOptUnusedLoads, 1);
        }
    }

    /* Return the number of changes */
    return Changes;
}



static unsigned RunOptGroup6 (CodeSeg* S)
/* This one is quite special. It tries to replace "lda (c_sp),y" by "lda (c_sp,x)".
** The latter is ony cycle slower, but if we're able to remove the necessary
** load of the Y register, because X is zero anyway, we gain 1 cycle and
** shorten the code by one (transfer) or two bytes (load). So what we do is
** to replace the insns, remove unused loads, and then change back all insns
** where Y is still zero (meaning that the load has not been removed).
*/
{
    unsigned Changes = 0;

    /* This group will only run for a standard 6502, because the 65C02 has a
    ** better addressing mode that covers this case.
    */
    if ((CPUIsets[CPU] & CPU_ISET_65SC02) == 0) {
        Changes += RunOptFunc (S, &DOptIndLoads1, 1);
        Changes += RunOptFunc (S, &DOptUnusedLoads, 1);
        Changes += RunOptFunc (S, &DOptIndLoads2, 1);
    }

    /* Return the number of changes */
    return Changes;
}



static unsigned RunOptGroup7 (CodeSeg* S)
/* The last group of optimization steps. Adjust branches, do size optimizations.
*/
{
    unsigned Changes = 0;
    unsigned C;

    /* Optimize for size, that is replace operations by shorter ones, even
    ** if this does hinder further optimizations (no problem since we're
    ** done soon).
    */
    C = RunOptFunc (S, &DOptSize1, 1);
    if (C) {
        Changes += C;
        /* Run some optimization passes again, since the size optimizations
        ** may have opened new oportunities.
        */
        Changes += RunOptFunc (S, &DOptUnusedLoads, 1);
        Changes += RunOptFunc (S, &DOptAXLoad, 5);
        Changes += RunOptFunc (S, &DOptAXLoad2, 5);
        Changes += RunOptFunc (S, &DOptUnusedStores, 1);
        Changes += RunOptFunc (S, &DOptJumpTarget1, 5);
        Changes += RunOptFunc (S, &DOptStore5, 1);
        Changes += RunOptFunc (S, &DOptTransfers1, 1);
    }

    C = RunOptFunc (S, &DOptSize2, 1);
    if (C) {
        Changes += C;
        /* Run some optimization passes again, since the size optimizations
        ** may have opened new oportunities.
        */
        Changes += RunOptFunc (S, &DOptUnusedLoads, 1);
        Changes += RunOptFunc (S, &DOptJumpTarget1, 5);
        Changes += RunOptFunc (S, &DOptStore5, 1);
        Changes += RunOptFunc (S, &DOptTransfers1, 1);
        Changes += RunOptFunc (S, &DOptTransfers3, 1);
    }

    Changes += RunOptFunc (S, &DOptPtrLoad20, 1);

    /* Adjust branch distances */
    Changes += RunOptFunc (S, &DOptBranchDist, 3);

    /* Replace conditional branches to RTS */
    C = RunOptFunc (S, &DOptRTSJumps2, 1);

    /* Replace JSR followed by RTS to JMP */
    C += RunOptFunc (S, &DOptRTS, 1);

    /* Replace JMP/BRA to JMP by direct JMP */
    C += RunOptFunc (S, &DOptJumpCascades, 1);
    C += RunOptFunc (S, &DOptBranchDist2, 1);

    /* Adjust branch distances again, since the previous step may change code
       between branches */
    C += RunOptFunc (S, &DOptBranchDist, 3);

    /* Re-optimize inc/decsp that may now be grouped */
    C += RunOptFunc (S, &DOptStackPtrOps, 5);
    /* Re-optimize JSR/RTS that may now be grouped */
    C += RunOptFunc (S, &DOptRTS, 1);
    C += RunOptFunc (S, &DOptTosLoadPop, 5);
    C += RunOptFunc (S, &DOptTosPushPop, 5);

    Changes += C;
    /* If we had changes, we must run dead code elimination again,
    ** since the changes may have introduced dead code.
    */
    if (C) {
        Changes += RunOptFunc (S, &DOptDeadCode, 1);
    }

    /* Return the number of changes */
    return Changes;
}



void RunOpt (CodeSeg* S)
/* Run the optimizer */
{
    const char* StatFileName;

    /* If we shouldn't run the optimizer, bail out */
    if (!S->Optimize) {
        return;
    }

    /* Check if we are requested to write optimizer statistics */
    StatFileName = getenv ("CC65_OPTSTATS");
    if (StatFileName) {
        ReadOptStats (StatFileName);
    }

    /* Print the name of the function we are working on */
    if (S->Func) {
        Print (stdout, 1, "Running optimizer for function '%s'\n", S->Func->Name);
    } else {
        Print (stdout, 1, "Running optimizer for global code segment\n");
    }

    /* If requested, open an output file */
    OpenDebugFile (S);
    WriteDebugOutput (S, 0);

    /* Generate register info for all instructions */
    CS_GenRegInfo (S);

    /* Run groups of optimizations */
    RunOptGroup1 (S);
    RunOptGroup2 (S);
    RunOptGroup3 (S);
    RunOptGroup4 (S);
    RunOptGroup5 (S);
    RunOptGroup6 (S);
    RunOptGroup7 (S);

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Close output file if necessary */
    if (DebugOptOutput) {
        CloseOutputFile ();
    }

    /* Write statistics */
    if (StatFileName) {
        WriteOptStats (StatFileName);
    }
}
