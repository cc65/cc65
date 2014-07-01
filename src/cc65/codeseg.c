/*****************************************************************************/
/*                                                                           */
/*                                 codeseg.c                                 */
/*                                                                           */
/*                          Code segment structure                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2011, Ullrich von Bassewitz                                      */
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



#include <string.h>
#include <ctype.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "debugflag.h"
#include "global.h"
#include "hashfunc.h"
#include "strbuf.h"
#include "strutil.h"
#include "xmalloc.h"

/* cc65 */
#include "asmlabel.h"
#include "codeent.h"
#include "codeinfo.h"
#include "codeseg.h"
#include "datatype.h"
#include "error.h"
#include "global.h"
#include "ident.h"
#include "output.h"
#include "symentry.h"



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void CS_PrintFunctionHeader (const CodeSeg* S)
/* Print a comment with the function signature to the output file */
{
    /* Get the associated function */
    const SymEntry* Func = S->Func;

    /* If this is a global code segment, do nothing */
    if (Func) {
        WriteOutput ("; ---------------------------------------------------------------\n"
                     "; ");
        PrintFuncSig (OutputFile, Func->Name, Func->Type);
        WriteOutput ("\n"
                     "; ---------------------------------------------------------------\n"
                     "\n");
    }
}



static void CS_MoveLabelsToEntry (CodeSeg* S, CodeEntry* E)
/* Move all labels from the label pool to the given entry and remove them
** from the pool.
*/
{
    /* Transfer the labels if we have any */
    unsigned I;
    unsigned LabelCount = CollCount (&S->Labels);
    for (I = 0; I < LabelCount; ++I) {

        /* Get the label */
        CodeLabel* L = CollAt (&S->Labels, I);

        /* Attach it to the entry */
        CE_AttachLabel (E, L);
    }

    /* Delete the transfered labels */
    CollDeleteAll (&S->Labels);
}



static void CS_MoveLabelsToPool (CodeSeg* S, CodeEntry* E)
/* Move the labels of the code entry E to the label pool of the code segment */
{
    unsigned LabelCount = CE_GetLabelCount (E);
    while (LabelCount--) {
        CodeLabel* L = CE_GetLabel (E, LabelCount);
        L->Owner = 0;
        CollAppend (&S->Labels, L);
    }
    CollDeleteAll (&E->Labels);
}



static CodeLabel* CS_FindLabel (CodeSeg* S, const char* Name, unsigned Hash)
/* Find the label with the given name. Return the label or NULL if not found */
{
    /* Get the first hash chain entry */
    CodeLabel* L = S->LabelHash[Hash];

    /* Search the list */
    while (L) {
        if (strcmp (Name, L->Name) == 0) {
            /* Found */
            break;
        }
        L = L->Next;
    }
    return L;
}



static CodeLabel* CS_NewCodeLabel (CodeSeg* S, const char* Name, unsigned Hash)
/* Create a new label and insert it into the label hash table */
{
    /* Create a new label */
    CodeLabel* L = NewCodeLabel (Name, Hash);

    /* Enter the label into the hash table */
    L->Next = S->LabelHash[L->Hash];
    S->LabelHash[L->Hash] = L;

    /* Return the new label */
    return L;
}



static void CS_RemoveLabelFromHash (CodeSeg* S, CodeLabel* L)
/* Remove the given code label from the hash list */
{
    /* Get the first entry in the hash chain */
    CodeLabel* List = S->LabelHash[L->Hash];
    CHECK (List != 0);

    /* First, remove the label from the hash chain */
    if (List == L) {
        /* First entry in hash chain */
        S->LabelHash[L->Hash] = L->Next;
    } else {
        /* Must search through the chain */
        while (List->Next != L) {
            /* If we've reached the end of the chain, something is *really* wrong */
            CHECK (List->Next != 0);
            /* Next entry */
            List = List->Next;
        }
        /* The next entry is the one, we have been searching for */
        List->Next = L->Next;
    }
}



/*****************************************************************************/
/*                    Functions for parsing instructions                     */
/*****************************************************************************/



static const char* SkipSpace (const char* S)
/* Skip white space and return an updated pointer */
{
    while (IsSpace (*S)) {
        ++S;
    }
    return S;
}



static const char* ReadToken (const char* L, const char* Term,
                              char* Buf, unsigned BufSize)
/* Read the next token into Buf, return the updated line pointer. The
** token is terminated by one of the characters given in term.
*/
{
    /* Read/copy the token */
    unsigned I = 0;
    unsigned ParenCount = 0;
    while (*L && (ParenCount > 0 || strchr (Term, *L) == 0)) {
        if (I < BufSize-1) {
            Buf[I] = *L;
        } else if (I == BufSize-1) {
            /* Cannot store this character, this is an input error (maybe
            ** identifier too long or similar).
            */
            Error ("ASM code error: syntax error");
        }
        ++I;
        if (*L == ')') {
            --ParenCount;
        } else if (*L == '(') {
            ++ParenCount;
        }
        ++L;
    }

    /* Terminate the buffer contents */
    Buf[I] = '\0';

    /* Return the updated line pointer */
    return L;
}



