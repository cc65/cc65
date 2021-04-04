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
    LI_NONE         = 0x00,
    LI_DIRECT       = 0x01,         /* Direct op may be used */
    LI_RELOAD_Y     = 0x02,         /* Reload index register Y */
    LI_REMOVE       = 0x04,         /* Load may be removed */
    LI_DONT_REMOVE  = 0x08,         /* Load may not be removed */
    LI_CHECK_ARG    = 0x10,         /* Load src might be modified later */
    LI_CHECK_Y      = 0x20,         /* Indexed load src might be modified later */
    LI_SRC_USE      = 0x40,         /* src of Opc argument is possibly used */
    LI_SRC_CHG      = 0x80,         /* src of Opc argument is possibly modified */
    LI_Y_SRC_USE    = 0x0100,       /* src of Opc addressing Y is possibly used */
    LI_Y_SRC_CHG    = 0x0200,       /* src of Opc addressing Y is possibly modified */
    LI_Y_USE        = 0x0400,       /* Opc addressing Y is possibly used */
    LI_Y_CHG        = 0x0800,       /* Opc addressing Y is possibly modified */
    LI_USED_BY_A    = 0x1000,       /* Content used by RegA */
    LI_USED_BY_X    = 0x2000,       /* Content used by RegX */
    LI_USED_BY_Y    = 0x4000,       /* Content used by RegY */
    LI_SP           = 0x8000,       /* Content on stack */
    LI_LOAD_INSN    = 0x010000,     /* Is a load insn */
} LI_FLAGS;

/* Structure that tells us how to load the lhs values */
typedef struct LoadRegInfo LoadRegInfo;
struct LoadRegInfo {
    LI_FLAGS        Flags;          /* Tells us how to load */
    int             LoadIndex;      /* Index of load insn, -1 if invalid */
    CodeEntry*      LoadEntry;      /* The actual entry, 0 if invalid */
    int             LoadYIndex;     /* Index of Y-load insn, -1 if invalid  */
    CodeEntry*      LoadYEntry;     /* The actual Y-load entry, 0 if invalid */
    int             ChgIndex;       /* Index of last change */
    CodeEntry*      ChgEntry;       /* The actual change entry */
    int             Offs;           /* Stack offset if data is on stack */
};

/* Now combined for both registers */
typedef struct LoadInfo LoadInfo;
struct LoadInfo {
    LoadRegInfo     A;              /* Info for A register */
    LoadRegInfo     X;              /* Info for X register */
    LoadRegInfo     Y;              /* Info for Y register */
};

/* Structure forward decl */
typedef struct StackOpData StackOpData;

/* Structure that holds the needed data */
struct StackOpData {
    CodeSeg*    Code;           /* Pointer to code segment */
    unsigned    Flags;          /* Flags to remember things */

    /* Pointer to optimizer subfunction description */
    const void* OptFunc;

    /* ZP register usage inside the sequence */
    unsigned    ZPUsage;
    unsigned    ZPChanged;

    /* Freedom of registers inside the sequence */
    unsigned    UsedRegs;       /* Registers used */

    /* Whether the rhs is changed multiple times */
    int         RhsMultiChg;

    /* Register load information for lhs, rhs and rv */
    LoadInfo    Lhs;
    LoadInfo    Rhs;
    LoadInfo    Rv;

    /* Several indices of insns in the code segment */
    int         PushIndex;      /* Index of call to pushax in codeseg */
    int         OpIndex;        /* Index of actual operation */

    /* Pointers to insns in the code segment */
    CodeEntry*  PrevEntry;      /* Entry before the call to pushax */
    CodeEntry*  PushEntry;      /* Pointer to entry with call to pushax */
    CodeEntry*  OpEntry;        /* Pointer to entry with op */
    CodeEntry*  NextEntry;      /* Entry after the op */

