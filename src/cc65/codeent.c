/*****************************************************************************/
/*                                                                           */
/*                                 codeent.c                                 */
/*                                                                           */
/*                            Code segment entry                             */
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



#include <stdlib.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "debugflag.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "error.h"
#include "global.h"
#include "codelab.h"
#include "opcodes.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Empty argument */
static char EmptyArg[] = "";



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void FreeArg (char* Arg)
/* Free a code entry argument */
{
    if (Arg != EmptyArg) {
        xfree (Arg);
    }
}



static char* GetArgCopy (const char* Arg)
/* Create an argument copy for assignment */
{
    if (Arg && Arg[0] != '\0') {
        /* Create a copy */
        return xstrdup (Arg);
    } else {
        /* Use the empty argument string */
        return EmptyArg;
    }
}



static int NumArg (const char* Arg, unsigned long* Num)
/* If the given argument is numerical, convert it and return true. Otherwise
** set Num to zero and return false.
*/
{
    char* End;
    unsigned long Val;

    /* Determine the base */
    int Base = 10;
    if (*Arg == '$') {
        ++Arg;
        Base = 16;
    } else if (*Arg == '%') {
        ++Arg;
        Base = 2;
    }

    /* Convert the value. strtol is not exactly what we want here, but it's
    ** cheap and may be replaced by something fancier later.
    */
    Val = strtoul (Arg, &End, Base);

    /* Check if the conversion was successful */
    if (*End != '\0') {

        /* Could not convert */
        *Num = 0;
        return 0;

    } else {

        /* Conversion ok */
        *Num = Val;
        return 1;

    }
}