static CodeEntry* ParseInsn (CodeSeg* S, LineInfo* LI, const char* L)
/* Parse an instruction nnd generate a code entry from it. If the line contains
** errors, output an error message and return NULL.
** For simplicity, we don't accept the broad range of input a "real" assembler
** does. The instruction and the argument are expected to be separated by
** white space, for example.
*/
{
    char                Mnemo[IDENTSIZE+10];
    const OPCDesc*      OPC;
    am_t                AM = 0;         /* Initialize to keep gcc silent */
    char                Arg[IDENTSIZE+10];
    char                Reg;
    CodeEntry*          E;
    CodeLabel*          Label;

    /* Read the first token and skip white space after it */
    L = SkipSpace (ReadToken (L, " \t:", Mnemo, sizeof (Mnemo)));

    /* Check if we have a label */
    if (*L == ':') {

        /* Skip the colon and following white space */
        L = SkipSpace (L+1);

        /* Add the label */
        CS_AddLabel (S, Mnemo);

        /* If we have reached end of line, bail out, otherwise a mnemonic
        ** may follow.
        */
        if (*L == '\0') {
            return 0;
        }

        L = SkipSpace (ReadToken (L, " \t", Mnemo, sizeof (Mnemo)));
    }

    /* Try to find the opcode description for the mnemonic */
    OPC = FindOP65 (Mnemo);

    /* If we didn't find the opcode, print an error and bail out */
    if (OPC == 0) {
        Error ("ASM code error: %s is not a valid mnemonic", Mnemo);
        return 0;
    }

    /* Get the addressing mode */
    Arg[0] = '\0';
    switch (*L) {

        case '\0':
            /* Implicit or accu */
            if (OPC->Info & OF_NOIMP) {
                AM = AM65_ACC;
            } else {
                AM = AM65_IMP;
            }
            break;

        case '#':
            /* Immidiate */
            StrCopy (Arg, sizeof (Arg), L+1);
            AM = AM65_IMM;
            break;

        case '(':
            /* Indirect */
            L = ReadToken (L+1, ",)", Arg, sizeof (Arg));

            /* Check for errors */
            if (*L == '\0') {
                Error ("ASM code error: syntax error");
                return 0;
            }

            /* Check the different indirect modes */
            if (*L == ',') {
                /* Expect zp x indirect */
                L = SkipSpace (L+1);
                if (toupper (*L) != 'X') {
                    Error ("ASM code error: `X' expected");
                    return 0;
                }
                L = SkipSpace (L+1);
                if (*L != ')') {
                    Error ("ASM code error: `)' expected");
                    return 0;
                }
                L = SkipSpace (L+1);
                if (*L != '\0') {
                    Error ("ASM code error: syntax error");
                    return 0;
                }
                AM = AM65_ZPX_IND;
            } else if (*L == ')') {
                /* zp indirect or zp indirect, y */
                L = SkipSpace (L+1);
                if (*L == ',') {
                    L = SkipSpace (L+1);
                    if (toupper (*L) != 'Y') {
                        Error ("ASM code error: `Y' expected");
                        return 0;
                    }
                    L = SkipSpace (L+1);
                    if (*L != '\0') {
                        Error ("ASM code error: syntax error");
                        return 0;
                    }
                    AM = AM65_ZP_INDY;
                } else if (*L == '\0') {
                    AM = AM65_ZP_IND;
                } else {
                    Error ("ASM code error: syntax error");
                    return 0;
                }
            }
            break;

        case 'a':
        case 'A':
            /* Accumulator? */
            if (L[1] == '\0') {
                AM = AM65_ACC;
                break;
            }
            /* FALLTHROUGH */

        default:
            /* Absolute, maybe indexed */
            L = ReadToken (L, ",", Arg, sizeof (Arg));
            if (*L == '\0') {
                /* Absolute, zeropage or branch */
                if ((OPC->Info & OF_BRA) != 0) {
                    /* Branch */
                    AM = AM65_BRA;
                } else if (GetZPInfo(Arg) != 0) {
                    AM = AM65_ZP;
                } else {
                    /* Check for subroutine call to local label */
                    if ((OPC->Info & OF_CALL) && IsLocalLabelName (Arg)) {
                        Error ("ASM code error: "
                               "Cannot use local label `%s' in subroutine call",
                               Arg);
                    }
                    AM = AM65_ABS;
                }
            } else if (*L == ',') {
                /* Indexed */
                L = SkipSpace (L+1);
                if (*L == '\0') {
                    Error ("ASM code error: syntax error");
                    return 0;
                } else {
                    Reg = toupper (*L);
                    L = SkipSpace (L+1);
                    if (Reg == 'X') {
                        if (GetZPInfo(Arg) != 0) {
                            AM = AM65_ZPX;
                        } else {
                            AM = AM65_ABSX;
                        }
                    } else if (Reg == 'Y') {
                        AM = AM65_ABSY;
                    } else {
                        Error ("ASM code error: syntax error");
                        return 0;
                    }
                    if (*L != '\0') {
                        Error ("ASM code error: syntax error");
                        return 0;
                    }
                }
            }
            break;

    }

    /* If the instruction is a branch, check for the label and generate it
    ** if it does not exist. This may lead to unused labels (if the label
    ** is actually an external one) which are removed by the CS_MergeLabels
    ** function later.
    */
    Label = 0;
    if (AM == AM65_BRA) {

        /* Generate the hash over the label, then search for the label */
        unsigned Hash = HashStr (Arg) % CS_LABEL_HASH_SIZE;
        Label = CS_FindLabel (S, Arg, Hash);

        /* If we don't have the label, it's a forward ref - create it */
        if (Label == 0) {
            /* Generate a new label */
            Label = CS_NewCodeLabel (S, Arg, Hash);
        }
    }

    /* We do now have the addressing mode in AM. Allocate a new CodeEntry
    ** structure and initialize it.
    */
    E = NewCodeEntry (OPC->OPC, AM, Arg, Label, LI);

    /* Return the new code entry */
    return E;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



CodeSeg* NewCodeSeg (const char* SegName, SymEntry* Func)
/* Create a new code segment, initialize and return it */
{
    unsigned I;
    const Type* RetType;

    /* Allocate memory */
    CodeSeg* S = xmalloc (sizeof (CodeSeg));

    /* Initialize the fields */
    S->SegName  = xstrdup (SegName);
    S->Func     = Func;
    InitCollection (&S->Entries);
    InitCollection (&S->Labels);
    for (I = 0; I < sizeof(S->LabelHash) / sizeof(S->LabelHash[0]); ++I) {
        S->LabelHash[I] = 0;
    }

    /* If we have a function given, get the return type of the function.
    ** Assume ANY return type besides void will use the A and X registers.
    */
    if (S->Func && !IsTypeVoid ((RetType = GetFuncReturn (Func->Type)))) {
        if (SizeOf (RetType) == SizeOf (type_long)) {
            S->ExitRegs = REG_EAX;
        } else {
            S->ExitRegs = REG_AX;
        }
    } else {
        S->ExitRegs = REG_NONE;
    }

    /* Copy the global optimization settings */
    S->Optimize       = (unsigned char) IS_Get (&Optimize);
    S->CodeSizeFactor = (unsigned) IS_Get (&CodeSizeFactor);

    /* Return the new struct */
    return S;
}



void CS_AddEntry (CodeSeg* S, struct CodeEntry* E)
/* Add an entry to the given code segment */
{
    /* Transfer the labels if we have any */
    CS_MoveLabelsToEntry (S, E);

    /* Add the entry to the list of code entries in this segment */
    CollAppend (&S->Entries, E);
}



void CS_AddVLine (CodeSeg* S, LineInfo* LI, const char* Format, va_list ap)
/* Add a line to the given code segment */
{
    const char* L;
    CodeEntry*  E;
    char        Token[IDENTSIZE+10];

    /* Format the line */
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;
    SB_VPrintf (&Buf, Format, ap);

    /* Skip whitespace */
    L = SkipSpace (SB_GetConstBuf (&Buf));

    /* Check which type of instruction we have */
    E = 0;      /* Assume no insn created */
    switch (*L) {

        case '\0':
            /* Empty line, just ignore it */
            break;

        case ';':
            /* Comment or hint, ignore it for now */
            break;

        case '.':
            /* Control instruction */
            ReadToken (L, " \t", Token, sizeof (Token));
            Error ("ASM code error: Pseudo instruction `%s' not supported", Token);
            break;

        default:
            E = ParseInsn (S, LI, L);
            break;
    }

    /* If we have a code entry, transfer the labels and insert it */
    if (E) {
        CS_AddEntry (S, E);
    }

    /* Cleanup the string buffer */
    SB_Done (&Buf);
}



void CS_AddLine (CodeSeg* S, LineInfo* LI, const char* Format, ...)
/* Add a line to the given code segment */
{
    va_list ap;
    va_start (ap, Format);
    CS_AddVLine (S, LI, Format, ap);
    va_end (ap);
}



void CS_InsertEntry (CodeSeg* S, struct CodeEntry* E, unsigned Index)
/* Insert the code entry at the index given. Following code entries will be
** moved to slots with higher indices.
*/
{
    /* Insert the entry into the collection */
    CollInsert (&S->Entries, E, Index);
}



void CS_DelEntry (CodeSeg* S, unsigned Index)
/* Delete an entry from the code segment. This includes moving any associated
** labels, removing references to labels and even removing the referenced labels
** if the reference count drops to zero.
** Note: Labels are moved forward if possible, that is, they are moved to the
** next insn (not the preceeding one).
*/
{
    /* Get the code entry for the given index */
    CodeEntry* E = CS_GetEntry (S, Index);

    /* If the entry has a labels, we have to move this label to the next insn.
    ** If there is no next insn, move the label into the code segement label
    ** pool. The operation is further complicated by the fact that the next
    ** insn may already have a label. In that case change all reference to
    ** this label and delete the label instead of moving it.
    */
    unsigned Count = CE_GetLabelCount (E);
    if (Count > 0) {

        /* The instruction has labels attached. Check if there is a next
        ** instruction.
        */
        if (Index == CS_GetEntryCount (S)-1) {

            /* No next instruction, move to the codeseg label pool */
            CS_MoveLabelsToPool (S, E);

        } else {

            /* There is a next insn, get it */
            CodeEntry* N = CS_GetEntry (S, Index+1);

            /* Move labels to the next entry */
            CS_MoveLabels (S, E, N);

        }
    }

    /* If this insn references a label, remove the reference. And, if the
    ** the reference count for this label drops to zero, remove this label.
    */
    if (E->JumpTo) {
        /* Remove the reference */
        CS_RemoveLabelRef (S, E);
    }

    /* Delete the pointer to the insn */
    CollDelete (&S->Entries, Index);

    /* Delete the instruction itself */
    FreeCodeEntry (E);
}



void CS_DelEntries (CodeSeg* S, unsigned Start, unsigned Count)
/* Delete a range of code entries. This includes removing references to labels,
** labels attached to the entries and so on.
*/
{
    /* Start deleting the entries from the rear, because this involves less
    ** memory moving.
    */
    while (Count--) {
        CS_DelEntry (S, Start + Count);
    }
}



void CS_MoveEntries (CodeSeg* S, unsigned Start, unsigned Count, unsigned NewPos)
/* Move a range of entries from one position to another. Start is the index
** of the first entry to move, Count is the number of entries and NewPos is
** the index of the target entry. The entry with the index Start will later
** have the index NewPos. All entries with indices NewPos and above are
** moved to higher indices. If the code block is moved to the end of the
** current code, and if pending labels exist, these labels will get attached
** to the first instruction of the moved block (the first one after the
** current code end)
*/
{
    /* Transparently handle an empty range */
    if (Count == 0) {
        return;
    }

    /* If NewPos is at the end of the code segment, move any labels from the
    ** label pool to the first instruction of the moved range.
    */
    if (NewPos == CS_GetEntryCount (S)) {
        CS_MoveLabelsToEntry (S, CS_GetEntry (S, Start));
    }

    /* Move the code block to the destination */
    CollMoveMultiple (&S->Entries, Start, Count, NewPos);
}



struct CodeEntry* CS_GetPrevEntry (CodeSeg* S, unsigned Index)
/* Get the code entry preceeding the one with the index Index. If there is no
** preceeding code entry, return NULL.
*/
{
    if (Index == 0) {
        /* This is the first entry */
        return 0;
    } else {
        /* Previous entry available */
        return CollAtUnchecked (&S->Entries, Index-1);
    }
}



struct CodeEntry* CS_GetNextEntry (CodeSeg* S, unsigned Index)
/* Get the code entry following the one with the index Index. If there is no
** following code entry, return NULL.
*/
{
    if (Index >= CollCount (&S->Entries)-1) {
        /* This is the last entry */
        return 0;
    } else {
        /* Code entries left */
        return CollAtUnchecked (&S->Entries, Index+1);
    }
}



int CS_GetEntries (CodeSeg* S, struct CodeEntry** List,
                   unsigned Start, unsigned Count)
/* Get Count code entries into List starting at index start. Return true if
** we got the lines, return false if not enough lines were available.
*/
{
    /* Check if enough entries are available */
    if (Start + Count > CollCount (&S->Entries)) {
        return 0;
    }

    /* Copy the entries */
    while (Count--) {
        *List++ = CollAtUnchecked (&S->Entries, Start++);
    }

    /* We have the entries */
    return 1;
}



unsigned CS_GetEntryIndex (CodeSeg* S, struct CodeEntry* E)
/* Return the index of a code entry */
{
    int Index = CollIndex (&S->Entries, E);
    CHECK (Index >= 0);
    return Index;
}



int CS_RangeHasLabel (CodeSeg* S, unsigned Start, unsigned Count)
/* Return true if any of the code entries in the given range has a label
** attached. If the code segment does not span the given range, check the
** possible span instead.
*/
{
    unsigned EntryCount = CS_GetEntryCount(S);

    /* Adjust count. We expect at least Start to be valid. */
    CHECK (Start < EntryCount);
    if (Start + Count > EntryCount) {
        Count = EntryCount - Start;
    }

    /* Check each entry. Since we have validated the index above, we may
    ** use the unchecked access function in the loop which is faster.
    */
    while (Count--) {
        const CodeEntry* E = CollAtUnchecked (&S->Entries, Start++);
        if (CE_HasLabel (E)) {
            return 1;
        }
    }

    /* No label in the complete range */
    return 0;
}



CodeLabel* CS_AddLabel (CodeSeg* S, const char* Name)
/* Add a code label for the next instruction to follow */
{
    /* Calculate the hash from the name */
    unsigned Hash = HashStr (Name) % CS_LABEL_HASH_SIZE;

    /* Try to find the code label if it does already exist */
    CodeLabel* L = CS_FindLabel (S, Name, Hash);

    /* Did we find it? */
    if (L) {
        /* We found it - be sure it does not already have an owner */
        if (L->Owner) {
            Error ("ASM label `%s' is already defined", Name);
            return L;
        }
    } else {
        /* Not found - create a new one */
        L = CS_NewCodeLabel (S, Name, Hash);
    }

    /* Safety. This call is quite costly, but safety is better */
    if (CollIndex (&S->Labels, L) >= 0) {
        Error ("ASM label `%s' is already defined", Name);
        return L;
    }

    /* We do now have a valid label. Remember it for later */
    CollAppend (&S->Labels, L);

    /* Return the label */
    return L;
}



CodeLabel* CS_GenLabel (CodeSeg* S, struct CodeEntry* E)
/* If the code entry E does already have a label, return it. Otherwise
** create a new label, attach it to E and return it.
*/
{
    CodeLabel* L;

    if (CE_HasLabel (E)) {

        /* Get the label from this entry */
        L = CE_GetLabel (E, 0);

    } else {

        /* Get a new name */
        const char* Name = LocalLabelName (GetLocalLabel ());

        /* Generate the hash over the name */
        unsigned Hash = HashStr (Name) % CS_LABEL_HASH_SIZE;

        /* Create a new label */
        L = CS_NewCodeLabel (S, Name, Hash);

        /* Attach this label to the code entry */
        CE_AttachLabel (E, L);

    }

    /* Return the label */
    return L;
}



void CS_DelLabel (CodeSeg* S, CodeLabel* L)
/* Remove references from this label and delete it. */
{
    unsigned Count, I;

    /* First, remove the label from the hash chain */
    CS_RemoveLabelFromHash (S, L);

    /* Remove references from insns jumping to this label */
    Count = CollCount (&L->JumpFrom);
    for (I = 0; I < Count; ++I) {
        /* Get the insn referencing this label */
        CodeEntry* E = CollAt (&L->JumpFrom, I);
        /* Remove the reference */
        CE_ClearJumpTo (E);
    }
    CollDeleteAll (&L->JumpFrom);

    /* Remove the reference to the owning instruction if it has one. The
    ** function may be called for a label without an owner when deleting
    ** unfinished parts of the code. This is unfortunate since it allows
    ** errors to slip through.
    */
    if (L->Owner) {
        CollDeleteItem (&L->Owner->Labels, L);
    }

    /* All references removed, delete the label itself */
    FreeCodeLabel (L);
}



void CS_MergeLabels (CodeSeg* S)
/* Merge code labels. That means: For each instruction, remove all labels but
** one and adjust references accordingly.
*/
{
    unsigned I;
    unsigned J;

    /* First, remove all labels from the label symbol table that don't have an
    ** owner (this means that they are actually external labels but we didn't
    ** know that previously since they may have also been forward references).
    */
    for (I = 0; I < CS_LABEL_HASH_SIZE; ++I) {

        /* Get the first label in this hash chain */
        CodeLabel** L = &S->LabelHash[I];
        while (*L) {
            if ((*L)->Owner == 0) {

                /* The label does not have an owner, remove it from the chain */
                CodeLabel* X = *L;
                *L = X->Next;

                /* Cleanup any entries jumping to this label */
                for (J = 0; J < CL_GetRefCount (X); ++J) {
                    /* Get the entry referencing this label */
                    CodeEntry* E = CL_GetRef (X, J);
                    /* And remove the reference. Do NOT call CE_ClearJumpTo
                    ** here, because this will also clear the label name,
                    ** which is not what we want.
                    */
                    E->JumpTo = 0;
                }

                /* Print some debugging output */
                if (Debug) {
                    printf ("Removing unused global label `%s'", X->Name);
                }

                /* And free the label */
                FreeCodeLabel (X);
            } else {
                /* Label is owned, point to next code label pointer */
                L = &((*L)->Next);
            }
        }
    }

    /* Walk over all code entries */
    for (I = 0; I < CS_GetEntryCount (S); ++I) {

        CodeLabel* RefLab;
        unsigned   J;

        /* Get a pointer to the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* If this entry has zero labels, continue with the next one */
        unsigned LabelCount = CE_GetLabelCount (E);
        if (LabelCount == 0) {
            continue;
        }

        /* We have at least one label. Use the first one as reference label. */
        RefLab = CE_GetLabel (E, 0);

        /* Walk through the remaining labels and change references to these
        ** labels to a reference to the one and only label. Delete the labels
        ** that are no longer used. To increase performance, walk backwards
        ** through the list.
        */
        for (J = LabelCount-1; J >= 1; --J) {

            /* Get the next label */
            CodeLabel* L = CE_GetLabel (E, J);

            /* Move all references from this label to the reference label */
            CL_MoveRefs (L, RefLab);

            /* Remove the label completely. */
            CS_DelLabel (S, L);
        }

        /* The reference label is the only remaining label. Check if there
        ** are any references to this label, and delete it if this is not
        ** the case.
        */
        if (CollCount (&RefLab->JumpFrom) == 0) {
            /* Delete the label */
            CS_DelLabel (S, RefLab);
        }
    }
}



void CS_MoveLabels (CodeSeg* S, struct CodeEntry* Old, struct CodeEntry* New)
/* Move all labels from Old to New. The routine will move the labels itself
** if New does not have any labels, and move references if there is at least
** a label for new. If references are moved, the old label is deleted
** afterwards.
*/
{
    /* Get the number of labels to move */
    unsigned OldLabelCount = CE_GetLabelCount (Old);

    /* Does the new entry have itself a label? */
    if (CE_HasLabel (New)) {

        /* The new entry does already have a label - move references */
        CodeLabel* NewLabel = CE_GetLabel (New, 0);
        while (OldLabelCount--) {

            /* Get the next label */
            CodeLabel* OldLabel = CE_GetLabel (Old, OldLabelCount);

            /* Move references */
            CL_MoveRefs (OldLabel, NewLabel);

            /* Delete the label */
            CS_DelLabel (S, OldLabel);

        }

    } else {

        /* The new entry does not have a label, just move them */
        while (OldLabelCount--) {

            /* Move the label to the new entry */
            CE_MoveLabel (CE_GetLabel (Old, OldLabelCount), New);

        }

    }
}



void CS_RemoveLabelRef (CodeSeg* S, struct CodeEntry* E)
/* Remove the reference between E and the label it jumps to. The reference
** will be removed on both sides and E->JumpTo will be 0 after that. If
** the reference was the only one for the label, the label will get
** deleted.
*/
{
    /* Get a pointer to the label and make sure it exists */
    CodeLabel* L = E->JumpTo;
    CHECK (L != 0);

    /* Delete the entry from the label */
    CollDeleteItem (&L->JumpFrom, E);

    /* The entry jumps no longer to L */
    CE_ClearJumpTo (E);

    /* If there are no more references, delete the label */
    if (CollCount (&L->JumpFrom) == 0) {
        CS_DelLabel (S, L);
    }
}



void CS_MoveLabelRef (CodeSeg* S, struct CodeEntry* E, CodeLabel* L)
/* Change the reference of E to L instead of the current one. If this
** was the only reference to the old label, the old label will get
** deleted.
*/
{
    /* Get the old label */
    CodeLabel* OldLabel = E->JumpTo;

    /* Be sure that code entry references a label */
    PRECONDITION (OldLabel != 0);

    /* Remove the reference to our label */
    CS_RemoveLabelRef (S, E);

    /* Use the new label */
    CL_AddRef (L, E);
}



void CS_DelCodeRange (CodeSeg* S, unsigned First, unsigned Last)
/* Delete all entries between first and last, both inclusive. The function
** can only handle basic blocks (First is the only entry, Last the only exit)
** and no open labels. It will call FAIL if any of these preconditions are
** violated.
*/
{
    unsigned   I;
    CodeEntry* FirstEntry;

    /* Do some sanity checks */
    CHECK (First <= Last && Last < CS_GetEntryCount (S));

    /* If Last is actually the last insn, call CS_DelCodeAfter instead, which
    ** is more flexible in this case.
    */
    if (Last == CS_GetEntryCount (S) - 1) {
        CS_DelCodeAfter (S, First);
        return;
    }

    /* Get the first entry and check if it has any labels. If it has, move
    ** them to the insn following Last. If Last is the last insn of the code
    ** segment, make them ownerless and move them to the label pool.
    */
    FirstEntry = CS_GetEntry (S, First);
    if (CE_HasLabel (FirstEntry)) {
        /* Get the entry following last */
        CodeEntry* FollowingEntry = CS_GetNextEntry (S, Last);
        if (FollowingEntry) {
            /* There is an entry after Last - move the labels */
            CS_MoveLabels (S, FirstEntry, FollowingEntry);
        } else {
            /* Move the labels to the pool and clear the owner pointer */
            CS_MoveLabelsToPool (S, FirstEntry);
        }
    }

    /* First pass: Delete all references to labels. If the reference count
    ** for a label drops to zero, delete it.
    */
    for (I = Last; I >= First; --I) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this entry has a label reference */
        if (E->JumpTo) {

            /* If the label is a label in the label pool, this is an error */
            CodeLabel* L = E->JumpTo;
            CHECK (CollIndex (&S->Labels, L) < 0);

            /* Remove the reference to the label */
            CS_RemoveLabelRef (S, E);
        }
    }

    /* Second pass: Delete the instructions. If a label attached to an
    ** instruction still has references, it must be references from outside
    ** the deleted area, which is an error.
    */
    for (I = Last; I >= First; --I) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this entry has a label attached */
        CHECK (!CE_HasLabel (E));

        /* Delete the pointer to the entry */
        CollDelete (&S->Entries, I);

        /* Delete the entry itself */
        FreeCodeEntry (E);
    }
}



