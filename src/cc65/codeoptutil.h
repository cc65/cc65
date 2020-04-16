/*****************************************************************************/
/*                                                                           */
/*                               codeoptutil.h                               */
/*                                                                           */
/*           Optimize operations that take operands via the stack            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
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



#ifndef CODEOPTUTIL_H
#define CODEOPTUTIL_H



/* cc65 */
#include "codeent.h"
#include "codeseg.h"



/*****************************************************************************/
/*                            Load tracking data                             */
/*****************************************************************************/



/* LoadRegInfo flags set by DirectOp */
typedef enum {
  LI_NONE               = 0x00,
  LI_DIRECT             = 0x01,         /* Direct op may be used */
  LI_RELOAD_Y           = 0x02,         /* Reload index register Y */
  LI_REMOVE             = 0x04,         /* Load may be removed */
  LI_DONT_REMOVE        = 0x08,         /* Load may not be removed */
  LI_CHECK_ARG          = 0x10,         /* Load src might be modified later */
  LI_SRC_CHG            = 0x20,         /* Load src is possibly modified */
  LI_LOAD_INSN          = 0x40,         /* Has a load insn */
  LI_CHECK_Y            = 0x80,         /* Indexed load src might be modified later */
  LI_USED_BY_A          = 0x100,        /* Content used by RegA */
  LI_USED_BY_X          = 0x200,        /* Content used by RegX */
  LI_USED_BY_Y          = 0x400,        /* Content used by RegY */
  LI_SP                 = 0x800,        /* Content on stack */
} LI_FLAGS;

/* Structure that tells us how to load the lhs values */
typedef struct LoadRegInfo LoadRegInfo;
struct LoadRegInfo {
    LI_FLAGS            Flags;          /* Tells us how to load */
    int                 LoadIndex;      /* Index of load insn, -1 if invalid */
    CodeEntry*          LoadEntry;      /* The actual entry, 0 if invalid */
    int                 LoadYIndex;     /* Index of Y-load insn, -1 if invalid  */
    CodeEntry*          LoadYEntry;     /* The actual Y-load entry, 0 if invalid */
    int                 XferIndex;      /* Index of transfer insn  */
    CodeEntry*          XferEntry;      /* The actual transfer entry */
    int                 Offs;           /* Stack offset if data is on stack */
};

/* Now combined for both registers */
typedef struct LoadInfo LoadInfo;
struct LoadInfo {
    LoadRegInfo         A;              /* Info for A register */
    LoadRegInfo         X;              /* Info for X register */
    LoadRegInfo         Y;              /* Info for Y register */
};

/* Structure forward decl */
typedef struct StackOpData StackOpData;
typedef struct OptFuncDesc OptFuncDesc;

/* Structure that holds the needed data */
struct StackOpData {
    CodeSeg*            Code;           /* Pointer to code segment */
    unsigned            Flags;          /* Flags to remember things */

    /* Pointer to optimizer subfunction description */
    const OptFuncDesc*  OptFunc;

    /* ZP register usage inside the sequence */
    unsigned            ZPUsage;
    unsigned            ZPChanged;

    /* Freedom of registers inside the sequence */
    unsigned            UsedRegs;       /* Registers used */

    /* Whether the rhs is changed multiple times */
    int                 RhsMultiChg;

    /* Register load information for lhs and rhs */
    LoadInfo            Lhs;
    LoadInfo            Rhs;

    /* Several indices of insns in the code segment */
    int                 PushIndex;      /* Index of call to pushax in codeseg */
    int                 OpIndex;        /* Index of actual operation */

    /* Pointers to insns in the code segment */
    CodeEntry*          PrevEntry;      /* Entry before the call to pushax */
    CodeEntry*          PushEntry;      /* Pointer to entry with call to pushax */
    CodeEntry*          OpEntry;        /* Pointer to entry with op */
    CodeEntry*          NextEntry;      /* Entry after the op */

    const char*         ZPLo;           /* Lo byte of zero page loc to use */
    const char*         ZPHi;           /* Hi byte of zero page loc to use */
    unsigned            IP;             /* Insertion point used by some routines */
};



/*****************************************************************************/
/*                            Load tracking code                             */
/*****************************************************************************/