static void SetUseChgInfo (CodeEntry* E, const OPCDesc* D)
/* Set the Use and Chg in E */
{
    const ZPInfo* Info;

    /* If this is a subroutine call, or a jump to an external function,
    ** lookup the information about this function and use it. The jump itself
    ** does not change any registers, so we don't need to use the data from D.
    */
    if ((E->Info & (OF_UBRA | OF_CALL)) != 0 && E->JumpTo == 0) {
        /* A subroutine call or jump to external symbol (function exit) */
        GetFuncInfo (E->Arg, &E->Use, &E->Chg);
    } else {
        /* Some other instruction. Use the values from the opcode description
        ** plus addressing mode info.
        */
        E->Use = D->Use | GetAMUseInfo (E->AM);
        E->Chg = D->Chg;

        /* Check for special zero page registers used */
        switch (E->AM) {

            case AM65_ACC:
                if (E->OPC == OP65_ASL || E->OPC == OP65_DEC ||
                    E->OPC == OP65_INC || E->OPC == OP65_LSR ||
                    E->OPC == OP65_ROL || E->OPC == OP65_ROR) {
                    /* A is changed by these insns */
                    E->Chg |= REG_A;
                }
                break;

            case AM65_ZP:
            case AM65_ABS:
            /* Be conservative: */
            case AM65_ZPX:
            case AM65_ABSX:
            case AM65_ABSY:
                Info = GetZPInfo (E->Arg);
                if (Info && Info->ByteUse != REG_NONE) {
                    if (E->OPC == OP65_ASL || E->OPC == OP65_DEC ||
                        E->OPC == OP65_INC || E->OPC == OP65_LSR ||
                        E->OPC == OP65_ROL || E->OPC == OP65_ROR ||
                        E->OPC == OP65_TRB || E->OPC == OP65_TSB) {
                        /* The zp loc is both, input and output */
                        E->Chg |= Info->ByteUse;
                        E->Use |= Info->ByteUse;
                    } else if ((E->Info & OF_STORE) != 0) {
                        /* Just output */
                        E->Chg |= Info->ByteUse;
                    } else {
                        /* Input only */
                        E->Use |= Info->ByteUse;
                    }
                }
                break;

            case AM65_ZPX_IND:
            case AM65_ZP_INDY:
            case AM65_ZP_IND:
                Info = GetZPInfo (E->Arg);
                if (Info && Info->ByteUse != REG_NONE) {
                    /* These addressing modes will never change the zp loc */
                    E->Use |= Info->WordUse;
                }
                break;

            default:
                /* Keep gcc silent */
                break;
        }
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* MakeHexArg (unsigned Num)
/* Convert Num into a string in the form $XY, suitable for passing it as an
** argument to NewCodeEntry, and return a pointer to the string.
** BEWARE: The function returns a pointer to a static buffer, so the value is
** gone if you call it twice (and apart from that it's not thread and signal
** safe).
*/
{
    static char Buf[16];
    xsprintf (Buf, sizeof (Buf), "$%02X", (unsigned char) Num);
    return Buf;
}



CodeEntry* NewCodeEntry (opc_t OPC, am_t AM, const char* Arg,
                         CodeLabel* JumpTo, LineInfo* LI)
/* Create a new code entry, initialize and return it */
{
    /* Get the opcode description */
    const OPCDesc* D = GetOPCDesc (OPC);

    /* Allocate memory */
    CodeEntry* E = xmalloc (sizeof (CodeEntry));

    /* Initialize the fields */
    E->OPC    = D->OPC;
    E->AM     = AM;
    E->Size   = GetInsnSize (E->OPC, E->AM);
    E->Arg    = GetArgCopy (Arg);
    E->Flags  = NumArg (E->Arg, &E->Num)? CEF_NUMARG : 0;   /* Needs E->Arg */
    E->Info   = D->Info;
    E->JumpTo = JumpTo;
    E->LI     = UseLineInfo (LI);
    E->RI     = 0;
    SetUseChgInfo (E, D);
    InitCollection (&E->Labels);

    /* If we have a label given, add this entry to the label */
    if (JumpTo) {
        CollAppend (&JumpTo->JumpFrom, E);
    }

    /* Return the initialized struct */
    return E;
}



void FreeCodeEntry (CodeEntry* E)
/* Free the given code entry */
{
    /* Free the string argument if we have one */
    FreeArg (E->Arg);

    /* Cleanup the collection */
    DoneCollection (&E->Labels);

    /* Release the line info */
    ReleaseLineInfo (E->LI);

    /* Delete the register info */
    CE_FreeRegInfo (E);

    /* Free the entry */
    xfree (E);
}



void CE_ReplaceOPC (CodeEntry* E, opc_t OPC)
/* Replace the opcode of the instruction. This will also replace related info,
** Size, Use and Chg, but it will NOT update any arguments or labels.
*/
{
    /* Get the opcode descriptor */
    const OPCDesc* D = GetOPCDesc (OPC);

    /* Replace the opcode */
    E->OPC  = OPC;
    E->Info = D->Info;
    E->Size = GetInsnSize (E->OPC, E->AM);
    SetUseChgInfo (E, D);
}



int CodeEntriesAreEqual (const CodeEntry* E1, const CodeEntry* E2)
/* Check if both code entries are equal */
{
    return (E1->OPC == E2->OPC && E1->AM == E2->AM && strcmp (E1->Arg, E2->Arg) == 0);
}



void CE_AttachLabel (CodeEntry* E, CodeLabel* L)
/* Attach the label to the entry */
{
    /* Add it to the entries label list */
    CollAppend (&E->Labels, L);

    /* Tell the label about it's owner */
    L->Owner = E;
}



void CE_ClearJumpTo (CodeEntry* E)
/* Clear the JumpTo entry and the argument (which contained the name of the
** label). Note: The function will not clear the backpointer from the label,
** so use it with care.
*/
{
    /* Clear the JumpTo entry */
    E->JumpTo = 0;

    /* Clear the argument and assign the empty one */
    FreeArg (E->Arg);
    E->Arg = EmptyArg;
}



void CE_MoveLabel (CodeLabel* L, CodeEntry* E)
/* Move the code label L from it's former owner to the code entry E. */
{
    /* Delete the label from the owner */
    CollDeleteItem (&L->Owner->Labels, L);

    /* Set the new owner */
    CollAppend (&E->Labels, L);
    L->Owner = E;
}



void CE_SetArg (CodeEntry* E, const char* Arg)
/* Replace the argument by the new one. */
{
    /* Free the old argument */
    FreeArg (E->Arg);

    /* Assign the new one */
    E->Arg = GetArgCopy (Arg);
}



void CE_SetNumArg (CodeEntry* E, long Num)
/* Set a new numeric argument for the given code entry that must already
** have a numeric argument.
*/
{
    char Buf[16];

    /* Check that the entry has a numerical argument */
    CHECK (E->Flags & CEF_NUMARG);

    /* Make the new argument string */
    if (E->Size == 2) {
        Num &= 0xFF;
        xsprintf (Buf, sizeof (Buf), "$%02X", (unsigned) Num);
    } else if (E->Size == 3) {
        Num &= 0xFFFF;
        xsprintf (Buf, sizeof (Buf), "$%04X", (unsigned) Num);
    } else {
        Internal ("Invalid instruction size in CE_SetNumArg");
    }

    /* Replace the argument by the new one */
    CE_SetArg (E, Buf);

    /* Use the new numerical value */
    E->Num = Num;
}



int CE_IsConstImm (const CodeEntry* E)
/* Return true if the argument of E is a constant immediate value */
{
    return (E->AM == AM65_IMM && CE_HasNumArg (E));
}



int CE_IsKnownImm (const CodeEntry* E, unsigned long Num)
/* Return true if the argument of E is a constant immediate value that is
** equal to Num.
*/
{
    return (E->AM == AM65_IMM && CE_HasNumArg (E) && E->Num == Num);
}



int CE_UseLoadFlags (CodeEntry* E)
/* Return true if the instruction uses any flags that are set by a load of
** a register (N and Z).
*/
{
    /* Follow unconditional branches, but beware of endless loops. After this,
    ** E will point to the first entry that is not a branch.
    */
    if (E->Info & OF_UBRA) {
        Collection C = AUTO_COLLECTION_INITIALIZER;

        /* Follow the chain */
        while (E->Info & OF_UBRA) {

            /* Remember the entry so we can detect loops */
            CollAppend (&C, E);

            /* Check the target */
            if (E->JumpTo == 0 || CollIndex (&C, E->JumpTo->Owner) >= 0) {
                /* Unconditional jump to external symbol, or endless loop. */
                DoneCollection (&C);
                return 0;       /* Flags not used */
            }

            /* Follow the chain */
            E = E->JumpTo->Owner;
        }

        /* Delete the collection */
        DoneCollection (&C);
    }

    /* A branch will use the flags */
    if (E->Info & OF_FBRA) {
        return 1;
    }

    /* Call of a boolean transformer routine will also use the flags */
    if (E->OPC == OP65_JSR) {
        /* Get the condition that is evaluated and check it */
        switch (FindBoolCmpCond (E->Arg)) {
            case CMP_EQ:
            case CMP_NE:
            case CMP_GT:
            case CMP_GE:
            case CMP_LT:
            case CMP_LE:
            case CMP_UGT:
            case CMP_ULE:
                /* Will use the N or Z flags */
                return 1;


            case CMP_UGE:       /* Uses only carry */
            case CMP_ULT:       /* Dito */
            default:            /* No bool transformer subroutine */
                return 0;
        }
    }

    /* Anything else */
    return 0;
}



void CE_FreeRegInfo (CodeEntry* E)
/* Free an existing register info struct */
{
    if (E->RI) {
        FreeRegInfo (E->RI);
        E->RI = 0;
    }
}



void CE_GenRegInfo (CodeEntry* E, RegContents* InputRegs)
/* Generate register info for this instruction. If an old info exists, it is
** overwritten.
*/
{
    /* Pointers to the register contents */
    RegContents* In;
    RegContents* Out;

    /* Function register usage */
    unsigned short Use, Chg;

    /* If we don't have a register info struct, allocate one. */
    if (E->RI == 0) {
        E->RI = NewRegInfo (InputRegs);
    } else {
        if (InputRegs) {
            E->RI->In  = *InputRegs;
        } else {
            RC_Invalidate (&E->RI->In);
        }
        E->RI->Out2 = E->RI->Out = E->RI->In;
    }

    /* Get pointers to the register contents */
    In  = &E->RI->In;
    Out = &E->RI->Out;

    /* Handle the different instructions */
    switch (E->OPC) {

        case OP65_ADC:
            /* We don't know the value of the carry, so the result is
            ** always unknown.
            */
            Out->RegA = UNKNOWN_REGVAL;
            break;

        case OP65_AND:
            if (RegValIsKnown (In->RegA)) {
                if (CE_IsConstImm (E)) {
                    Out->RegA = In->RegA & (short) E->Num;
                } else if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            Out->RegA = In->RegA & In->Tmp1;
                            break;
                        case REG_PTR1_LO:
                            Out->RegA = In->RegA & In->Ptr1Lo;
                            break;
                        case REG_PTR1_HI:
                            Out->RegA = In->RegA & In->Ptr1Hi;
                            break;
                        case REG_SREG_LO:
                            Out->RegA = In->RegA & In->SRegLo;
                            break;
                        case REG_SREG_HI:
                            Out->RegA = In->RegA & In->SRegHi;
                            break;
                        default:
                            Out->RegA = UNKNOWN_REGVAL;
                            break;
                    }
                } else {
                    Out->RegA = UNKNOWN_REGVAL;
                }
            } else if (CE_IsKnownImm (E, 0)) {
                /* A and $00 does always give zero */
                Out->RegA = 0;
            }
            break;

        case OP65_ASL:
            if (E->AM == AM65_ACC && RegValIsKnown (In->RegA)) {
                Out->RegA = (In->RegA << 1) & 0xFF;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->Tmp1 = (In->Tmp1 << 1) & 0xFF;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = (In->Ptr1Lo << 1) & 0xFF;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = (In->Ptr1Hi << 1) & 0xFF;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = (In->SRegLo << 1) & 0xFF;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = (In->SRegHi << 1) & 0xFF;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_BCC:
            break;

        case OP65_BCS:
            break;

        case OP65_BEQ:
            break;

        case OP65_BIT:
            break;

        case OP65_BMI:
            break;

        case OP65_BNE:
            break;

        case OP65_BPL:
            break;

        case OP65_BRA:
            break;

        case OP65_BRK:
            break;

        case OP65_BVC:
            break;

        case OP65_BVS:
            break;

        case OP65_CLC:
            break;

        case OP65_CLD:
            break;

        case OP65_CLI:
            break;

        case OP65_CLV:
            break;

        case OP65_CMP:
            break;

        case OP65_CPX:
            break;

        case OP65_CPY:
            break;

        case OP65_DEA:
            if (RegValIsKnown (In->RegA)) {
                Out->RegA = (In->RegA - 1) & 0xFF;
            }
            break;

        case OP65_DEC:
            if (E->AM == AM65_ACC && RegValIsKnown (In->RegA)) {
                Out->RegA = (In->RegA - 1) & 0xFF;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->Tmp1 = (In->Tmp1 - 1) & 0xFF;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = (In->Ptr1Lo - 1) & 0xFF;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = (In->Ptr1Hi - 1) & 0xFF;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = (In->SRegLo - 1) & 0xFF;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = (In->SRegHi - 1) & 0xFF;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_DEX:
            if (RegValIsKnown (In->RegX)) {
                Out->RegX = (In->RegX - 1) & 0xFF;
            }
            break;

        case OP65_DEY:
            if (RegValIsKnown (In->RegY)) {
                Out->RegY = (In->RegY - 1) & 0xFF;
            }
            break;

        case OP65_EOR:
            if (RegValIsKnown (In->RegA)) {
                if (CE_IsConstImm (E)) {
                    Out->RegA = In->RegA ^ (short) E->Num;
                } else if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            Out->RegA = In->RegA ^ In->Tmp1;
                            break;
                        case REG_PTR1_LO:
                            Out->RegA = In->RegA ^ In->Ptr1Lo;
                            break;
                        case REG_PTR1_HI:
                            Out->RegA = In->RegA ^ In->Ptr1Hi;
                            break;
                        case REG_SREG_LO:
                            Out->RegA = In->RegA ^ In->SRegLo;
                            break;
                        case REG_SREG_HI:
                            Out->RegA = In->RegA ^ In->SRegHi;
                            break;
                        default:
                            Out->RegA = UNKNOWN_REGVAL;
                            break;
                    }
                } else {
                    Out->RegA = UNKNOWN_REGVAL;
                }
            }
            break;

        case OP65_INA:
            if (RegValIsKnown (In->RegA)) {
                Out->RegA = (In->RegA + 1) & 0xFF;
            }
            break;

        case OP65_INC:
            if (E->AM == AM65_ACC && RegValIsKnown (In->RegA)) {
                Out->RegA = (In->RegA + 1) & 0xFF;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->Tmp1 = (In->Tmp1 + 1) & 0xFF;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = (In->Ptr1Lo + 1) & 0xFF;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = (In->Ptr1Hi + 1) & 0xFF;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = (In->SRegLo + 1) & 0xFF;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = (In->SRegHi + 1) & 0xFF;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_INX:
            if (RegValIsKnown (In->RegX)) {
                Out->RegX = (In->RegX + 1) & 0xFF;
            }
            break;

        case OP65_INY:
            if (RegValIsKnown (In->RegY)) {
                Out->RegY = (In->RegY + 1) & 0xFF;
            }
            break;

        case OP65_JCC:
            break;

        case OP65_JCS:
            break;

        case OP65_JEQ:
            break;

        case OP65_JMI:
            break;

        case OP65_JMP:
            break;

        case OP65_JNE:
            break;

        case OP65_JPL:
            break;

        case OP65_JSR:
            /* Get the code info for the function */
            GetFuncInfo (E->Arg, &Use, &Chg);
            if (Chg & REG_A) {
                Out->RegA = UNKNOWN_REGVAL;
            }
            if (Chg & REG_X) {
                Out->RegX = UNKNOWN_REGVAL;
            }
            if (Chg & REG_Y) {
                Out->RegY = UNKNOWN_REGVAL;
            }
            if (Chg & REG_TMP1) {
                Out->Tmp1 = UNKNOWN_REGVAL;
            }
            if (Chg & REG_PTR1_LO) {
                Out->Ptr1Lo = UNKNOWN_REGVAL;
            }
            if (Chg & REG_PTR1_HI) {
                Out->Ptr1Hi = UNKNOWN_REGVAL;
            }
            if (Chg & REG_SREG_LO) {
                Out->SRegLo = UNKNOWN_REGVAL;
            }
            if (Chg & REG_SREG_HI) {
                Out->SRegHi = UNKNOWN_REGVAL;
            }
            /* ## FIXME: Quick hack for some known functions: */
            if (strcmp (E->Arg, "complax") == 0) {
                if (RegValIsKnown (In->RegA)) {
                    Out->RegA = (In->RegA ^ 0xFF);
                }
                if (RegValIsKnown (In->RegX)) {
                    Out->RegX = (In->RegX ^ 0xFF);
                }
            } else if (strcmp (E->Arg, "tosandax") == 0) {
                if (In->RegA == 0) {
                    Out->RegA = 0;
                }
                if (In->RegX == 0) {
                    Out->RegX = 0;
                }
            } else if (strcmp (E->Arg, "tosaslax") == 0) {
                if (RegValIsKnown (In->RegA) && (In->RegA & 0x0F) >= 8) {
                    printf ("Hey!\n");
                    Out->RegA = 0;
                }
            } else if (strcmp (E->Arg, "tosorax") == 0) {
                if (In->RegA == 0xFF) {
                    Out->RegA = 0xFF;
                }
                if (In->RegX == 0xFF) {
                    Out->RegX = 0xFF;
                }
            } else if (strcmp (E->Arg, "tosshlax") == 0) {
                if ((In->RegA & 0x0F) >= 8) {
                    Out->RegA = 0;
                }
            } else if (FindBoolCmpCond (E->Arg) != CMP_INV ||
                       FindTosCmpCond (E->Arg) != CMP_INV) {
                /* Result is boolean value, so X is zero on output */
                Out->RegX = 0;
            }
            break;

        case OP65_JVC:
            break;

        case OP65_JVS:
            break;

        case OP65_LDA:
            if (CE_IsConstImm (E)) {
                Out->RegA = (unsigned char) E->Num;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Use & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->RegA = In->Tmp1;
                        break;
                    case REG_PTR1_LO:
                        Out->RegA = In->Ptr1Lo;
                        break;
                    case REG_PTR1_HI:
                        Out->RegA = In->Ptr1Hi;
                        break;
                    case REG_SREG_LO:
                        Out->RegA = In->SRegLo;
                        break;
                    case REG_SREG_HI:
                        Out->RegA = In->SRegHi;
                        break;
                    default:
                        Out->RegA = UNKNOWN_REGVAL;
                        break;
                }
            } else {
                /* A is now unknown */
                Out->RegA = UNKNOWN_REGVAL;
            }
            break;

        case OP65_LDX:
            if (CE_IsConstImm (E)) {
                Out->RegX = (unsigned char) E->Num;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Use & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->RegX = In->Tmp1;
                        break;
                    case REG_PTR1_LO:
                        Out->RegX = In->Ptr1Lo;
                        break;
                    case REG_PTR1_HI:
                        Out->RegX = In->Ptr1Hi;
                        break;
                    case REG_SREG_LO:
                        Out->RegX = In->SRegLo;
                        break;
                    case REG_SREG_HI:
                        Out->RegX = In->SRegHi;
                        break;
                    default:
                        Out->RegX = UNKNOWN_REGVAL;
                        break;
                }
            } else {
                /* X is now unknown */
                Out->RegX = UNKNOWN_REGVAL;
            }
            break;

        case OP65_LDY:
            if (CE_IsConstImm (E)) {
                Out->RegY = (unsigned char) E->Num;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Use & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->RegY = In->Tmp1;
                        break;
                    case REG_PTR1_LO:
                        Out->RegY = In->Ptr1Lo;
                        break;
                    case REG_PTR1_HI:
                        Out->RegY = In->Ptr1Hi;
                        break;
                    case REG_SREG_LO:
                        Out->RegY = In->SRegLo;
                        break;
                    case REG_SREG_HI:
                        Out->RegY = In->SRegHi;
                        break;
                    default:
                        Out->RegY = UNKNOWN_REGVAL;
                        break;
                }
            } else {
                /* Y is now unknown */
                Out->RegY = UNKNOWN_REGVAL;
            }
            break;

        case OP65_LSR:
            if (E->AM == AM65_ACC && RegValIsKnown (In->RegA)) {
                Out->RegA = (In->RegA >> 1) & 0xFF;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->Tmp1 = (In->Tmp1 >> 1) & 0xFF;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = (In->Ptr1Lo >> 1) & 0xFF;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = (In->Ptr1Hi >> 1) & 0xFF;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = (In->SRegLo >> 1) & 0xFF;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = (In->SRegHi >> 1) & 0xFF;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_NOP:
            break;

        case OP65_ORA:
            if (RegValIsKnown (In->RegA)) {
                if (CE_IsConstImm (E)) {
                    Out->RegA = In->RegA | (short) E->Num;
                } else if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            Out->RegA = In->RegA | In->Tmp1;
                            break;
                        case REG_PTR1_LO:
                            Out->RegA = In->RegA | In->Ptr1Lo;
                            break;
                        case REG_PTR1_HI:
                            Out->RegA = In->RegA | In->Ptr1Hi;
                            break;
                        case REG_SREG_LO:
                            Out->RegA = In->RegA | In->SRegLo;
                            break;
                        case REG_SREG_HI:
                            Out->RegA = In->RegA | In->SRegHi;
                            break;
                        default:
                            Out->RegA = UNKNOWN_REGVAL;
                            break;
                    }
                } else {
                    /* A is now unknown */
                    Out->RegA = UNKNOWN_REGVAL;
                }
            } else if (CE_IsKnownImm (E, 0xFF)) {
                /* ORA with 0xFF does always give 0xFF */
                Out->RegA = 0xFF;
            }
            break;

        case OP65_PHA:
            break;

        case OP65_PHP:
            break;

        case OP65_PHX:
            break;

        case OP65_PHY:
            break;

        case OP65_PLA:
            Out->RegA = UNKNOWN_REGVAL;
            break;

        case OP65_PLP:
            break;

        case OP65_PLX:
            Out->RegX = UNKNOWN_REGVAL;
            break;

        case OP65_PLY:
            Out->RegY = UNKNOWN_REGVAL;
            break;

        case OP65_ROL:
            /* We don't know the value of the carry bit */
            if (E->AM == AM65_ACC) {
                Out->RegA = UNKNOWN_REGVAL;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->Tmp1 = UNKNOWN_REGVAL;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = UNKNOWN_REGVAL;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = UNKNOWN_REGVAL;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = UNKNOWN_REGVAL;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = UNKNOWN_REGVAL;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_ROR:
            /* We don't know the value of the carry bit */
            if (E->AM == AM65_ACC) {
                Out->RegA = UNKNOWN_REGVAL;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->Tmp1 = UNKNOWN_REGVAL;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = UNKNOWN_REGVAL;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = UNKNOWN_REGVAL;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = UNKNOWN_REGVAL;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = UNKNOWN_REGVAL;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_RTI:
            break;

        case OP65_RTS:
            break;

        case OP65_SBC:
            /* We don't know the value of the carry bit */
            Out->RegA = UNKNOWN_REGVAL;
            break;

        case OP65_SEC:
            break;

        case OP65_SED:
            break;

        case OP65_SEI:
            break;

        case OP65_STA:
            if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
                    case REG_TMP1:
                        Out->Tmp1 = In->RegA;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = In->RegA;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = In->RegA;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = In->RegA;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = In->RegA;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_STX:
            if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
                    case REG_TMP1:
                        Out->Tmp1 = In->RegX;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = In->RegX;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = In->RegX;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = In->RegX;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = In->RegX;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_STY:
            if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
                    case REG_TMP1:
                        Out->Tmp1 = In->RegY;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = In->RegY;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = In->RegY;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = In->RegY;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = In->RegY;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_STZ:
            if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
                    case REG_TMP1:
                        Out->Tmp1 = 0;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = 0;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = 0;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = 0;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = 0;
                        break;
                }
            } else if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_TAX:
            Out->RegX = In->RegA;
            break;

        case OP65_TAY:
            Out->RegY = In->RegA;
            break;

        case OP65_TRB:
            if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            } else if (E->AM == AM65_ZP) {
                if (RegValIsKnown (In->RegA)) {
                    switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                        case REG_TMP1:
                            Out->Tmp1 &= ~In->RegA;
                            break;
                        case REG_PTR1_LO:
                            Out->Ptr1Lo &= ~In->RegA;
                            break;
                        case REG_PTR1_HI:
                            Out->Ptr1Hi &= ~In->RegA;
                            break;
                        case REG_SREG_LO:
                            Out->SRegLo &= ~In->RegA;
                            break;
                        case REG_SREG_HI:
                            Out->SRegHi &= ~In->RegA;
                            break;
                    }
                } else {
                    switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                        case REG_TMP1:
                            Out->Tmp1 = UNKNOWN_REGVAL;
                            break;
                        case REG_PTR1_LO:
                            Out->Ptr1Lo = UNKNOWN_REGVAL;
                            break;
                        case REG_PTR1_HI:
                            Out->Ptr1Hi = UNKNOWN_REGVAL;
                            break;
                        case REG_SREG_LO:
                            Out->SRegLo = UNKNOWN_REGVAL;
                            break;
                        case REG_SREG_HI:
                            Out->SRegHi = UNKNOWN_REGVAL;
                            break;
                    }
                }
            }
            break;

        case OP65_TSB:
            if (E->AM == AM65_ZPX) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            } else if (E->AM == AM65_ZP) {
                if (RegValIsKnown (In->RegA)) {
                    switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                        case REG_TMP1:
                            Out->Tmp1 |= In->RegA;
                            break;
                        case REG_PTR1_LO:
                            Out->Ptr1Lo |= In->RegA;
                            break;
                        case REG_PTR1_HI:
                            Out->Ptr1Hi |= In->RegA;
                            break;
                        case REG_SREG_LO:
                            Out->SRegLo |= In->RegA;
                            break;
                        case REG_SREG_HI:
                            Out->SRegHi |= In->RegA;
                            break;
                    }
                } else {
                    switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                        case REG_TMP1:
                            Out->Tmp1 = UNKNOWN_REGVAL;
                            break;
                        case REG_PTR1_LO:
                            Out->Ptr1Lo = UNKNOWN_REGVAL;
                            break;
                        case REG_PTR1_HI:
                            Out->Ptr1Hi = UNKNOWN_REGVAL;
                            break;
                        case REG_SREG_LO:
                            Out->SRegLo = UNKNOWN_REGVAL;
                            break;
                        case REG_SREG_HI:
                            Out->SRegHi = UNKNOWN_REGVAL;
                            break;
                    }
                }
            }
            break;

        case OP65_TSX:
            Out->RegX = UNKNOWN_REGVAL;
            break;

        case OP65_TXA:
            Out->RegA = In->RegX;
            break;

        case OP65_TXS:
            break;

        case OP65_TYA:
            Out->RegA = In->RegY;
            break;

        default:
            break;

    }
}