void CS_DelCodeAfter (CodeSeg* S, unsigned Last)
/* Delete all entries including the given one */
{
    /* Get the number of entries in this segment */
    unsigned Count = CS_GetEntryCount (S);

    /* First pass: Delete all references to labels. If the reference count
    ** for a label drops to zero, delete it.
    */
    unsigned C = Count;
    while (Last < C--) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, C);

        /* Check if this entry has a label reference */
        if (E->JumpTo) {
            /* If the label is a label in the label pool and this is the last
            ** reference to the label, remove the label from the pool.
            */
            CodeLabel* L = E->JumpTo;
            int Index = CollIndex (&S->Labels, L);
            if (Index >= 0 && CollCount (&L->JumpFrom) == 1) {
                /* Delete it from the pool */
                CollDelete (&S->Labels, Index);
            }

            /* Remove the reference to the label */
            CS_RemoveLabelRef (S, E);
        }

    }

    /* Second pass: Delete the instructions. If a label attached to an
    ** instruction still has references, it must be references from outside
    ** the deleted area. Don't delete the label in this case, just make it
    ** ownerless and move it to the label pool.
    */
    C = Count;
    while (Last < C--) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, C);

        /* Check if this entry has a label attached */
        if (CE_HasLabel (E)) {
            /* Move the labels to the pool and clear the owner pointer */
            CS_MoveLabelsToPool (S, E);
        }

        /* Delete the pointer to the entry */
        CollDelete (&S->Entries, C);

        /* Delete the entry itself */
        FreeCodeEntry (E);
    }
}



