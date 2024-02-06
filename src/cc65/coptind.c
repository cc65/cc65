/*****************************************************************************/
/*                                                                           */
/*                                 coptind.c                                 */
/*                                                                           */
/*              Environment independent low level optimizations              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
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



/* common */
#include "cpu.h"

/* cc65 */
#include "codeent.h"
#include "coptind.h"
#include "codeinfo.h"
#include "codeopt.h"
#include "error.h"



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static int MemAccess (CodeSeg* S, unsigned From, unsigned To, const CodeEntry* N)
/* Checks a range of code entries if there are any memory accesses to N->Arg */
{
    /* Get the length of the argument */
    unsigned NLen = strlen (N->Arg);

    /* What to check for? */
    enum {
        None    = 0x00,
        Base    = 0x01,         /* Check for location without "+1" */
        Word    = 0x02,         /* Check for location with "+1" added */
    } What = None;


    /* If the argument of N is a zero page location that ends with "+1", we
    ** must also check for word accesses to the location without +1.
    */
    if (N->AM == AM65_ZP && NLen > 2 && strcmp (N->Arg + NLen - 2, "+1") == 0) {
        What |= Base;
    }

    /* If the argument is zero page indirect, we must also check for accesses
    ** to "arg+1"
    */
    if (N->AM == AM65_ZP_INDY || N->AM == AM65_ZPX_IND || N->AM == AM65_ZP_IND) {
        What |= Word;
    }

    /* Walk over all code entries */
    while (From <= To) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, From);

        /* Check if there is an argument and if this argument equals Arg in
        ** some variants.
        */
        if (E->Arg[0] != '\0') {

            unsigned ELen;

            if (strcmp (E->Arg, N->Arg) == 0) {
                /* Found an access */
                return 1;
            }

            ELen = strlen (E->Arg);
            if ((What & Base) != 0) {
                if (ELen == NLen - 2 && strncmp (E->Arg, N->Arg, NLen-2) == 0) {
                    /* Found an access */
                    return 1;
                }
            }

            if ((What & Word) != 0) {
                if (ELen == NLen + 2 && strncmp (E->Arg, N->Arg, NLen) == 0 &&
                    E->Arg[NLen] == '+' && E->Arg[NLen+1] == '1') {
                    /* Found an access */
                    return 1;
                }
            }
        }

        /* Next entry */
        ++From;
    }

    /* Nothing found */
    return 0;
}



static short ZPRegVal (unsigned short Use, const RegContents* RC)
/* Return the contents of the given zeropage register */
{
    if ((Use & REG_TMP1) != 0) {
        return RC->Tmp1;
    } else if ((Use & REG_PTR1_LO) != 0) {
        return RC->Ptr1Lo;
    } else if ((Use & REG_PTR1_HI) != 0) {
        return RC->Ptr1Hi;
    } else if ((Use & REG_SREG_LO) != 0) {
        return RC->SRegLo;
    } else if ((Use & REG_SREG_HI) != 0) {
        return RC->SRegHi;
    } else {
        return UNKNOWN_REGVAL;
    }
}



/*****************************************************************************/
/*                      Remove unused loads and stores                       */
/*****************************************************************************/



unsigned OptUnusedLoads (CodeSeg* S)
/* Remove loads of registers where the value loaded is not used later. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a register load or transfer insn */
        if ((E->Info & (OF_LOAD | OF_XFR | OF_REG_INCDEC)) != 0         &&
            (N = CS_GetNextEntry (S, I)) != 0                           &&
            !CE_UseLoadFlags (N)) {

            /* Check which sort of load or transfer it is */
            unsigned R;
            switch (E->OPC) {
                case OP65_DEA:
                case OP65_INA:
                case OP65_LDA:
                case OP65_TXA:
                case OP65_TYA:  R = REG_A;      break;
                case OP65_DEX:
                case OP65_INX:
                case OP65_LDX:
                case OP65_TAX:  R = REG_X;      break;
                case OP65_DEY:
                case OP65_INY:
                case OP65_LDY:
                case OP65_TAY:  R = REG_Y;      break;
                default:        goto NextEntry;         /* OOPS */
            }

            /* Get register usage and check if the register value is used later */
            if ((GetRegInfo (S, I+1, R) & R) == 0) {

                /* Register value is not used, remove the load */
                CS_DelEntry (S, I);

                /* Remember, we had changes. Account the deleted entry in I. */
                ++Changes;
                --I;

            }
        }

NextEntry:
        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptUnusedStores (CodeSeg* S)
