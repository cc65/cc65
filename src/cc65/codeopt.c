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



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* A list of all the function descriptions */
static OptFunc DOpt65C02BitOps  = { Opt65C02BitOps,  "Opt65C02BitOps",   66, 0, 0, 0, 0, 0 };
static OptFunc DOpt65C02Ind     = { Opt65C02Ind,     "Opt65C02Ind",     100, 0, 0, 0, 0, 0 };
static OptFunc DOpt65C02Stores  = { Opt65C02Stores,  "Opt65C02Stores",  100, 0, 0, 0, 0, 0 };
static OptFunc DOptAdd1         = { OptAdd1,         "OptAdd1",         125, 0, 0, 0, 0, 0 };
static OptFunc DOptAdd2         = { OptAdd2,         "OptAdd2",         200, 0, 0, 0, 0, 0 };
static OptFunc DOptAdd3         = { OptAdd3,         "OptAdd3",          65, 0, 0, 0, 0, 0 };
static OptFunc DOptAdd4         = { OptAdd4,         "OptAdd4",          90, 0, 0, 0, 0, 0 };
static OptFunc DOptAdd5         = { OptAdd5,         "OptAdd5",         100, 0, 0, 0, 0, 0 };
static OptFunc DOptAdd6         = { OptAdd6,         "OptAdd6",          40, 0, 0, 0, 0, 0 };
static OptFunc DOptBNegA1       = { OptBNegA1,       "OptBNegA1",       100, 0, 0, 0, 0, 0 };
static OptFunc DOptBNegA2       = { OptBNegA2,       "OptBNegA2",       100, 0, 0, 0, 0, 0 };
static OptFunc DOptBNegAX1      = { OptBNegAX1,      "OptBNegAX1",      100, 0, 0, 0, 0, 0 };
static OptFunc DOptBNegAX2      = { OptBNegAX2,      "OptBNegAX2",      100, 0, 0, 0, 0, 0 };
static OptFunc DOptBNegAX3      = { OptBNegAX3,      "OptBNegAX3",      100, 0, 0, 0, 0, 0 };
static OptFunc DOptBNegAX4      = { OptBNegAX4,      "OptBNegAX4",      100, 0, 0, 0, 0, 0 };
static OptFunc DOptBoolCmp      = { OptBoolCmp,      "OptBoolCmp",      100, 0, 0, 0, 0, 0 };
static OptFunc DOptBoolTrans    = { OptBoolTrans,    "OptBoolTrans",    100, 0, 0, 0, 0, 0 };
static OptFunc DOptBoolUnary1   = { OptBoolUnary1,   "OptBoolUnary1",    40, 0, 0, 0, 0, 0 };
static OptFunc DOptBoolUnary2   = { OptBoolUnary2,   "OptBoolUnary2",    40, 0, 0, 0, 0, 0 };
static OptFunc DOptBoolUnary3   = { OptBoolUnary3,   "OptBoolUnary3",    40, 0, 0, 0, 0, 0 };
static OptFunc DOptBranchDist   = { OptBranchDist,   "OptBranchDist",     0, 0, 0, 0, 0, 0 };
static OptFunc DOptBranchDist2  = { OptBranchDist2,  "OptBranchDist2",    0, 0, 0, 0, 0, 0 };
static OptFunc DOptCmp1         = { OptCmp1,         "OptCmp1",          42, 0, 0, 0, 0, 0 };
static OptFunc DOptCmp2         = { OptCmp2,         "OptCmp2",          85, 0, 0, 0, 0, 0 };
static OptFunc DOptCmp3         = { OptCmp3,         "OptCmp3",          75, 0, 0, 0, 0, 0 };
static OptFunc DOptCmp4         = { OptCmp4,         "OptCmp4",          75, 0, 0, 0, 0, 0 };
static OptFunc DOptCmp5         = { OptCmp5,         "OptCmp5",         100, 0, 0, 0, 0, 0 };
static OptFunc DOptCmp7         = { OptCmp7,         "OptCmp7",          85, 0, 0, 0, 0, 0 };
static OptFunc DOptCmp8         = { OptCmp8,         "OptCmp8",          50, 0, 0, 0, 0, 0 };
static OptFunc DOptCmp9         = { OptCmp9,         "OptCmp9",          85, 0, 0, 0, 0, 0 };
static OptFunc DOptComplAX1     = { OptComplAX1,     "OptComplAX1",      65, 0, 0, 0, 0, 0 };
static OptFunc DOptCondBranch1  = { OptCondBranch1,  "OptCondBranch1",   80, 0, 0, 0, 0, 0 };
static OptFunc DOptCondBranch2  = { OptCondBranch2,  "OptCondBranch2",   40, 0, 0, 0, 0, 0 };
static OptFunc DOptCondBranch3  = { OptCondBranch3,  "OptCondBranch3",   40, 0, 0, 0, 0, 0 };
static OptFunc DOptCondBranchC  = { OptCondBranchC,  "OptCondBranchC",    0, 0, 0, 0, 0, 0 };
static OptFunc DOptDeadCode     = { OptDeadCode,     "OptDeadCode",     100, 0, 0, 0, 0, 0 };
static OptFunc DOptDeadJumps    = { OptDeadJumps,    "OptDeadJumps",    100, 0, 0, 0, 0, 0 };
static OptFunc DOptDecouple     = { OptDecouple,     "OptDecouple",     100, 0, 0, 0, 0, 0 };
static OptFunc DOptDupLoads     = { OptDupLoads,     "OptDupLoads",       0, 0, 0, 0, 0, 0 };
static OptFunc DOptGotoSPAdj    = { OptGotoSPAdj,    "OptGotoSPAdj",      0, 0, 0, 0, 0, 0 };
static OptFunc DOptIndLoads1    = { OptIndLoads1,    "OptIndLoads1",      0, 0, 0, 0, 0, 0 };
static OptFunc DOptIndLoads2    = { OptIndLoads2,    "OptIndLoads2",      0, 0, 0, 0, 0, 0 };
static OptFunc DOptJumpCascades = { OptJumpCascades, "OptJumpCascades", 100, 0, 0, 0, 0, 0 };
static OptFunc DOptJumpTarget1  = { OptJumpTarget1,  "OptJumpTarget1",  100, 0, 0, 0, 0, 0 };
static OptFunc DOptJumpTarget2  = { OptJumpTarget2,  "OptJumpTarget2",  100, 0, 0, 0, 0, 0 };
static OptFunc DOptJumpTarget3  = { OptJumpTarget3,  "OptJumpTarget3",  100, 0, 0, 0, 0, 0 };
static OptFunc DOptLoad1        = { OptLoad1,        "OptLoad1",        100, 0, 0, 0, 0, 0 };
static OptFunc DOptLoad2        = { OptLoad2,        "OptLoad2",        200, 0, 0, 0, 0, 0 };
static OptFunc DOptLoad3        = { OptLoad3,        "OptLoad3",          0, 0, 0, 0, 0, 0 };
static OptFunc DOptLongAssign   = { OptLongAssign,   "OptLongAssign",   100, 0, 0, 0, 0, 0 };
static OptFunc DOptLongCopy     = { OptLongCopy,     "OptLongCopy",     100, 0, 0, 0, 0, 0 };
static OptFunc DOptNegAX1       = { OptNegAX1,       "OptNegAX1",       165, 0, 0, 0, 0, 0 };
static OptFunc DOptNegAX2       = { OptNegAX2,       "OptNegAX2",       200, 0, 0, 0, 0, 0 };
static OptFunc DOptPrecalc      = { OptPrecalc,      "OptPrecalc",      100, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad1     = { OptPtrLoad1,     "OptPtrLoad1",     100, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad2     = { OptPtrLoad2,     "OptPtrLoad2",     100, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad3     = { OptPtrLoad3,     "OptPtrLoad3",     100, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad4     = { OptPtrLoad4,     "OptPtrLoad4",     100, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad5     = { OptPtrLoad5,     "OptPtrLoad5",      50, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad6     = { OptPtrLoad6,     "OptPtrLoad6",      60, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad7     = { OptPtrLoad7,     "OptPtrLoad7",     140, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad11    = { OptPtrLoad11,    "OptPtrLoad11",     92, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad12    = { OptPtrLoad12,    "OptPtrLoad12",     50, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad13    = { OptPtrLoad13,    "OptPtrLoad13",     65, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad14    = { OptPtrLoad14,    "OptPtrLoad14",    108, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad15    = { OptPtrLoad15,    "OptPtrLoad15",     86, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad16    = { OptPtrLoad16,    "OptPtrLoad16",    100, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad17    = { OptPtrLoad17,    "OptPtrLoad17",    190, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad18    = { OptPtrLoad18,    "OptPtrLoad18",    100, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad19    = { OptPtrLoad19,    "OptPtrLoad19",     65, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrStore1    = { OptPtrStore1,    "OptPtrStore1",     65, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrStore2    = { OptPtrStore2,    "OptPtrStore2",     65, 0, 0, 0, 0, 0 };
static OptFunc DOptPtrStore3    = { OptPtrStore3,    "OptPtrStore3",    100, 0, 0, 0, 0, 0 };
static OptFunc DOptPush1        = { OptPush1,        "OptPush1",         65, 0, 0, 0, 0, 0 };
static OptFunc DOptPush2        = { OptPush2,        "OptPush2",         50, 0, 0, 0, 0, 0 };
static OptFunc DOptPushPop1     = { OptPushPop1,     "OptPushPop1",       0, 0, 0, 0, 0, 0 };
static OptFunc DOptPushPop2     = { OptPushPop2,     "OptPushPop2",       0, 0, 0, 0, 0, 0 };
static OptFunc DOptRTS          = { OptRTS,          "OptRTS",          100, 0, 0, 0, 0, 0 };
static OptFunc DOptRTSJumps1    = { OptRTSJumps1,    "OptRTSJumps1",    100, 0, 0, 0, 0, 0 };
static OptFunc DOptRTSJumps2    = { OptRTSJumps2,    "OptRTSJumps2",    100, 0, 0, 0, 0, 0 };
static OptFunc DOptShift1       = { OptShift1,       "OptShift1",       100, 0, 0, 0, 0, 0 };
static OptFunc DOptShift2       = { OptShift2,       "OptShift2",       100, 0, 0, 0, 0, 0 };
static OptFunc DOptShift3       = { OptShift3,       "OptShift3",        17, 0, 0, 0, 0, 0 };
static OptFunc DOptShift4       = { OptShift4,       "OptShift4",       100, 0, 0, 0, 0, 0 };
static OptFunc DOptShift5       = { OptShift5,       "OptShift5",       110, 0, 0, 0, 0, 0 };
static OptFunc DOptShift6       = { OptShift6,       "OptShift6",       200, 0, 0, 0, 0, 0 };
static OptFunc DOptShiftBack    = { OptShiftBack,    "OptShiftBack",      0, 0, 0, 0, 0, 0 };
static OptFunc DOptSignExtended = { OptSignExtended, "OptSignExtended",   0, 0, 0, 0, 0, 0 };
static OptFunc DOptSize1        = { OptSize1,        "OptSize1",        100, 0, 0, 0, 0, 0 };
static OptFunc DOptSize2        = { OptSize2,        "OptSize2",        100, 0, 0, 0, 0, 0 };
static OptFunc DOptStackOps     = { OptStackOps,     "OptStackOps",     100, 0, 0, 0, 0, 0 };
static OptFunc DOptStackPtrOps  = { OptStackPtrOps,  "OptStackPtrOps",   50, 0, 0, 0, 0, 0 };
static OptFunc DOptStore1       = { OptStore1,       "OptStore1",        70, 0, 0, 0, 0, 0 };
static OptFunc DOptStore2       = { OptStore2,       "OptStore2",       115, 0, 0, 0, 0, 0 };
static OptFunc DOptStore3       = { OptStore3,       "OptStore3",       120, 0, 0, 0, 0, 0 };
static OptFunc DOptStore4       = { OptStore4,       "OptStore4",        50, 0, 0, 0, 0, 0 };
static OptFunc DOptStore5       = { OptStore5,       "OptStore5",       100, 0, 0, 0, 0, 0 };
static OptFunc DOptStoreLoad    = { OptStoreLoad,    "OptStoreLoad",      0, 0, 0, 0, 0, 0 };
static OptFunc DOptLoadStoreLoad= { OptLoadStoreLoad,"OptLoadStoreLoad",  0, 0, 0, 0, 0, 0 };
static OptFunc DOptSub1         = { OptSub1,         "OptSub1",         100, 0, 0, 0, 0, 0 };
static OptFunc DOptSub2         = { OptSub2,         "OptSub2",         100, 0, 0, 0, 0, 0 };
static OptFunc DOptSub3         = { OptSub3,         "OptSub3",         100, 0, 0, 0, 0, 0 };
static OptFunc DOptTest1        = { OptTest1,        "OptTest1",         65, 0, 0, 0, 0, 0 };
static OptFunc DOptTest2        = { OptTest2,        "OptTest2",         50, 0, 0, 0, 0, 0 };
static OptFunc DOptTransfers1   = { OptTransfers1,   "OptTransfers1",     0, 0, 0, 0, 0, 0 };
static OptFunc DOptTransfers2   = { OptTransfers2,   "OptTransfers2",    60, 0, 0, 0, 0, 0 };
static OptFunc DOptTransfers3   = { OptTransfers3,   "OptTransfers3",    65, 0, 0, 0, 0, 0 };
static OptFunc DOptTransfers4   = { OptTransfers4,   "OptTransfers4",    65, 0, 0, 0, 0, 0 };
static OptFunc DOptUnusedLoads  = { OptUnusedLoads,  "OptUnusedLoads",    0, 0, 0, 0, 0, 0 };
static OptFunc DOptUnusedStores = { OptUnusedStores, "OptUnusedStores",   0, 0, 0, 0, 0, 0 };


/* Table containing all the steps in alphabetical order */
static OptFunc* OptFuncs[] = {
    &DOpt65C02BitOps,
    &DOpt65C02Ind,
    &DOpt65C02Stores,
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
    &DOptLoadStoreLoad,
    &DOptSub1,
    &DOptSub2,
    &DOptSub3,
    &DOptTest1,
    &DOptTest2,
    &DOptTransfers1,
    &DOptTransfers2,
    &DOptTransfers3,
    &DOptTransfers4,
    &DOptUnusedLoads,
    &DOptUnusedStores,
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
        C += RunOptFunc (S, &DOptDupLoads, 1);
        C += RunOptFunc (S, &DOptStoreLoad, 1);
        C += RunOptFunc (S, &DOptLoadStoreLoad, 1);
        C += RunOptFunc (S, &DOptTransfers1, 1);
        C += RunOptFunc (S, &DOptTransfers3, 1);
        C += RunOptFunc (S, &DOptTransfers4, 1);
        C += RunOptFunc (S, &DOptStore1, 1);
        C += RunOptFunc (S, &DOptStore5, 1);
        C += RunOptFunc (S, &DOptPushPop1, 1);
        C += RunOptFunc (S, &DOptPushPop2, 1);
        C += RunOptFunc (S, &DOptPrecalc, 1);
        C += RunOptFunc (S, &DOptShiftBack, 1);
        C += RunOptFunc (S, &DOptSignExtended, 1);

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
/* This one is quite special. It tries to replace "lda (sp),y" by "lda (sp,x)".
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
        Changes += RunOptFunc (S, &DOptUnusedStores, 1);
        Changes += RunOptFunc (S, &DOptJumpTarget1, 5);
        Changes += RunOptFunc (S, &DOptStore5, 1);
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