void CS_ResetMarks (CodeSeg* S, unsigned First, unsigned Last)
/* Remove all user marks from the entries in the given range */
{
    while (First <= Last) {
        CE_ResetMark (CS_GetEntry (S, First++));
    }
}



int CS_IsBasicBlock (CodeSeg* S, unsigned First, unsigned Last)
/* Check if the given code segment range is a basic block. That is, check if
** First is the only entrance and Last is the only exit. This means that no
** jump/branch inside the block may jump to an insn below First or after(!)
** Last, and that no insn may jump into this block from the outside.
*/
{
    unsigned I;

    /* Don't accept invalid ranges */
    CHECK (First <= Last);

    /* First pass: Walk over the range and remove all marks from the entries */
    CS_ResetMarks (S, First, Last);

    /* Second pass: Walk over the range checking all labels. Note: There may be
    ** label on the first insn which is ok.
    */
    I = First + 1;
    while (I <= Last) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this entry has one or more labels, if so, check which
        ** entries jump to this label.
        */
        unsigned LabelCount = CE_GetLabelCount (E);
        unsigned LabelIndex;
        for (LabelIndex = 0; LabelIndex < LabelCount; ++LabelIndex) {

            /* Get this label */
            CodeLabel* L = CE_GetLabel (E, LabelIndex);

            /* Walk over all entries that jump to this label. Check for each
            ** of the entries if it is out of the range.
            */
            unsigned RefCount = CL_GetRefCount (L);
            unsigned RefIndex;
            for (RefIndex = 0; RefIndex < RefCount; ++RefIndex) {

                /* Get the code entry that jumps here */
                CodeEntry* Ref = CL_GetRef (L, RefIndex);

                /* Walk over out complete range and check if we find the
                ** refering entry. This is cheaper than using CS_GetEntryIndex,
                ** because CS_GetEntryIndex will search the complete code
                ** segment and not just our range.
                */
                unsigned J;
                for (J = First; J <= Last; ++J) {
                    if (Ref == CS_GetEntry (S, J)) {
                        break;
                    }
                }
                if (J > Last) {
                    /* We did not find the entry. This means that the jump to
                    ** out code segment entry E came from outside the range,
                    ** which in turn means that the given range is not a basic
                    ** block.
                    */
                    CS_ResetMarks (S, First, Last);
                    return 0;
                }

                /* If we come here, we found the entry. Mark it, so we know
                ** that the branch to the label is in range.
                */
                CE_SetMark (Ref);
            }
        }

        /* Next entry */
        ++I;
    }

    /* Third pass: Walk again over the range and check all branches. If we
    ** find a branch that is not marked, its target is not inside the range
    ** (since we checked all the labels in the range before).
    */
    I = First;
    while (I <= Last) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a branch and if so, if it has a mark */
        if (E->Info & (OF_UBRA | OF_CBRA)) {
            if (!CE_HasMark (E)) {
                /* No mark means not a basic block. Before bailing out, be sure
                ** to remove the marks from the remaining entries.
                */
                CS_ResetMarks (S, I+1, Last);
                return 0;
            }

            /* Remove the mark */
            CE_ResetMark (E);
        }

        /* Next entry */
        ++I;
    }

    /* Done - this is a basic block */
    return 1;
}