/* Remove stores into zero page registers that aren't used later */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a register load or transfer insn */
        if ((E->Info & OF_STORE) != 0    &&
            E->AM == AM65_ZP             &&
            (E->Chg & REG_ZP) != 0) {

            /* Check for the zero page location. We know that there cannot be
            ** more than one zero page location involved in the store.
            */
            unsigned R = E->Chg & REG_ZP;

            /* Get register usage and check if the register value is used later */
            if ((GetRegInfo (S, I+1, R) & R) == 0) {

                /* Register value is not used, remove the load */
                CS_DelEntry (S, I);

                /* Remember, we had changes */
                ++Changes;

                /* Continue with next insn */
                continue;
            }
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptLoad3 (CodeSeg* S)
/* Remove repeated loads from one and the same memory location */
{
    unsigned Changes = 0;
    CodeEntry* Load = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Forget a preceeding load if we have a label */
        if (Load && CE_HasLabel (E)) {
            Load = 0;
        }

        /* Check if this insn is a load */
        if (E->Info & OF_LOAD) {

            CodeEntry* N;

            /* If we had a preceeding load that is identical, remove this one.
            ** If it is not identical, or we didn't have one, remember it.
            */
            if (Load != 0                               &&
                E->OPC == Load->OPC                     &&
                E->AM == Load->AM                       &&
                ((E->Arg == 0 && Load->Arg == 0) ||
                 strcmp (E->Arg, Load->Arg) == 0)       &&
                (N = CS_GetNextEntry (S, I)) != 0       &&
                (N->Info & OF_CBRA) == 0) {

                /* Now remove the call to the subroutine */
                CS_DelEntry (S, I);

                /* Remember, we had changes */
                ++Changes;

                /* Next insn */
                continue;

            } else {

                Load = E;

            }

        } else if ((E->Info & OF_CMP) == 0 && (E->Info & OF_CBRA) == 0) {
            /* Forget the first load on occurance of any insn we don't like */
            Load = 0;
        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptDupLoads (CodeSeg* S)
/* Remove loads of registers where the value loaded is already in the register. */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Assume we won't delete or replace the entry */
        int Delete = 0;
        opc_t NewOPC = OP65_INVALID;

        /* Get a pointer to the input registers of the insn */
        const RegContents* In  = &E->RI->In;

        /* Handle the different instructions */
        switch (E->OPC) {

            case OP65_LDA:
                if (RegValIsKnown (In->RegA)          && /* Value of A is known */
                    CE_IsKnownImm (E, In->RegA)       && /* Value to be loaded is known */
                    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
                    !CE_UseLoadFlags (N)) {              /* Which does not use the flags */
                    Delete = 1;
                }
                break;

            case OP65_LDX:
                if (RegValIsKnown (In->RegX)          && /* Value of X is known */
                    CE_IsKnownImm (E, In->RegX)       && /* Value to be loaded is known */
                    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
                    !CE_UseLoadFlags (N)) {              /* Which does not use the flags */
                    Delete = 1;
                }
                break;

            case OP65_LDY:
                if (RegValIsKnown (In->RegY)          && /* Value of Y is known */
                    CE_IsKnownImm (E, In->RegY)       && /* Value to be loaded is known */
                    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
                    !CE_UseLoadFlags (N)) {              /* Which does not use the flags */
                    Delete = 1;
                }
                break;

            case OP65_STA:
                /* If we store into a known zero page location, and this
                ** location does already contain the value to be stored,
                ** remove the store.
                */
                if (RegValIsKnown (In->RegA)          && /* Value of A is known */
                    E->AM == AM65_ZP                  && /* Store into zp */
                    In->RegA == ZPRegVal (E->Chg, In)) { /* Value identical */

                    Delete = 1;
                }
                break;

            case OP65_STX:
                /* If we store into a known zero page location, and this
                ** location does already contain the value to be stored,
                ** remove the store.
                */
                if (RegValIsKnown (In->RegX)          && /* Value of A is known */
                    E->AM == AM65_ZP                  && /* Store into zp */
                    In->RegX == ZPRegVal (E->Chg, In)) { /* Value identical */

                    Delete = 1;

                /* If the value in the X register is known and the same as
                ** that in the A register, replace the store by a STA. The
                ** optimizer will then remove the load instruction for X
                ** later. STX does support the zeropage,y addressing mode,
                ** so be sure to check for that.
                */
                } else if (RegValIsKnown (In->RegX)   &&
                           In->RegX == In->RegA       &&
                           E->AM != AM65_ABSY         &&
                           E->AM != AM65_ZPY) {
                    /* Use the A register instead */
                    NewOPC = OP65_STA;
                }
                break;

            case OP65_STY:
                /* If we store into a known zero page location, and this
                ** location does already contain the value to be stored,
                ** remove the store.
                */
                if (RegValIsKnown (In->RegY)          && /* Value of Y is known */
                    E->AM == AM65_ZP                  && /* Store into zp */
                    In->RegY == ZPRegVal (E->Chg, In)) { /* Value identical */

                    Delete = 1;

                /* If the value in the Y register is known and the same as
                ** that in the A register, replace the store by a STA. The
                ** optimizer will then remove the load instruction for Y
                ** later. If replacement by A is not possible try a
                ** replacement by X, but check for invalid addressing modes
                ** in this case.
                */
                } else if (RegValIsKnown (In->RegY)) {
                    if (In->RegY == In->RegA) {
                        NewOPC = OP65_STA;
                    } else if (In->RegY == In->RegX   &&
                               E->AM != AM65_ABSX     &&
                               E->AM != AM65_ZPX) {
                        NewOPC = OP65_STX;
                    }
                }
                break;

            case OP65_STZ:
                /* If we store into a known zero page location, and this
                ** location does already contain the value to be stored,
                ** remove the store.
                */
                if ((CPUIsets[CPU] & CPU_ISET_65SC02) != 0 && E->AM == AM65_ZP) {
                    if (ZPRegVal (E->Chg, In) == 0) {
                        Delete = 1;
                    }
                }
                break;

            case OP65_TAX:
                if (RegValIsKnown (In->RegA)          &&
                    In->RegA == In->RegX              &&
                    (N = CS_GetNextEntry (S, I)) != 0 &&
                    !CE_UseLoadFlags (N)) {
                    /* Value is identical and not followed by a branch */
                    Delete = 1;
                }
                break;

            case OP65_TAY:
                if (RegValIsKnown (In->RegA)            &&
                    In->RegA == In->RegY                &&
                    (N = CS_GetNextEntry (S, I)) != 0   &&
                    !CE_UseLoadFlags (N)) {
                    /* Value is identical and not followed by a branch */
                    Delete = 1;
                }
                break;

            case OP65_TXA:
                if (RegValIsKnown (In->RegX)            &&
                    In->RegX == In->RegA                &&
                    (N = CS_GetNextEntry (S, I)) != 0   &&
                    !CE_UseLoadFlags (N)) {
                    /* Value is identical and not followed by a branch */
                    Delete = 1;
                }
                break;

            case OP65_TYA:
                if (RegValIsKnown (In->RegY)            &&
                    In->RegY == In->RegA                &&
                    (N = CS_GetNextEntry (S, I)) != 0   &&
                    !CE_UseLoadFlags (N)) {
                    /* Value is identical and not followed by a branch */
                    Delete = 1;
                }
                break;

            default:
                break;

        }

        /* Delete the entry if requested */
        if (Delete) {

            /* Register value is not used, remove the load */
            CS_DelEntry (S, I);

            /* Remember, we had changes */
            ++Changes;

        } else {

            if (NewOPC != OP65_INVALID) {
                /* Replace the opcode */
                CE_ReplaceOPC (E, NewOPC);

                /* Remember, we had changes */
                ++Changes;
            }

            /* Next entry */
            ++I;

        }

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptStoreLoad (CodeSeg* S)
/* Remove a store followed by a load from the same location. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;
        CodeEntry* X;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it is a store instruction followed by a load from the
        ** same address which is itself not followed by a conditional branch.
        */
        if ((E->Info & OF_STORE) != 0                       &&
            (N = CS_GetNextEntry (S, I)) != 0               &&
            !CE_HasLabel (N)                                &&
            E->AM == N->AM                                  &&
            ((E->OPC == OP65_STA && N->OPC == OP65_LDA) ||
             (E->OPC == OP65_STX && N->OPC == OP65_LDX) ||
             (E->OPC == OP65_STY && N->OPC == OP65_LDY))    &&
            strcmp (E->Arg, N->Arg) == 0                    &&
            (X = CS_GetNextEntry (S, I+1)) != 0             &&
            !CE_UseLoadFlags (X)) {

            /* Register has already the correct value, remove the load */
            CS_DelEntry (S, I+1);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptLoadStoreLoad (CodeSeg* S)
/* Search for the sequence
**
**      ld.     xx
**      st.     yy
**      ld.     xx
**
** and remove the useless load.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[3];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if ((L[0]->OPC == OP65_LDA ||
             L[0]->OPC == OP65_LDX ||
             L[0]->OPC == OP65_LDY)                         &&
            (L[0]->AM == AM65_ABS || L[0]->AM == AM65_ZP)   &&
            !CS_RangeHasLabel (S, I+1, 3)                   &&
            CS_GetEntries (S, L+1, I+1, 2)                  &&
            (L[1]->OPC == OP65_STA ||
             L[1]->OPC == OP65_STX ||
             L[1]->OPC == OP65_STY)                         &&
            L[2]->OPC == L[0]->OPC                          &&
            L[2]->AM == L[0]->AM                            &&
            strcmp (L[0]->Arg, L[2]->Arg) == 0) {

            /* Remove the second load */
            CS_DelEntries (S, I+2, 1);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptTransfers1 (CodeSeg* S)
/* Remove transfers from one register to another and back */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;
        CodeEntry* X;
        CodeEntry* P;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if we have two transfer instructions */
        if ((E->Info & OF_XFR) != 0                 &&
            (N = CS_GetNextEntry (S, I)) != 0       &&
            !CE_HasLabel (N)                        &&
            (N->Info & OF_XFR) != 0) {

            /* Check if it's a transfer and back */
            if ((E->OPC == OP65_TAX && N->OPC == OP65_TXA && !RegXUsed (S, I+2)) ||
                (E->OPC == OP65_TAY && N->OPC == OP65_TYA && !RegYUsed (S, I+2)) ||
                (E->OPC == OP65_TXA && N->OPC == OP65_TAX && !RegAUsed (S, I+2)) ||
                (E->OPC == OP65_TYA && N->OPC == OP65_TAY && !RegAUsed (S, I+2))) {

                /* If the next insn is a conditional branch, check if the insn
                ** preceeding the first xfr will set the flags right, otherwise we
                ** may not remove the sequence.
                */
                if ((X = CS_GetNextEntry (S, I+1)) == 0) {
                    goto NextEntry;
                }
                if (CE_UseLoadFlags (X)) {
                    if (I == 0) {
                        /* No preceeding entry */
                        goto NextEntry;
                    }
                    P = CS_GetEntry (S, I-1);
                    if ((P->Info & OF_SETF) == 0) {
                        /* Does not set the flags */
                        goto NextEntry;
                    }
                }

                /* Remove both transfers */
                CS_DelEntry (S, I+1);
                CS_DelEntry (S, I);

                /* Remember, we had changes */
                ++Changes;
            }
        }

NextEntry:
        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptTransfers2 (CodeSeg* S)
/* Replace loads followed by a register transfer by a load with the second
** register if possible.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if we have a load followed by a transfer where the loaded
        ** register is not used later.
        */
        if ((E->Info & OF_LOAD) != 0                &&
            (N = CS_GetNextEntry (S, I)) != 0       &&
            !CE_HasLabel (N)                        &&
            (N->Info & OF_XFR) != 0                 &&
            (GetRegInfo (S, I+2, E->Chg & REG_ALL) & E->Chg & REG_ALL) == 0) {

            CodeEntry* X = 0;

            if (E->OPC == OP65_LDA && N->OPC == OP65_TAX) {
                /* LDA/TAX - check for the right addressing modes */
                if (E->AM == AM65_IMM ||
                    E->AM == AM65_ZP  ||
                    E->AM == AM65_ABS ||
                    E->AM == AM65_ABSY) {
                    /* Replace */
                    X = NewCodeEntry (OP65_LDX, E->AM, E->Arg, 0, N->LI);
                }
            } else if (E->OPC == OP65_LDA && N->OPC == OP65_TAY) {
                /* LDA/TAY - check for the right addressing modes */
                if (E->AM == AM65_IMM ||
                    E->AM == AM65_ZP  ||
                    E->AM == AM65_ZPX ||
                    E->AM == AM65_ABS ||
                    E->AM == AM65_ABSX) {
                    /* Replace */
                    X = NewCodeEntry (OP65_LDY, E->AM, E->Arg, 0, N->LI);
                }
            } else if (E->OPC == OP65_LDY && N->OPC == OP65_TYA) {
                /* LDY/TYA. LDA supports all addressing modes LDY does */
                X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, N->LI);
            } else if (E->OPC == OP65_LDX && N->OPC == OP65_TXA) {
                /* LDX/TXA. LDA doesn't support zp,y, so we must map it to
                ** abs,y instead.
                */
                am_t AM = (E->AM == AM65_ZPY)? AM65_ABSY : E->AM;
                X = NewCodeEntry (OP65_LDA, AM, E->Arg, 0, N->LI);
            }

            /* If we have a load entry, add it and remove the old stuff */
            if (X) {
                CS_InsertEntry (S, X, I+2);
                CS_DelEntries (S, I, 2);
                ++Changes;
                --I;    /* Correct for one entry less */
            }
        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptTransfers3 (CodeSeg* S)
/* Replace a register transfer followed by a store of the second register by a
** store of the first register if this is possible.
*/
{
    unsigned Changes      = 0;
    unsigned UsedRegs     = REG_NONE;   /* Track used registers */
    unsigned Xfer         = 0;          /* Index of transfer insn */
    unsigned Store        = 0;          /* Index of store insn */
    CodeEntry* XferEntry  = 0;          /* Pointer to xfer insn */
    CodeEntry* StoreEntry = 0;          /* Pointer to store insn */

    enum {
        Initialize,
        Search,
        FoundXfer,
        FoundStore
    } State = Initialize;

    /* Walk over the entries. Look for a xfer instruction that is followed by
    ** a store later, where the value of the register is not used later.
    */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        switch (State) {

            case Initialize:
                /* Clear the list of used registers */
                UsedRegs = REG_NONE;
                /* FALLTHROUGH */

            case Search:
                if (E->Info & OF_XFR) {
                    /* Found start of sequence */
                    Xfer = I;
                    XferEntry = E;
                    State = FoundXfer;
                }
                break;

            case FoundXfer:
                /* If we find a conditional jump, abort the sequence, since
                ** handling them makes things really complicated.
                */
                if (E->Info & OF_CBRA) {

                    /* Switch back to searching */
                    I = Xfer;
                    State = Initialize;

                /* Does this insn use the target register of the transfer? */
                } else if ((E->Use & XferEntry->Chg) != 0) {

                    /* It it's a store instruction, and the block is a basic
                    ** block, proceed. Otherwise restart
                    */
                    if ((E->Info & OF_STORE) != 0       &&
                        CS_IsBasicBlock (S, Xfer, I)) {
                        Store = I;
                        StoreEntry = E;
                        State = FoundStore;
                    } else {
                        I = Xfer;
                        State = Initialize;
                    }

                /* Does this insn change the target register of the transfer? */
                } else if (E->Chg & XferEntry->Chg & ~PSTATE_ZN) {

                    /* We *may* add code here to remove the transfer, but I'm
                    ** currently not sure about the consequences, so I won't
                    ** do that and bail out instead.
                    */
                    I = Xfer;
                    State = Initialize;

                /* Does this insn have a label? */
                } else if (CE_HasLabel (E)) {

                    /* Too complex to handle - bail out */
                    I = Xfer;
                    State = Initialize;

                } else {
                    /* Track used registers */
                    UsedRegs |= E->Use;
                }
                break;

            case FoundStore:
                /* We are at the instruction behind the store. If the register
                ** isn't used later, and we have an address mode match, we can
                ** replace the transfer by a store and remove the store here.
                */
                if ((GetRegInfo (S, I, XferEntry->Chg & REG_ALL) &
                    XferEntry->Chg & REG_ALL) == 0                              &&
                    (StoreEntry->AM == AM65_ABS ||
                     StoreEntry->AM == AM65_ZP)                                 &&
                    (StoreEntry->AM != AM65_ZP ||
                     (StoreEntry->Chg & UsedRegs) == 0)                         &&
                    !MemAccess (S, Xfer+1, Store-1, StoreEntry)) {

                    /* Generate the replacement store insn */
                    CodeEntry* X = 0;
                    switch (XferEntry->OPC) {

                        case OP65_TXA:
                            X = NewCodeEntry (OP65_STX,
                                              StoreEntry->AM,
                                              StoreEntry->Arg,
                                              0,
                                              StoreEntry->LI);
                            break;

                        case OP65_TAX:
                            X = NewCodeEntry (OP65_STA,
                                              StoreEntry->AM,
                                              StoreEntry->Arg,
                                              0,
                                              StoreEntry->LI);
                            break;

                        case OP65_TYA:
                            X = NewCodeEntry (OP65_STY,
                                              StoreEntry->AM,
                                              StoreEntry->Arg,
                                              0,
                                              StoreEntry->LI);
                            break;

                        case OP65_TAY:
                            X = NewCodeEntry (OP65_STA,
                                              StoreEntry->AM,
                                              StoreEntry->Arg,
                                              0,
                                              StoreEntry->LI);
                            break;

                        default:
                            break;
                    }

                    /* If we have a replacement store, change the code */
                    if (X) {
                        /* Insert after the xfer insn */
                        CS_InsertEntry (S, X, Xfer+1);

                        /* Remove the xfer instead */
                        CS_DelEntry (S, Xfer);

                        /* Remove the final store */
                        CS_DelEntry (S, Store);

                        /* Correct I so we continue with the next insn */
                        I -= 2;

                        /* Remember we had changes */
                        ++Changes;
                    } else {
                        /* Restart after last xfer insn */
                        I = Xfer;
                    }
                } else {
                    /* Restart after last xfer insn */
                    I = Xfer;
                }
                State = Initialize;
                break;

        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptTransfers4 (CodeSeg* S)
/* Replace a load of a register followed by a transfer insn of the same register
** by a load of the second register if possible.
*/
{
    unsigned Changes      = 0;
    unsigned Load         = 0;  /* Index of load insn */
    unsigned Xfer         = 0;  /* Index of transfer insn */
    CodeEntry* LoadEntry  = 0;  /* Pointer to load insn */
    CodeEntry* XferEntry  = 0;  /* Pointer to xfer insn */

    enum {
        Search,
        FoundLoad,
        FoundXfer
    } State = Search;

    /* Walk over the entries. Look for a load instruction that is followed by
    ** a load later.
    */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        switch (State) {

            case Search:
                if (E->Info & OF_LOAD) {
                    /* Found start of sequence */
                    Load = I;
                    LoadEntry = E;
                    State = FoundLoad;
                }
                break;

            case FoundLoad:
                /* If we find a conditional jump, abort the sequence, since
                ** handling them makes things really complicated.
                */
                if (E->Info & OF_CBRA) {

                    /* Switch back to searching */
                    I = Load;
                    State = Search;

                /* Does this insn use the target register of the load? */
                } else if ((E->Use & LoadEntry->Chg) != 0) {

                    /* It it's a xfer instruction, and the block is a basic
                    ** block, proceed. Otherwise restart
                    */
                    if ((E->Info & OF_XFR) != 0       &&
                        CS_IsBasicBlock (S, Load, I)) {
                        Xfer = I;
                        XferEntry = E;
                        State = FoundXfer;
                    } else {
                        I = Load;
                        State = Search;
                    }

                /* Does this insn change the target register of the load? */
                } else if (E->Chg & LoadEntry->Chg & ~PSTATE_ZN) {

                    /* We *may* add code here to remove the load, but I'm
                    ** currently not sure about the consequences, so I won't
                    ** do that and bail out instead.
                    */
                    I = Load;
                    State = Search;
                }
                break;

            case FoundXfer:
                /* We are at the instruction behind the xfer. If the register
                ** isn't used later, and we have an address mode match, we can
                ** replace the transfer by a load and remove the initial load.
                */
                if ((GetRegInfo (S, I, LoadEntry->Chg & REG_ALL) &
                    LoadEntry->Chg & REG_ALL) == 0                              &&
                    (LoadEntry->AM == AM65_ABS ||
                     LoadEntry->AM == AM65_ZP  ||
                     LoadEntry->AM == AM65_IMM)                                 &&
                    !MemAccess (S, Load+1, Xfer-1, LoadEntry)) {

                    /* Generate the replacement load insn */
                    CodeEntry* X = 0;
                    switch (XferEntry->OPC) {

                        case OP65_TXA:
                        case OP65_TYA:
                            X = NewCodeEntry (OP65_LDA,
                                              LoadEntry->AM,
                                              LoadEntry->Arg,
                                              0,
                                              LoadEntry->LI);
                            break;

                        case OP65_TAX:
                            X = NewCodeEntry (OP65_LDX,
                                              LoadEntry->AM,
                                              LoadEntry->Arg,
                                              0,
                                              LoadEntry->LI);
                            break;

                        case OP65_TAY:
                            X = NewCodeEntry (OP65_LDY,
                                              LoadEntry->AM,
                                              LoadEntry->Arg,
                                              0,
                                              LoadEntry->LI);
                            break;

                        default:
                            break;
                    }

                    /* If we have a replacement load, change the code */
                    if (X) {
                        /* Insert after the xfer insn */
                        CS_InsertEntry (S, X, Xfer+1);

                        /* Remove the xfer instead */
                        CS_DelEntry (S, Xfer);

                        /* Remove the initial load */
                        CS_DelEntry (S, Load);

                        /* Correct I so we continue with the next insn */
                        I -= 2;

                        /* Remember we had changes */
                        ++Changes;
                    } else {
                        /* Restart after last xfer insn */
                        I = Xfer;
                    }
                } else {
                    /* Restart after last xfer insn */
                    I = Xfer;
                }
                State = Search;
                break;

        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPushPop1 (CodeSeg* S)
/* Remove a PHA/PLA sequence were A not used later */
{
    unsigned Changes = 0;
    unsigned Push    = 0;       /* Index of push insn */
    unsigned Pop     = 0;       /* Index of pop insn */
    unsigned ChgA    = 0;       /* Flag for A changed */
    enum {
        Searching,
        FoundPush,
        FoundPop
    } State = Searching;

    /* Walk over the entries. Look for a push instruction that is followed by
    ** a pop later, where the pop is not followed by an conditional branch,
    ** and where the value of the A register is not used later on.
    ** Look out for the following problems:
    **
    **  - There may be another PHA/PLA inside the sequence: Restart it.
    **  - If the PLA has a label, all jumps to this label must be inside
    **    the sequence, otherwise we cannot remove the PHA/PLA.
    */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* X;
        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        switch (State) {

            case Searching:
                if (E->OPC == OP65_PHA) {
                    /* Found start of sequence */
                    Push  = I;
                    ChgA  = 0;
                    State = FoundPush;
                }
                break;

            case FoundPush:
                if (E->OPC == OP65_PHA) {
                    /* Inner push/pop, restart */
                    Push = I;
                    ChgA = 0;
                } else if (E->OPC == OP65_PLA) {
                    /* Found a matching pop */
                    Pop = I;
                    /* Check that the block between Push and Pop is a basic
                    ** block (one entry, one exit). Otherwise ignore it.
                    */
                    if (CS_IsBasicBlock (S, Push, Pop)) {
                        State = FoundPop;
                    } else {
                        /* Go into searching mode again */
                        State = Searching;
                    }
                } else if (E->Chg & REG_A) {
                    ChgA = 1;
                }
                break;

            case FoundPop:
                /* We're at the instruction after the PLA.
                ** Check for the following conditions:
                **   - If this instruction is a store of A that doesn't use
                **     another register, if the instruction does not have a
                **     label, and A is not used later, we may replace the PHA
                **     by the store and remove pla if several other conditions
                **     are met.
                **   - If this instruction is not a conditional branch, and A
                **     is either unused later, or not changed by the code
                **     between push and pop, we may remove PHA and PLA.
                */
                if (E->OPC == OP65_STA                          &&
                    (E->AM == AM65_ABS || E->AM == AM65_ZP)     &&
                    !CE_HasLabel (E)                            &&
                    ((N = CS_GetNextEntry (S, I)) == 0          ||
                     (!CE_UseLoadFlags (N)                      &&
                      !RegAUsed (S, I+1)))                      &&
                    !MemAccess (S, Push+1, Pop-1, E)) {

                    /* Insert a STA after the PHA */
                    X = NewCodeEntry (OP65_STA, E->AM, E->Arg, E->JumpTo, E->LI);
                    CS_InsertEntry (S, X, Push+1);

                    /* Remove the PHA instead */
                    CS_DelEntry (S, Push);

                    /* Remove the PLA/STA sequence */
                    CS_DelEntries (S, Pop, 2);

                    /* Correct I so we continue with the next insn */
                    I -= 2;

                    /* Remember we had changes */
                    ++Changes;

                } else if (!CE_UseLoadFlags (E) &&
                           (!RegAUsed (S, I) || !ChgA)) {

                    /* We can remove the PHA and PLA instructions */
                    CS_DelEntry (S, Pop);
                    CS_DelEntry (S, Push);

                    /* Correct I so we continue with the next insn */
                    I -= 2;

                    /* Remember we had changes */
                    ++Changes;

                }
                /* Go into search mode again */
                State = Searching;
                break;

        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPushPop2 (CodeSeg* S)
/* Remove a PHP/PLP sequence were no processor flags changed inside */
{
    unsigned Changes = 0;
    unsigned Push    = 0;       /* Index of push insn */
    unsigned Pop     = 0;       /* Index of pop insn */
    enum {
        Searching,
        FoundPush,
        FoundPop
    } State = Searching;

    /* Walk over the entries. Look for a push instruction that is followed by
    ** a pop later, where the pop is not followed by an conditional branch,
    ** and where the value of the A register is not used later on.
    ** Look out for the following problems:
    **
    **  - There may be another PHP/PLP inside the sequence: Restart it.
    **  - All jumps inside the sequence must not go outside the sequence,
    **    otherwise it would be too complicated to remove the PHP/PLP.
    */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        switch (State) {

            case Searching:
                if (E->OPC == OP65_PHP) {
                    /* Found start of sequence */
                    Push  = I;
                    State = FoundPush;
                }
                break;

            case FoundPush:
                if (E->OPC == OP65_PHP) {
                    /* Inner push/pop, restart */
                    Push = I;
                } else if (E->OPC == OP65_PLP) {
                    /* Found a matching pop */
                    Pop = I;
                    /* Check that the block between Push and Pop is a basic
                    ** block (one entry, one exit). Otherwise ignore it.
                    */
                    if (CS_IsBasicBlock (S, Push, Pop)) {
                        State = FoundPop;
                    } else {
                        /* Go into searching mode again */
                        State = Searching;
                    }
                } else if ((E->Info & OF_BRA)   == 0 &&
                           (E->Info & OF_STORE) == 0 &&
                           E->OPC != OP65_NOP        &&
                           E->OPC != OP65_TSX) {
                    /* Don't bother skipping dead code */
                    State = Searching;
                }
                break;

            case FoundPop:
                /* We can remove the PHP and PLP instructions */
                CS_DelEntry (S, Pop);
                CS_DelEntry (S, Push);

                /* Correct I so we continue with THIS insn */
                I -= 3;

                /* Remember we had changes */
                ++Changes;

                /* Go into search mode again */
                State = Searching;
                break;

        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPrecalc (CodeSeg* S)
/* Replace immediate operations with the accu where the current contents are
** known by a load of the final value.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Get pointers to the input and output registers of the insn */
        const RegContents* Out = &E->RI->Out;
        const RegContents* In  = &E->RI->In;

        /* Argument for LDn and flag */
        const char* Arg = 0;
        opc_t OPC = OP65_LDA;

        /* Handle the different instructions */
        switch (E->OPC) {

            case OP65_LDA:
                if (E->AM != AM65_IMM && RegValIsKnown (Out->RegA)) {
                    /* Result of load is known */
                    Arg = MakeHexArg (Out->RegA);
                }
                break;

            case OP65_LDX:
                if (E->AM != AM65_IMM && RegValIsKnown (Out->RegX)) {
                    /* Result of load is known but register is X */
                    Arg = MakeHexArg (Out->RegX);
                    OPC = OP65_LDX;
                }
                break;

            case OP65_LDY:
                if (E->AM != AM65_IMM && RegValIsKnown (Out->RegY)) {
                    /* Result of load is known but register is Y */
                    Arg = MakeHexArg (Out->RegY);
                    OPC = OP65_LDY;
                }
                break;

            case OP65_ADC:
            case OP65_SBC:
                if (CE_IsKnownImm (E, 0x00)) {
                    /* If this is an operation with an immediate operand of zero,
                    ** and the Z/N flags reflect the current states of the content
                    ** in A, then the operation won't give us any results we don't
                    ** already have (including the flags) as long as the C flag is
                    ** set normally (cleared for ADC and set for SBC) for the
                    ** operation. So we can remove the operation if it is the
                    ** normal case or the result in A is not used later.
                    ** Something like this is generated as a result of a compare
                    ** where parts of the values are known to be zero.
                    ** The only situation where we need to leave things as they
                    ** are is when an indeterminate V flag is being tested later,
                    ** because ADC/SBC #0 always clears it.
                    */
                    int CondC = PStatesAreKnown (In->PFlags, PSTATE_C) &&
                                ((E->OPC == OP65_ADC && (In->PFlags & PFVAL_C) == 0) ||
                                 (E->OPC == OP65_SBC && (In->PFlags & PFVAL_C) != 0));
                    int CondV = PStatesAreKnown (In->PFlags, PSTATE_V) && (In->PFlags & PFVAL_V) == 0;
                    int CondZN = (In->ZNRegs & ZNREG_A) != 0;
                    unsigned R = 0;
                    if (CondC) {
                        R = (CondV ? 0 : PSTATE_V) | (CondZN ? 0 : PSTATE_ZN);
                    } else {
                        R = REG_A | PSTATE_CZVN;
                    }
                    if (R != 0) {
                        /* Collect info on all flags in one round to save time */
                        R = GetRegInfo (S, I + 1, R);
                    }
                    CondV = (CondC && CondV) || (R & PSTATE_V) == 0;
                    CondZN = (CondC && CondZN) || (R & PSTATE_ZN) == 0;
                    /* This is done last as it could change the info used by the two above */
                    CondC = CondC || (R & (REG_A | PSTATE_C)) == 0;
                    if (CondC && CondV && CondZN) {
                        /* ?+0, ?-0 or result unused -> remove */
                        CS_DelEntry (S, I);
                        ++Changes;
                    }
                } else if (E->OPC == OP65_ADC && In->RegA == 0) {
                    /* 0 + arg. In this case we need only care about the C/V flags and
                    ** let the load set the Z/N flags properly.
                    */
                    int CondC = PStatesAreClear (In->PFlags, PSTATE_C);
                    int CondV = PStatesAreClear (In->PFlags, PSTATE_V);
                    unsigned R = (CondC ? 0 : REG_A | PSTATE_C) | (CondC && CondV ? 0 : PSTATE_V);
                    if (R) {
                        R = GetRegInfo (S, I + 1, R);
                    }
                    CondV = (CondC && CondV) || (R & PSTATE_V) == 0;
                    CondC = CondC || (R & (REG_A | PSTATE_C)) == 0;
                    if (CondC && CondV) {
                        /* 0 + arg -> replace with lda arg */
                        CE_ReplaceOPC (E, OP65_LDA);
                        ++Changes;
                    }
                }
                break;

            case OP65_AND:
                if (CE_IsKnownImm (E, 0xFF) &&
                    ((In->ZNRegs & ZNREG_A) != 0 ||
                     (GetRegInfo (S, I + 1, PSTATE_ZN) & PSTATE_ZN) == 0)) {
                    /* AND with 0xFF, remove */
                    CS_DelEntry (S, I);
                    ++Changes;
                } else if (CE_IsKnownImm (E, 0x00)) {
                    /* AND with 0x00, replace by lda #$00 */
                    Arg = MakeHexArg (0x00);
                } else if (RegValIsKnown (Out->RegA)) {
                    /* Accu AND zp with known contents */
                    Arg = MakeHexArg (Out->RegA);
                } else if (In->RegA == 0xFF) {
                    /* AND but A contains 0xFF - replace by lda */
                    CE_ReplaceOPC (E, OP65_LDA);
                    ++Changes;
                }
                break;

            case OP65_ORA:
                if (CE_IsKnownImm (E, 0x00) &&
                    ((In->ZNRegs & ZNREG_A) != 0 ||
                     (GetRegInfo (S, I + 1, PSTATE_ZN) & PSTATE_ZN) == 0)) {
                    /* ORA with zero, remove */
                    CS_DelEntry (S, I);
                    ++Changes;
                } else if (CE_IsKnownImm (E, 0xFF)) {
                    /* ORA with 0xFF, replace by lda #$ff */
                    Arg = MakeHexArg (0xFF);
                } else if (RegValIsKnown (Out->RegA)) {
                    /* Accu AND zp with known contents */
                    Arg = MakeHexArg (Out->RegA);
                } else if (In->RegA == 0) {
                    /* ORA but A contains 0x00 - replace by lda */
                    CE_ReplaceOPC (E, OP65_LDA);
                    ++Changes;
                }
                break;

            case OP65_EOR:
                if (CE_IsKnownImm (E, 0x00) &&
                    ((In->ZNRegs & ZNREG_A) != 0 ||
                     (GetRegInfo (S, I + 1, PSTATE_ZN) & PSTATE_ZN) == 0)) {
                    /* EOR with zero, remove */
                    CS_DelEntry (S, I);
                    ++Changes;
                } else if (RegValIsKnown (Out->RegA)) {
                    /* Accu op zp with known contents */
                    Arg = MakeHexArg (Out->RegA);
                } else if (In->RegA == 0) {
                    /* EOR but A contains 0x00 - replace by lda */
                    CE_ReplaceOPC (E, OP65_LDA);
                    ++Changes;
                }
                 break;

            default:
                break;

        }

        /* Check if we have to replace the insn by LDA */
        if (Arg) {
            CodeEntry* X = NewCodeEntry (OPC, AM65_IMM, Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+1);
            CS_DelEntry (S, I);
            ++Changes;
        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShiftBack (CodeSeg* S)
/* Remove a pair of shifts to the opposite directions if none of the bits of
** the register A or the Z/N flags modified by these shifts are used later.
*/
{
    unsigned Changes = 0;
    CodeEntry* E;
    CodeEntry* N;
    unsigned CheckStates;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        E = CS_GetEntry (S, I);

        /* Check if it's a register load or transfer insn */
        if (E->OPC == OP65_ROL                  &&
            (N = CS_GetNextEntry (S, I)) != 0   &&
            (N->OPC == OP65_LSR ||
             N->OPC == OP65_ROR)                &&
            !CE_HasLabel (N)) {
            CheckStates = PSTATE_ZN;
            if (N->OPC == OP65_LSR &&
                !PStatesAreClear (E->RI->Out.PFlags, PSTATE_C)) {
                CheckStates |= REG_A;
            }
            if ((GetRegInfo (S, I+2, CheckStates) & CheckStates) == 0) {

                /* Remove the shifts */
                CS_DelEntries (S, I, 2);

                /* Remember, we had changes */
                ++Changes;

                /* Continue with next insn */
                continue;
            }
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}


unsigned OptSignExtended (CodeSeg* S)
/* Change
**
**      lda     xxx     ; X is 0
**      bpl     L1
**      dex/ldx #$FF
**  L1: cpx     #$00
**      bpl     L2
**
** or
**
**      lda     xxx     ; X is 0
**      bpl     L1
**      dex/ldx #$FF
**  L1: cpx     #$80
**      bcc/bmi L2
**
** into
**      lda     xxx     ; X is 0
**      bpl     L2
**      dex/ldx #$FF
**
** provided the C flag isn't used later.
*/
{
    unsigned Changes = 0;
    CodeEntry* L[5];
    CodeEntry* X;
    unsigned CheckStates;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check if it's a register load or transfer insn */
        if (L[0]->OPC == OP65_LDA                   &&
            CS_GetEntries (S, L+1, I+1, 4)          &&
            !CS_RangeHasLabel (S, I+1, 2)           &&
            CE_GetLabelCount (L[3]) == 1            &&
            L[1]->JumpTo == CE_GetLabel (L[3], 0)   &&
            (L[1]->Info & OF_CBRA) != 0             &&
            GetBranchCond (L[1]->OPC) == BC_PL      &&
            RegValIsKnown (L[2]->RI->Out.RegX)      &&
            L[2]->RI->Out.RegX == 0xFF              &&
            L[2]->OPC != OP65_JSR                   &&
            (L[2]->Chg & REG_AXY) == REG_X) {

            /* We find a sign extention */
            CheckStates = PSTATE_CZN;
            if (L[3]->OPC == OP65_CPX                       &&
                CE_IsConstImm (L[3])                        &&
                (L[4]->Info & OF_CBRA) != 0                 &&
                ((L[3]->Num == 0x00                     &&
                  GetBranchCond (L[4]->OPC) == BC_PL)       ||
                ((L[3]->Num == 0x80                     &&
                  GetBranchCond (L[4]->OPC) == BC_CC &&
                  GetBranchCond (L[4]->OPC) == BC_MI)))) {

                /* Check if the processor states set by the CPX are unused later */
                if ((GetRegInfo (S, I+5, CheckStates) & CheckStates) == 0) {

                    /* Change the target of the sign extention branch */
                    X = NewCodeEntry (OP65_JPL, L[4]->AM, L[4]->Arg, L[4]->JumpTo, L[4]->LI);
                    CS_InsertEntry (S, X, I+1);
                    CS_DelEntry (S, I+2);

                    /* Remove the old conditional branch */
                    CS_DelEntries (S, I+3, 2);

                    /* Remember, we had changes */
                    ++Changes;

                    /* Continue with the current insn */
                    continue;
                }
            }
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