static char* RegInfoDesc (unsigned U, char* Buf)
/* Return a string containing register info */
{
    Buf[0] = '\0';

    strcat (Buf, U & REG_SREG_HI? "H" : "_");
    strcat (Buf, U & REG_SREG_LO? "L" : "_");
    strcat (Buf, U & REG_A?       "A" : "_");
    strcat (Buf, U & REG_X?       "X" : "_");
    strcat (Buf, U & REG_Y?       "Y" : "_");
    strcat (Buf, U & REG_TMP1?    "T1" : "__");
    strcat (Buf, U & REG_PTR1?    "1" : "_");
    strcat (Buf, U & REG_PTR2?    "2" : "_");
    strcat (Buf, U & REG_SAVE?    "V"  : "_");
    strcat (Buf, U & REG_SP?      "S" : "_");

    return Buf;
}



static char* RegContentDesc (const RegContents* RC, char* Buf)
/* Return a string containing register contents */
{
    char* B = Buf;

    if (RegValIsUnknown (RC->RegA)) {
        strcpy (B, "A:XX ");
    } else {
        sprintf (B, "A:%02X ", RC->RegA);
    }
    B += 5;
    if (RegValIsUnknown (RC->RegX)) {
        strcpy (B, "X:XX ");
    } else {
        sprintf (B, "X:%02X ", RC->RegX);
    }
    B += 5;
    if (RegValIsUnknown (RC->RegY)) {
        strcpy (B, "Y:XX");
    } else {
        sprintf (B, "Y:%02X", RC->RegY);
    }
    B += 4;

    return Buf;
}