void ClearLoadRegInfo (LoadRegInfo* LRI);
/* Clear a LoadRegInfo struct */

void CopyLoadRegInfo (LoadRegInfo* To, LoadRegInfo* From);
/* Copy a LoadRegInfo struct */

void FinalizeLoadRegInfo (LoadRegInfo* LRI, CodeSeg* S);
/* Prepare a LoadRegInfo struct for use */

void AdjustLoadRegInfo (LoadRegInfo* LRI, int Index, int Change);
/* Adjust a load register info struct after deleting or inserting an entry
** with a given index
*/

void ClearLoadInfo (LoadInfo* LI);
/* Clear a LoadInfo struct */

void CopyLoadInfo (LoadInfo* To, LoadInfo* From);
/* Copy a LoadInfo struct */

void FinalizeLoadInfo (LoadInfo* LI, CodeSeg* S);
/* Prepare a LoadInfo struct for use */

void AdjustLoadInfo (LoadInfo* LI, int Index, int Change);
/* Adjust a load info struct after deleting entry with a given index */

RegInfo* GetLastChangedRegInfo (StackOpData* D, LoadRegInfo* Reg);
/* Get RegInfo of the last insn entry that changed the reg */

unsigned int TrackLoads (LoadInfo* LI, LoadInfo* LLI, CodeSeg* S, int I);
/* Track loads for a code entry.
** Return used registers.
*/

void SetDontRemoveEntryFlag (LoadRegInfo* LRI);
/* Flag the entry as non-removable according to register flags */

void ResetDontRemoveEntryFlag (LoadRegInfo* LRI);
/* Unflag the entry as non-removable according to register flags */

void SetDontRemoveEntryFlags (StackOpData* D);
/* Flag the entries as non-removable according to register flags */

void ResetDontRemoveEntryFlags (StackOpData* D);
/* Unflag the entries as non-removable according to register flags */

void ResetStackOpData (StackOpData* Data);
/* Reset the given data structure */



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



void InsertEntry (StackOpData* D, CodeEntry* E, int Index);
/* Insert a new entry. Depending on Index, D->PushIndex and D->OpIndex will
** be adjusted by this function.
*/

void DelEntry (StackOpData* D, int Index);
/* Delete an entry. Depending on Index, D->PushIndex and D->OpIndex will be
** adjusted by this function, and PushEntry/OpEntry may get invalidated.
*/

void AdjustStackOffset (StackOpData* D, unsigned Offs);
/* Adjust the offset for all stack accesses in the range PushIndex to OpIndex.
** OpIndex is adjusted according to the insertions.
*/

int IsRegVar (StackOpData* D);
/* If the value pushed is that of a zeropage variable that is unchanged until Op,
** replace ZPLo and ZPHi in the given StackOpData struct by the variable and return true.
** Otherwise leave D untouched and return false.
*/

void AddStoreLhsA (StackOpData* D);
/* Add a store to zero page after the push insn */

void AddStoreLhsX (StackOpData* D);
/* Add a store to zero page after the push insn */

void ReplacePushByStore (StackOpData* D);
/* Replace the call to the push subroutine by a store into the zero page
** location (actually, the push is not replaced, because we need it for
** later, but the name is still ok since the push will get removed at the
** end of each routine).
*/

void AddOpLow (StackOpData* D, opc_t OPC, LoadInfo* LI);
/* Add an op for the low byte of an operator. This function honours the
** OP_DIRECT and OP_RELOAD_Y flags and generates the necessary instructions.
** All code is inserted at the current insertion point.
*/

void AddOpHigh (StackOpData* D, opc_t OPC, LoadInfo* LI, int KeepResult);
/* Add an op for the high byte of an operator. Special cases (constant values
** or similar) have to be checked separately, the function covers only the
** generic case. Code is inserted at the insertion point.
*/

void RemoveRegLoads (StackOpData* D, LoadInfo* LI);
/* Remove register load insns */


void RemoveRemainders (StackOpData* D);
/* Remove the code that is unnecessary after translation of the sequence */

int HarmlessCall (const char* Name);
/* Check if this is a call to a harmless subroutine that will not interrupt
** the pushax/op sequence when encountered.
*/

/* End of codeoptutil.h */

#endif