void CS_OutputPrologue (const CodeSeg* S)
/* If the given code segment is a code segment for a function, output the
** assembler prologue into the file. That is: Output a comment header, switch
** to the correct segment and enter the local function scope. If the code
** segment is global, do nothing.
*/
{
    /* Get the function associated with the code segment */
    SymEntry* Func = S->Func;

    /* If the code segment is associated with a function, print a function
    ** header and enter a local scope. Be sure to switch to the correct
    ** segment before outputing the function label.
    */
    if (Func) {
        /* Get the function descriptor */
        CS_PrintFunctionHeader (S);
        WriteOutput (".segment\t\"%s\"\n\n.proc\t_%s", S->SegName, Func->Name);
        if (IsQualNear (Func->Type)) {
            WriteOutput (": near");
        } else if (IsQualFar (Func->Type)) {
            WriteOutput (": far");
        }
        WriteOutput ("\n\n");
    }

}



void CS_OutputEpilogue (const CodeSeg* S)
/* If the given code segment is a code segment for a function, output the
** assembler epilogue into the file. That is: Close the local function scope.
*/
{
    if (S->Func) {
        WriteOutput ("\n.endproc\n\n");
    }
}



void CS_Output (CodeSeg* S)
/* Output the code segment data to the output file */
{
    unsigned I;
    const LineInfo* LI;

    /* Get the number of entries in this segment */
    unsigned Count = CS_GetEntryCount (S);

    /* If the code segment is empty, bail out here */
    if (Count == 0) {
        return;
    }

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Output the segment directive */
    WriteOutput (".segment\t\"%s\"\n\n", S->SegName);

    /* Output all entries, prepended by the line information if it has changed */
    LI = 0;
    for (I = 0; I < Count; ++I) {
        /* Get the next entry */
        const CodeEntry* E = CollConstAt (&S->Entries, I);
        /* Check if the line info has changed. If so, output the source line
        ** if the option is enabled and output debug line info if the debug
        ** option is enabled.
        */
        if (E->LI != LI) {
            /* Line info has changed, remember the new line info */
            LI = E->LI;

            /* Add the source line as a comment. Beware: When line continuation
            ** was used, the line may contain newlines.
            */
            if (AddSource) {
                const char* L = LI->Line;
                WriteOutput (";\n; ");
                while (*L) {
                    const char* N = strchr (L, '\n');
                    if (N) {
                        /* We have a newline, just write the first part */
                        WriteOutput ("%.*s\n; ", (int) (N - L), L);
                        L = N+1;
                    } else {
                        /* No Newline, write as is */
                        WriteOutput ("%s\n", L);
                        break;
                    }
                }
                WriteOutput (";\n");
            }

            /* Add line debug info */
            if (DebugInfo) {
                WriteOutput ("\t.dbg\tline, \"%s\", %u\n",
                             GetInputName (LI), GetInputLine (LI));
            }
        }
        /* Output the code */
        CE_Output (E);
    }

    /* If debug info is enabled, terminate the last line number information */
    if (DebugInfo) {
        WriteOutput ("\t.dbg\tline\n");
    }

    /* Free register info */
    CS_FreeRegInfo (S);
}