    const char* ZPLo;           /* Lo byte of zero page loc to use */
    const char* ZPHi;           /* Hi byte of zero page loc to use */
    unsigned    IP;             /* Insertion point used by some routines */
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

void PrepairLoadRegInfoForArgCheck (CodeSeg* S, LoadRegInfo* LRI, CodeEntry* E);
/* Set the load src flags and remember to check for load src change if necessary */

void SetIfOperandSrcAffected (LoadInfo* LLI, CodeEntry* E);
/* Check and flag operand src that may be affected */

void SetIfOperandLoadUnremovable (LoadInfo* LI, unsigned Used);
/* Check and flag operand load that may be unremovable */

unsigned int TrackLoads (LoadInfo* LI, CodeSeg* S, int I);
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



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



/* Backup location types */
#define BU_UNKNOWN      0x00000000U   /* Unknown */
#define BU_IMM          0x00000000U   /* Immediate */
#define BU_REG          0x01000000U   /* In register */
#define BU_ZP           0x02000000U   /* On ZP */
#define BU_SP6502       0x04000000U   /* On 6502 stack */
#define BU_SP           0x08000000U   /* On CC65 stack */
#define BU_B8           0x00000000U   /* Size of 8-bit */
#define BU_B16          0x10000000U   /* Size of 16-bit */
#define BU_B24          0x20000000U   /* Size of 24-bit */
#define BU_B32          0x30000000U   /* Size of 32-bit */
#define BU_TYPE_MASK    0x0F000000U   /* Type mask */
#define BU_SIZE_MASK    0xF0000000U   /* Size mask */

typedef struct {
    unsigned Type;              /* Backup location type and size */
    unsigned ZPUsage;           /* ZP unusable for backup */
    union {
        unsigned        Where;  /* Backup location */
        unsigned        Imm;    /* Backed-up value */
        unsigned char*  Bytes;  /* Pointer to backed-up value */
    };
} BackupInfo;



const char* GetZPName (unsigned ZPLoc);
/* Get the name strings of certain known ZP Regs */

unsigned FindAvailableBackupLoc (BackupInfo* B, unsigned Type);
/* Find a ZP loc for storing the backup and fill in the info.
** The allowed types are specified with the Type parameter.
** For convenience, all types are aloowed if none is specified.
** Return the type of the found loc.
*/

void AdjustEntryIndices (Collection* Indices, int Index, int Change);
/* Adjust a load register info struct after deleting or inserting successive
** entries with a given index.
*/

void DelEntryIdx (CodeSeg* S, int Idx, Collection* Indices);
/* Delete an entry and adjust Indices if necessary */

void DelEntriesIdx (CodeSeg* S, int Idx, int Count, Collection* Indices);
/* Delete entries and adjust Indices if necessary */

void RemoveFlaggedRegLoads (CodeSeg* S, LoadRegInfo* LRI, Collection* Indices);
/* Remove flagged register load insns */

void RemoveFlaggedLoads (CodeSeg* S, LoadInfo* LI, Collection* Indices);
/* Remove flagged load insns */

int BackupABefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of A before the specified index Idx */

int BackupXBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of X before the specified index Idx */

int BackupYBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of Y before the specified index Idx */

int BackupAXBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of AX before the specified index Idx */

int BackupAXYBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of AXY before the specified index Idx.
** This doesn't allow separating the backup of Y from that of AX for now.
*/

int BackupAAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of A after the specified index Idx */

int BackupXAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of X after the specified index Idx */

int BackupYAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of Y after the specified index Idx */

int BackupAXAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of AX after the specified index Idx */

int BackupAXYAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Backup the content of AXY after the specified index Idx.
** This doesn't allow separating the backup of Y from that of AX for now.
*/

int RestoreABefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Restore the content of Y before the specified index Idx */

int RestoreXBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Restore the content of X before the specified index Idx */

int RestoreYBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Restore the content of Y before the specified index Idx */

int RestoreAXBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Restore the content of AX before the specified index Idx */

int RestoreAXYBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices);
/* Restore the content of AXY before the specified index Idx.
** This only allows restore from compacted AXY backup for now.
*/