void CE_Output (const CodeEntry* E)
/* Output the code entry to the output file */
{
    const OPCDesc* D;
    unsigned Chars;
    int Space;
    const char* Target;

    /* If we have a label, print that */
    unsigned LabelCount = CollCount (&E->Labels);
    unsigned I;
    for (I = 0; I < LabelCount; ++I) {
        CL_Output (CollConstAt (&E->Labels, I));
    }

    /* Get the opcode description */
    D = GetOPCDesc (E->OPC);

    /* Print the mnemonic */
    Chars = WriteOutput ("\t%s", D->Mnemo);

    /* Space to leave before the operand */
    Space = 9 - Chars;

    /* Print the operand */
    switch (E->AM) {

        case AM65_IMP:
            /* implicit */
            break;

        case AM65_ACC:
            /* accumulator */
            Chars += WriteOutput ("%*sa", Space, "");
            break;

        case AM65_IMM:
            /* immidiate */
            Chars += WriteOutput ("%*s#%s", Space, "", E->Arg);
            break;

        case AM65_ZP:
        case AM65_ABS:
            /* zeropage and absolute */
            Chars += WriteOutput ("%*s%s", Space, "", E->Arg);
            break;

        case AM65_ZPX:
        case AM65_ABSX:
            /* zeropage,X and absolute,X */
            Chars += WriteOutput ("%*s%s,x", Space, "", E->Arg);
            break;

        case AM65_ABSY:
            /* absolute,Y */
            Chars += WriteOutput ("%*s%s,y", Space, "", E->Arg);
            break;

        case AM65_ZPX_IND:
            /* (zeropage,x) */
            Chars += WriteOutput ("%*s(%s,x)", Space, "", E->Arg);
            break;

        case AM65_ZP_INDY:
            /* (zeropage),y */
            Chars += WriteOutput ("%*s(%s),y", Space, "", E->Arg);
            break;

        case AM65_ZP_IND:
            /* (zeropage) */
            Chars += WriteOutput ("%*s(%s)", Space, "", E->Arg);
            break;

        case AM65_BRA:
            /* branch */
            Target = E->JumpTo? E->JumpTo->Name : E->Arg;
            Chars += WriteOutput ("%*s%s", Space, "", Target);
            break;

        default:
            Internal ("Invalid addressing mode");

    }

    /* Print usage info if requested by the debugging flag */
    if (Debug) {
        char Use [128];
        char Chg [128];
        WriteOutput ("%*s; USE: %-12s CHG: %-12s SIZE: %u",
                     (int)(30-Chars), "",
                     RegInfoDesc (E->Use, Use),
                     RegInfoDesc (E->Chg, Chg),
                     E->Size);

        if (E->RI) {
            char RegIn[32];
            char RegOut[32];
            WriteOutput ("    In %s  Out %s",
                         RegContentDesc (&E->RI->In, RegIn),
                         RegContentDesc (&E->RI->Out, RegOut));
        }
    }

    /* Terminate the line */
    WriteOutput ("\n");
}