void CS_FreeRegInfo (CodeSeg* S)
/* Free register infos for all instructions */
{
    unsigned I;
    for (I = 0; I < CS_GetEntryCount (S); ++I) {
        CE_FreeRegInfo (CS_GetEntry(S, I));
    }
}



void CS_GenRegInfo (CodeSeg* S)
/* Generate register infos for all instructions */
{
    unsigned I;
    RegContents Regs;           /* Initial register contents */
    RegContents* CurrentRegs;   /* Current register contents */
    int WasJump;                /* True if last insn was a jump */
    int Done;                   /* All runs done flag */

    /* Be sure to delete all register infos */
    CS_FreeRegInfo (S);

    /* We may need two runs to get back references right */
    do {

        /* Assume we're done after this run */
        Done = 1;

        /* On entry, the register contents are unknown */
        RC_Invalidate (&Regs);
        CurrentRegs = &Regs;

        /* Walk over all insns and note just the changes from one insn to the
        ** next one.
        */
        WasJump = 0;
        for (I = 0; I < CS_GetEntryCount (S); ++I) {

            CodeEntry* P;

            /* Get the next instruction */
            CodeEntry* E = CollAtUnchecked (&S->Entries, I);

            /* If the instruction has a label, we need some special handling */
            unsigned LabelCount = CE_GetLabelCount (E);
            if (LabelCount > 0) {

                /* Loop over all entry points that jump here. If these entry
                ** points already have register info, check if all values are
                ** known and identical. If all values are identical, and the
                ** preceeding instruction was not an unconditional branch, check
                ** if the register value on exit of the preceeding instruction
                ** is also identical. If all these values are identical, the
                ** value of a register is known, otherwise it is unknown.
                */
                CodeLabel* Label = CE_GetLabel (E, 0);
                unsigned Entry;
                if (WasJump) {
                    /* Preceeding insn was an unconditional branch */
                    CodeEntry* J = CL_GetRef(Label, 0);
                    if (J->RI) {
                        Regs = J->RI->Out2;
                    } else {
                        RC_Invalidate (&Regs);
                    }
                    Entry = 1;
                } else {
                    Regs = *CurrentRegs;
                    Entry = 0;
                }

                while (Entry < CL_GetRefCount (Label)) {
                    /* Get this entry */
                    CodeEntry* J = CL_GetRef (Label, Entry);
                    if (J->RI == 0) {
                        /* No register info for this entry. This means that the
                        ** instruction that jumps here is at higher addresses and
                        ** the jump is a backward jump. We need a second run to
                        ** get the register info right in this case. Until then,
                        ** assume unknown register contents.
                        */
                        Done = 0;
                        RC_Invalidate (&Regs);
                        break;
                    }
                    if (J->RI->Out2.RegA != Regs.RegA) {
                        Regs.RegA = UNKNOWN_REGVAL;
                    }
                    if (J->RI->Out2.RegX != Regs.RegX) {
                        Regs.RegX = UNKNOWN_REGVAL;
                    }
                    if (J->RI->Out2.RegY != Regs.RegY) {
                        Regs.RegY = UNKNOWN_REGVAL;
                    }
                    if (J->RI->Out2.SRegLo != Regs.SRegLo) {
                        Regs.SRegLo = UNKNOWN_REGVAL;
                    }
                    if (J->RI->Out2.SRegHi != Regs.SRegHi) {
                        Regs.SRegHi = UNKNOWN_REGVAL;
                    }
                    if (J->RI->Out2.Tmp1 != Regs.Tmp1) {
                        Regs.Tmp1 = UNKNOWN_REGVAL;
                    }
                    ++Entry;
                }

                /* Use this register info */
                CurrentRegs = &Regs;

            }

            /* Generate register info for this instruction */
            CE_GenRegInfo (E, CurrentRegs);

            /* Remember for the next insn if this insn was an uncondition branch */
            WasJump = (E->Info & OF_UBRA) != 0;

            /* Output registers for this insn are input for the next */
            CurrentRegs = &E->RI->Out;

            /* If this insn is a branch on zero flag, we may have more info on
            ** register contents for one of both flow directions, but only if
            ** there is a previous instruction.
            */
            if ((E->Info & OF_ZBRA) != 0 && (P = CS_GetPrevEntry (S, I)) != 0) {

                /* Get the branch condition */
                bc_t BC = GetBranchCond (E->OPC);

                /* Check the previous instruction */
                switch (P->OPC) {

                    case OP65_ADC:
                    case OP65_AND:
                    case OP65_DEA:
                    case OP65_EOR:
                    case OP65_INA:
                    case OP65_LDA:
                    case OP65_ORA:
                    case OP65_PLA:
                    case OP65_SBC:
                        /* A is zero in one execution flow direction */
                        if (BC == BC_EQ) {
                            E->RI->Out2.RegA = 0;
                        } else {
                            E->RI->Out.RegA = 0;
                        }
                        break;

                    case OP65_CMP:
                        /* If this is an immidiate compare, the A register has
                        ** the value of the compare later.
                        */
                        if (CE_IsConstImm (P)) {
                            if (BC == BC_EQ) {
                                E->RI->Out2.RegA = (unsigned char)P->Num;
                            } else {
                                E->RI->Out.RegA = (unsigned char)P->Num;
                            }
                        }
                        break;

                    case OP65_CPX:
                        /* If this is an immidiate compare, the X register has
                        ** the value of the compare later.
                        */
                        if (CE_IsConstImm (P)) {
                            if (BC == BC_EQ) {
                                E->RI->Out2.RegX = (unsigned char)P->Num;
                            } else {
                                E->RI->Out.RegX = (unsigned char)P->Num;
                            }
                        }
                        break;

                    case OP65_CPY:
                        /* If this is an immidiate compare, the Y register has
                        ** the value of the compare later.
                        */
                        if (CE_IsConstImm (P)) {
                            if (BC == BC_EQ) {
                                E->RI->Out2.RegY = (unsigned char)P->Num;
                            } else {
                                E->RI->Out.RegY = (unsigned char)P->Num;
                            }
                        }
                        break;

                    case OP65_DEX:
                    case OP65_INX:
                    case OP65_LDX:
                    case OP65_PLX:
                        /* X is zero in one execution flow direction */
                        if (BC == BC_EQ) {
                            E->RI->Out2.RegX = 0;
                        } else {
                            E->RI->Out.RegX = 0;
                        }
                        break;

                    case OP65_DEY:
                    case OP65_INY:
                    case OP65_LDY:
                    case OP65_PLY:
                        /* X is zero in one execution flow direction */
                        if (BC == BC_EQ) {
                            E->RI->Out2.RegY = 0;
                        } else {
                            E->RI->Out.RegY = 0;
                        }
                        break;

                    case OP65_TAX:
                    case OP65_TXA:
                        /* If the branch is a beq, both A and X are zero at the
                        ** branch target, otherwise they are zero at the next
                        ** insn.
                        */
                        if (BC == BC_EQ) {
                            E->RI->Out2.RegA = E->RI->Out2.RegX = 0;
                        } else {
                            E->RI->Out.RegA = E->RI->Out.RegX = 0;
                        }
                        break;

                    case OP65_TAY:
                    case OP65_TYA:
                        /* If the branch is a beq, both A and Y are zero at the
                        ** branch target, otherwise they are zero at the next
                        ** insn.
                        */
                        if (BC == BC_EQ) {
                            E->RI->Out2.RegA = E->RI->Out2.RegY = 0;
                        } else {
                            E->RI->Out.RegA = E->RI->Out.RegY = 0;
                        }
                        break;

                    default:
                        break;

                }
            }
        }
    } while (!Done);

}