int BackupArgAfter (CodeSeg* S, BackupInfo* B, int Idx, const CodeEntry* E, Collection* Indices);
/* Backup the content of the opc arg of the entry E after the specified index Idx.
** Reg A/Y will be used to transfer the content from a memory location to another
** regardless of whether it is in use.
*/

int LoadABefore (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices);
/* Reload into A the same arg according to LoadRegInfo at Idx */

int LoadXBefore (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices);
/* Reload into X the same arg according to LoadRegInfo at Idx */

int LoadYBefore (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices);
/* Reload into Y the same arg according to LoadRegInfo at Idx */

int LoadAAfter (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices);
/* Reload into A the same arg according to LoadRegInfo after Idx */

int LoadXAfter (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices);
/* Reload into X the same arg according to LoadRegInfo after Idx */

int LoadYAfter (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices);
/* Reload into Y the same arg according to LoadRegInfo after Idx */

unsigned GetRegAccessedInOpenRange (CodeSeg* S, int First, int Last);
/* Get what ZPs, registers or processor states are used or changed in the range
** (First, Last).
** The code block must be basic without any jump backwards.
*/

unsigned GetRegUsageInOpenRange (CodeSeg* S, int First, int Last, unsigned* Use, unsigned* Chg);
/* Get what ZPs, registers or processor states are used or changed in the range
** (First, Last) in output parameters Use and Chg.
** Return what ZP regs are used before changed in this range.
** The code block must be basic without any jump backwards.
*/

int IsArgSameInOpenRange (CodeSeg* S, int First, int Last, CodeEntry* E);
/* Check if the loading the opc arg gives the same result everywhere between (First, Last).
** The code block in the range must be basic without any jump backwards.
** Note: this always checks Y if any of the LI_CHECK_Y / LI_RELOAD_Y flags is set.
*/

int FindArgFirstChangeInOpenRange (CodeSeg* S, int First, int Last, CodeEntry* E);
/* Find the first possible spot where the loaded arg of E might be changed in
** the range (First, Last). The code block in the range must be basic without
** any jump backwards.
** Return the index of the found entry, or Last if not found.
** Note: changes of Y are always ignored even if the LI_RELOAD_Y flag is not set.
*/

int FindArgLastUsageInOpenRange (CodeSeg* S, int First, int Last, CodeEntry* E, int ReloadY);
/* Find the last index where the arg of E might be used or changed in the range (First, Last).
** ReloadY indicates whether Y is supposed to be reloaded.
** The code block in the range must be basic without any jump backwards.
** Return the index of the found entry, or First if not found.
*/

int FindRegFirstChangeInOpenRange (CodeSeg* S, int First, int Last, unsigned what);
/* Find the first possible spot where the queried ZPs, registers and/or processor
** states might be changed in the range (First, Last). The code block in the
** range must be basic without any jump backwards.
** Return the index of the found entry, or Last if not found.
*/

int FindRegFirstUseInOpenRange (CodeSeg* S, int First, int Last, unsigned what);
/* Find the first possible spot where the queried ZPs, registers and/or processor
** states might be used in the range (First, Last). The code block in the range
** must be basic without any jump backwards.
** Return the index of the found entry, or Last if not found.
*/

int FindRegLastChangeInOpenRange (CodeSeg* S, int First, int Last, unsigned what);
/* Find the last possible spot where the queried ZPs, registers and/or processor
** states might be changed in the range (First, Last). The code block in the
** range must be basic without any jump backwards.
** Return the index of the found entry, or First if not found.
*/

int FindRegLastUseInOpenRange (CodeSeg* S, int First, int Last, unsigned what);
/* Find the last possible spot where the queried ZPs, registers and/or processor
** states might be used in the range (First, Last). The code block in the range
** must be basic without any jump backwards.
** Return the index of the found entry, or First if not found.
*/

/* End of codeoptutil.h */

#endif
