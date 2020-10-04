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



#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "debugflag.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "asmlabel.h"
#include "codeent.h"
#include "codeinfo.h"
#include "codelab.h"
#include "error.h"
#include "global.h"
#include "ident.h"
#include "opcodes.h"
#include "output.h"
#include "reginfo.h"



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



static void FreeParsedArg (char* ArgBase)
/* Free a code entry parsed argument */
{
    if (ArgBase != 0 && ArgBase != EmptyArg) {
        xfree (ArgBase);
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

        /* Append processor flags as well as special usages */
        switch (E->OPC) {

            case OP65_ADC:
            case OP65_SBC:
                E->Use |= PSTATE_C;
                E->Chg |= PSTATE_CZVN;
                break;
            case OP65_ROL:
            case OP65_ROR:
                E->Use |= PSTATE_C;
                E->Chg |= PSTATE_CZN;
                break;
            case OP65_ASL:
            case OP65_LSR:
                E->Chg |= PSTATE_CZN;
                break;
            case OP65_CMP:
            case OP65_CPX:
            case OP65_CPY:
                E->Chg |= PSTATE_CZN;
                break;
            case OP65_BIT:
                E->Chg |= PSTATE_ZVN;
                if (E->AM != AM65_IMM) {
                    E->Chg &= ~(PSTATE_V | PSTATE_N);
                }
                break;
            case OP65_BRK:
                E->Chg |= PSTATE_B;
                break;
            case OP65_CLC:
            case OP65_SEC:
                E->Chg |= PSTATE_C;
                break;
            case OP65_CLD:
            case OP65_SED:
                E->Chg |= PSTATE_D;
                break;
            case OP65_CLI:
            case OP65_SEI:
                E->Chg |= PSTATE_I;
                break;
            case OP65_CLV:
                E->Chg |= PSTATE_V;
                break;
            case OP65_TRB:
            case OP65_TSB:
                E->Chg |= PSTATE_Z;
                break;
            case OP65_BCC:
            case OP65_BCS:
            case OP65_JCC:
            case OP65_JCS:
                E->Use |= PSTATE_C;
                break;
            case OP65_BEQ:
            case OP65_BNE:
            case OP65_JEQ:
            case OP65_JNE:
                E->Use |= PSTATE_Z;
                break;
            case OP65_BMI:
            case OP65_BPL:
            case OP65_JMI:
            case OP65_JPL:
                E->Use |= PSTATE_N;
                break;
            case OP65_BVC:
            case OP65_BVS:
            case OP65_JVC:
            case OP65_JVS:
                E->Use |= PSTATE_V;
                break;
            case OP65_BRA:
            case OP65_JMP:
                break;
            case OP65_AND:
            case OP65_EOR:
            case OP65_ORA:
            case OP65_DEA:
            case OP65_DEC:
            case OP65_DEX:
            case OP65_DEY:
            case OP65_INA:
            case OP65_INC:
            case OP65_INX:
            case OP65_INY:
            case OP65_LDA:
            case OP65_LDX:
            case OP65_LDY:
            case OP65_TAX:
            case OP65_TAY:
            case OP65_TXA:
            case OP65_TYA:
                E->Chg |= PSTATE_ZN;
                break;
            case OP65_TSX:
                E->Use |= SLV_SP65;
                E->Chg |= PSTATE_ZN;
                break;
            case OP65_TXS:
                E->Chg |= SLV_SP65;
                break;
            case OP65_PLA:
            case OP65_PLX:
            case OP65_PLY:
                E->Use |= SLV_SP65;
                E->Chg |= SLV_PL65 | PSTATE_ZN;
                break;
            case OP65_PLP:
                E->Use |= SLV_SP65;
                E->Chg |= SLV_PL65 | PSTATE_ALL;
                break;
            case OP65_PHA:
            case OP65_PHX:
            case OP65_PHY:
                E->Use |= SLV_SP65;
                E->Chg |= SLV_PH65;
                break;
            case OP65_PHP:
                E->Use |= SLV_SP65 | PSTATE_ALL;
                E->Chg |= SLV_PH65;
                break;
            case OP65_RTI:
                E->Chg |= PSTATE_ALL;
                break;
            case OP65_RTS:
                break;
            case OP65_STA:
            case OP65_STX:
            case OP65_STY:
            case OP65_STZ:
            case OP65_JSR:
            case OP65_NOP:
            default:
                break;
        }
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int ParseOpcArgStr (const char* Arg, unsigned short* ArgInfo, struct StrBuf* Name, long* Offset)
/* Break the opcode argument string into a symbol name/label part plus an offset.
** Both parts are optional, but if there are any characters in the string that
** can't be parsed, it's an failure.
** The caller is responsible for managing the StrBuf.
** Return whether parsing succeeds or not.
*/
{
    unsigned short  Flags = 0;
    const char*     OffsetPart = 0;
    const char*     NameEnd = 0;
    int             Negative = 0;
    int             Parentheses = 0;
    unsigned long   NumVal = 0;
    long long       AccOffset = 0;
    char*           End;            /* Used for checking errors */

    if (ArgInfo != 0) {
        *ArgInfo = 0;
    }
    *Offset = 0;

    /* A numeric address is treated as an unnamed address with the numeric part as the offset */
    if (IsDigit (Arg[0]) || Arg[0] == '$') {
        /* A call to a numeric address */
        SB_Clear (Name);
        SB_Terminate (Name);
        OffsetPart = Arg;
    } else {
        /* <, >, ^ */
        if (Arg[0] == '<') {
            Flags |= AIF_LOBYTE;
        } else if (Arg[0] == '>') {
            Flags |= AIF_HIBYTE;
        } else if (Arg[0] == '^') {
            Flags |= AIF_BANKBYTE;
        }

        if ((Flags & (AIF_FAR)) != 0) {
            /* Skip this char */
            ++Arg;
        }

        /* Skip spaces */
        while (Arg[0] == ' ') {
            ++Arg;
        }

        /* Strip parentheses off if exist */
        if (Arg[0] == '(') {
            /* Skip this char */
            ++Arg;

            End = strchr (Arg, ')');
            if (End == 0 || End[1] != '\0') {
                /* Not closed at the end, bail out */
                *Offset = 0;
                if (ArgInfo != 0) {
                    *ArgInfo = Flags | AIF_FAILURE;
                }
                return 0;
            }

            /* Found */
            Parentheses = 1;

            /* Skip spaces */
            while (Arg[0] == ' ') {
                ++Arg;
            }
        }

        /* If the symbol name starts with an underline, it is an external symbol.
        ** If the symbol does not start with an underline, it may be a built-in
        ** symbol.
        */
        if (Arg[0] == '_') {
            Flags |= AIF_EXTERNAL;
        } else {
            Flags |= AIF_BUILTIN;
        }

        /* Rip off the offset if present. */
        OffsetPart = strchr (Arg, '+');
        if (OffsetPart == 0) {
            OffsetPart = strchr (Arg, '-');
        }

        if (OffsetPart != 0) {
            /* Get the real arg name */
            NameEnd = strchr (Arg, ' ');
            if (NameEnd == 0 || NameEnd > OffsetPart) {
                NameEnd = OffsetPart;
            }
            SB_CopyBuf (Name, Arg, NameEnd - Arg);
            SB_Terminate (Name);

        } else {
            /* No offset */
            if (Parentheses == 0) {
                SB_CopyStr (Name, Arg);
            } else {
                SB_CopyBuf (Name, Arg, End - Arg);
            }
            SB_Terminate (Name);
        }

        if ((Flags & AIF_EXTERNAL) == 0) {
            if (SB_GetLen (Name) > 0) {
                Flags |= AIF_HAS_NAME;

                /* See if the name is a local label */
                if (IsLocalLabelName (SB_GetConstBuf (Name))) {
                    Flags |= AIF_LOCAL;
                }
            }

        } else {
            if (SB_GetLen (Name) <= 0) {
                /* Invalid external name */
                Flags &= ~AIF_EXTERNAL;
                *Offset = 0;
                if (ArgInfo != 0) {
                    *ArgInfo = Flags | AIF_FAILURE;
                }
                return 0;
            }
            Flags |= AIF_HAS_NAME;
        }

        /* A byte size expression with no parentheses but an offset is not
        ** handled correctly for now, so just bail out in such cases.
        */
        if ((Flags & AIF_FAR) != 0  &&
            Parentheses == 0        &&
            OffsetPart != 0         &&
            OffsetPart[0] != '\0') {
            /* Bail out */
            *Offset = 0;
            if (ArgInfo != 0) {
                *ArgInfo = Flags | AIF_FAILURE;
            }
            return 0;
        }
    }

    /* Get the offset */
    while (OffsetPart != 0       &&
           OffsetPart[0] != '\0' &&
           OffsetPart[0] != ')') {
        /* Skip spaces */
        while (OffsetPart[0] == ' ') {
            ++OffsetPart;
        }

        Negative = 0;
        if (OffsetPart[0] == '+') {
            ++OffsetPart;
        } else if (OffsetPart[0] == '-') {
            Negative = 1;
            ++OffsetPart;
        }

        /* Skip spaces */
        while (OffsetPart[0] == ' ') {
            ++OffsetPart;
        }

        /* Determine the base and convert the value. strtol/strtoul is not
        ** exactly what we want here, but it's cheap and may be replaced by
        ** something fancier later.
        */
        if (OffsetPart[0] == '$') {
            /* Base 16 hexedemical */
            NumVal = strtoul (OffsetPart+1, &End, 16);
        } else if (OffsetPart[0] != '%') {
            /* Base 10 decimal */
            NumVal = strtoul (OffsetPart, &End, 10);
        } else {
            /* Base 2 binary */
            NumVal = strtoul (OffsetPart+1, &End, 2);
        }

        /* Check if the conversion was successful */
        if (*End != '\0' && *End != ' ' && *End != '+' && *End != '-' && *End != ')') {
            /* Could not convert */
            *Offset = 0;
            if (ArgInfo != 0) {
                *ArgInfo = Flags | AIF_FAILURE;
            }
            return 0;
        }

        /* Check for out of range result */
        if (NumVal == ULONG_MAX && errno == ERANGE) {
            /* Could not convert */
            *Offset = 0;
            if (ArgInfo != 0) {
                *ArgInfo = Flags | AIF_FAILURE;
            }
            return 0;
        }

        /* This argument does have an offset */
        Flags |= AIF_HAS_OFFSET;

        if (Negative) {
            AccOffset -= (long long)NumVal;
        } else {
            AccOffset += (long long)NumVal;
        }

        /* See if there are more */
        Arg = OffsetPart;
        OffsetPart = strchr (Arg, '+');
        if (OffsetPart == 0) {
            OffsetPart = strchr (Arg, '-');
        }
    }

    if (AccOffset > LONG_MAX || AccOffset < LONG_MIN) {
        /* Could not convert */
        *Offset = 0;
        if (ArgInfo != 0) {
            *ArgInfo = Flags | AIF_FAILURE;
        }
        return 0;
    }
    *Offset = (long)AccOffset;
    if (ArgInfo != 0) {
        *ArgInfo = Flags & ~AIF_FAILURE;
    }

    return 1;
}



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



void PreparseArg (CodeEntry* E)
/* Parse the argument string and memorize the result for the code entry */
{
    StrBuf B = AUTO_STRBUF_INITIALIZER;

    /* Parse the argument string */
    if (ParseOpcArgStr (E->Arg, &E->ArgInfo, &B, &E->ArgOff)) {
        E->ArgBase = SB_GetBuf (&B);

        if ((E->ArgInfo & (AIF_HAS_NAME | AIF_HAS_OFFSET)) == AIF_HAS_OFFSET) {
            E->Flags |= CEF_NUMARG;

            /* Use the new numerical value */
            E->Num = E->ArgOff;
        }

    } else {
        /* Parsing fails. Issue an error/warning so that this could be spotted and fixed. */
        E->ArgBase = EmptyArg;
        SB_Done (&B);
        if (Debug) {
            Warning ("Parsing argument \"%s\" failed!", E->Arg);
        }
    }
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
    E->OPC      = D->OPC;
    E->AM       = AM;
    E->Size     = GetInsnSize (E->OPC, E->AM);
    E->Arg      = GetArgCopy (Arg);
    E->Flags    = 0;
    E->Info     = D->Info;
    E->ArgInfo  = 0;
    E->JumpTo   = JumpTo;
    E->LI       = UseLineInfo (LI);
    E->RI       = 0;

    /* Parse the argument string if it's given */
    if (Arg == 0 || Arg[0] == '\0') {
        E->ArgBase = EmptyArg;
    } else {
        PreparseArg (E);
    }

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
    /* Free the argument base string if we have one */
    FreeParsedArg (E->ArgBase);

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

    /* Clear the argument */
    CE_SetArg (E, 0);
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
/* Replace the whole argument by the new one. */
{
    /* Free the old parsed argument base */
    FreeParsedArg (E->ArgBase);

    /* Free the old argument */
    FreeArg (E->Arg);

    /* Assign the new one */
    E->Arg = GetArgCopy (Arg);

    /* Parse the new argument string */
    PreparseArg (E);

    /* Update the Use and Chg in E */
    SetUseChgInfo (E, GetOPCDesc (E->OPC));
}



void CE_SetArgBaseAndOff (CodeEntry* E, const char* ArgBase, long ArgOff)
/* Replace the new argument base and offset. Argument base is always applied.
** Argument offset is applied if and only if E has the AIF_HAS_OFFSET flag set.
*/
{
    if (ArgBase != 0 && ArgBase[0] != '\0') {

        /* The argument base is not blank */
        char Buf[IDENTSIZE + 16];
        char* Str = Buf;
        size_t Len = strlen (ArgBase) + 16;
        if (Len >= sizeof (Buf)) {
            Str = xmalloc (Len);
        }

        if ((E->ArgInfo & AIF_FAR) == 0) {
            if (CE_HasArgOffset (E)) {
                sprintf (Str, "%s%+ld", ArgBase, ArgOff);
            } else {
                sprintf (Str, "%s", ArgBase);
            }
            CE_SetArg (E, Str);
        } else {
            /* A byte expression */
            const char* Expr = "";
            if ((E->ArgInfo & AIF_FAR) == AIF_LOBYTE) {
                Expr = "<";
            } else if ((E->ArgInfo & AIF_FAR) == AIF_HIBYTE) {
                Expr = ">";
            } else if ((E->ArgInfo & AIF_FAR) == AIF_BANKBYTE) {
                Expr = "^";
            } else {
                Internal ("Invalid byte size flag in CE_SetArgBaseAndOff");
            }

            if (CE_HasArgOffset (E)) {
                sprintf (Str, "%s(%s%+ld)", Expr, ArgBase, ArgOff);
            } else {
                sprintf (Str, "%s(%s)", Expr, ArgBase);
            }
        }

        CE_SetArg (E, Str);

        if (Str != Buf) {
            xfree (Str);
        }

    } else {
        /* The argument has no base */
        if ((E->ArgInfo & AIF_HAS_OFFSET) != 0) {
            /* This is a numeric argument */
            E->Flags |= CEF_NUMARG;
            CE_SetNumArg (E, ArgOff);
        } else {
            /* Empty argument */
            CE_SetArg (E, EmptyArg);
        }
    }
}



void CE_SetArgBase (CodeEntry* E, const char* ArgBase)
/* Replace the argument base by the new one.
** The entry must have an existing base.
*/
{
    /* Check that the entry has a base name */
    CHECK (CE_HasArgBase (E));

    CE_SetArgBaseAndOff (E, ArgBase, E->ArgOff);
}



void CE_SetArgOffset (CodeEntry* E, long ArgOff)
/* Replace the argument offset by the new one */
{
    CE_SetArgBaseAndOff (E, E->ArgBase, ArgOff);
}



void CE_SetNumArg (CodeEntry* E, long Num)
/* Set a new numeric argument for the given code entry that must already
** have a numeric argument.
*/
{
    char Buf[16];

    /* Check that the entry has a numerical argument */
    CHECK (CE_HasNumArg (E));

    /* Make the new argument string */
    if (E->Size == 2) {
        Num &= 0xFF;
        xsprintf (Buf, sizeof (Buf), "$%02X", (unsigned) Num);
    } else if (E->Size == 3 || E->Size == 5) {
        Num &= 0xFFFF;
        xsprintf (Buf, sizeof (Buf), "$%04X", (unsigned) Num);
    } else {
        Internal ("Invalid instruction size in CE_SetNumArg");
    }

    /* Replace the whole argument by the new one */
    CE_SetArg (E, Buf);
}



int CE_IsArgStrParsed (const CodeEntry* E)
/* Return true if the argument of E was successfully parsed last time */
{
    return (E->ArgInfo & AIF_FAILURE) == 0;
}



int CE_HasArgBase (const CodeEntry* E)
/* Return true if the argument of E has a non-blank base name */
{
    return (E->ArgInfo & AIF_HAS_NAME) != 0 && E->ArgBase[0] != '\0';
}



int CE_HasArgOffset (const CodeEntry* E)
/* Return true if the argument of E has a non-zero offset */
{
    return (E->ArgInfo & AIF_HAS_OFFSET) != 0 && E->ArgOff != 0;
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

    /* PHP will use all flags */
    if (E->OPC == OP65_PHP) {
        return 1;
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



static void DeduceZ (RegContents* C, short Val)
/* Auto-set Z flag */
{
    if (RegValIsUnknown (Val)) {
        C->PFlags |= UNKNOWN_PFVAL_Z;
    } else {
        C->PFlags &= ~UNKNOWN_PFVAL_Z;
        if (Val == 0) {
            C->PFlags |= PFVAL_Z;
        }
    }
}



static void DeduceZN (RegContents* C, short Val)
/* Auto-set Z/N flags */
{
    if (RegValIsUnknown (Val)) {
        C->PFlags |= UNKNOWN_PFVAL_ZN;
    } else {
        C->PFlags &= ~UNKNOWN_PFVAL_ZN;
        if (Val == 0) {
            C->PFlags |= PFVAL_Z;
        } else if (Val & PFVAL_N) {
            C->PFlags |= PFVAL_N;
        }
    }
}



static short KnownOpADCDeduceCZVN (RegContents* Out, RegContents* In, short Rhs)
/* Do the ADC and auto-set C/Z/V/N flags.
** Both operands and the C flag must be known.
*/
{
    short SVal, UVal;
    SVal = (signed char)(In->RegA & 0xFF) + (signed char)(Rhs & 0xFF);
    UVal = (In->RegA & 0xFF) + (Rhs & 0xFF);
    if (In->PFlags & PFVAL_C) {
        ++SVal;
        ++UVal;
    }

    Out->PFlags &= ~UNKNOWN_PFVAL_CZVN;
    if (UVal > 0xFF) {
        Out->PFlags |= PFVAL_C;
    }

    if (SVal < -128 || SVal > 127) {
        Out->PFlags |= PFVAL_V;
    }

    DeduceZN (Out, UVal);

    return UVal;
}



static short KnownOpSBCDeduceCZVN (RegContents* Out, RegContents* In, short Rhs)
/* Do the SBC and auto-set C/Z/V/N flags.
** Both operands and the C flag must be known.
*/
{
    short SVal, UVal;
    SVal = (signed char)(In->RegA & 0xFF) - (signed char)(Rhs & 0xFF);
    UVal = (In->RegA & 0xFF) - (Rhs & 0xFF);
    if ((In->PFlags & PFVAL_C) == 0) {
        --SVal;
        --UVal;
    }

    Out->PFlags &= ~UNKNOWN_PFVAL_CZVN;
    if (UVal >= 0) {
        Out->PFlags |= PFVAL_C;
    }

    if (SVal < -128 || SVal > 127) {
        Out->PFlags |= PFVAL_V;
    }

    DeduceZN (Out, UVal);

    return UVal;
}



static short KnownOpCmpDeduceCZN (RegContents* C, short Lhs, short Rhs)
/* Do the CMP and auto-set C/Z/N flags.
** Both operands must be known.
 */
{
    short Val = (Lhs & 0xFF) - (Rhs & 0xFF);

    C->PFlags &= ~UNKNOWN_PFVAL_C;
    if (Val >= 0) {
        C->PFlags |= PFVAL_C;
    }
    DeduceZN (C, Val);

    return Val;
}



static short AnyOpASLDeduceCZN (RegContents* C, short Shiftee)
/* Do the ASL and auto-set C/Z/N flags */
{
    if (RegValIsKnown (Shiftee)) {
        C->PFlags &= ~UNKNOWN_PFVAL_C;
        if (Shiftee & 0x80U) {
            C->PFlags |= PFVAL_C;
        }
        Shiftee = (Shiftee << 1) & 0xFF;
    }
    DeduceZN (C, Shiftee);

    return Shiftee;
}



static short AnyOpLSRDeduceCZN (RegContents* C, short Shiftee)
/* Do the LSR and auto-set C/Z/N flags */
{
    if (RegValIsKnown (Shiftee)) {
        C->PFlags &= ~UNKNOWN_PFVAL_C;
        if (Shiftee & 0x01U) {
            C->PFlags |= PFVAL_C;
        }
        Shiftee = (Shiftee >> 1) & 0xFF;
    }
    DeduceZN (C, Shiftee);

    return Shiftee;
}



static short AnyOpROLDeduceCZN (RegContents* C, short PFlags, short Shiftee)
/* Do the ROL and auto-set C/Z/N flags */
{
    if (RegValIsKnown (Shiftee) && PStatesAreKnown (PFlags, PSTATE_C)) {
        C->PFlags &= ~UNKNOWN_PFVAL_C;
        if (Shiftee & 0x80U) {
            C->PFlags |= PFVAL_C;
        }
        Shiftee = (Shiftee << 1) & 0xFF;
        if (PFlags & PFVAL_C) {
            Shiftee |= 0x01U;
        }
    } else {
        Shiftee = UNKNOWN_REGVAL;
    }
    DeduceZN (C, Shiftee);

    return Shiftee;
}



static short AnyOpRORDeduceCZN (RegContents* C, short PFlags, short Shiftee)
/* Do the ROR and auto-set C/Z/N flags */
{
    if (RegValIsKnown (Shiftee) && PStatesAreKnown (PFlags, PSTATE_C)) {
        C->PFlags &= ~UNKNOWN_PFVAL_C;
        if (Shiftee & 0x01U) {
            C->PFlags |= PFVAL_C;
        }
        Shiftee = (Shiftee >> 1) & 0xFF;
        if (PFlags & PFVAL_C) {
            Shiftee |= 0x80U;
        }
    } else {
        Shiftee = UNKNOWN_REGVAL;
    }
    DeduceZN (C, Shiftee);

    return Shiftee;
}



static void BranchDeduceOnProcessorFlag (RegContents* True, RegContents* False, unsigned short PTrueFlag)
/* Auto-set the corresponding C/Z/V/N flag output for both True/Flase code flows */
{
    PTrueFlag &= 0xFF;
    unsigned short Mask = ~(PTrueFlag * 0x0101U) & 0xFFFFU;
    True->PFlags  &= Mask;
    True->PFlags  |= PTrueFlag;
    False->PFlags &= Mask;
}



static int MightAffectKnownZP (CodeEntry* E, RegContents* In)
/* TODO: This is supposed to check if any builtin ZP could be affected.
** It simply returns TRUE in most cases for now.
*/
{
    unsigned Index = 0;

    if (E->AM == AM65_ZP ||
        E->AM == AM65_ABS ||
        (E->AM == AM65_ZPX && RegValIsKnown (In->RegX) && (Index = In->RegX & 0xFF)) ||
        (E->AM == AM65_ZPY && RegValIsKnown (In->RegY) && (Index = In->RegY & 0xFF)) ||
        (E->AM == AM65_ABSX && RegValIsKnown (In->RegX) && (Index = In->RegX & 0xFF)) ||
        (E->AM == AM65_ABSY && RegValIsKnown (In->RegY) && (Index = In->RegY & 0xFF))) {
        return 1;
    } else if ((E->AM == AM65_ZP_IND) ||
               (E->AM == AM65_ZPX_IND && RegValIsKnown (In->RegX) && (Index = In->RegX & 0xFF)) ||
               (E->AM == AM65_ZP_INDY && RegValIsKnown (In->RegY) && (Index = In->RegY & 0xFF))) {
        return 1;
    }
    return 1;
}



void CE_GenRegInfo (CodeEntry* E, RegContents* InputRegs)
/* Generate register info for this instruction. If an old info exists, it is
** overwritten.
*/
{
    /* Pointers to the register contents */
    RegContents* In;
    RegContents* Out;
    RegContents* BranchOut;

    /* Function register usage */
    unsigned Use, Chg;

    /* Value in question */
    short Val = UNKNOWN_REGVAL;

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
    BranchOut = &E->RI->Out2;

    /* Handle the different instructions */
    switch (E->OPC) {

        case OP65_ADC:
            Out->RegA = UNKNOWN_REGVAL;
            Out->PFlags |= UNKNOWN_PFVAL_CZVN;
            Out->ZNRegs = ZNREG_A;
            if (PStatesAreKnown (In->PFlags, PSTATE_C) &&
                RegValIsKnown (In->RegA)) {
                if (CE_IsConstImm (E)) {
                    Out->RegA = KnownOpADCDeduceCZVN (Out, In, (short) E->Num);
                } else if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            Out->RegA = KnownOpADCDeduceCZVN (Out, In, In->Tmp1);
                            break;
                        case REG_PTR1_LO:
                            Out->RegA = KnownOpADCDeduceCZVN (Out, In, In->Ptr1Lo);
                            break;
                        case REG_PTR1_HI:
                            Out->RegA = KnownOpADCDeduceCZVN (Out, In, In->Ptr1Hi);
                            break;
                        case REG_SREG_LO:
                            Out->RegA = KnownOpADCDeduceCZVN (Out, In, In->SRegLo);
                            break;
                        case REG_SREG_HI:
                            Out->RegA = KnownOpADCDeduceCZVN (Out, In, In->SRegHi);
                            break;
                        default:
                            break;
                    }
                }
            }
            break;

        case OP65_AND:
            Out->RegA = UNKNOWN_REGVAL;
            Out->ZNRegs = ZNREG_A;
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
                            break;
                    }
                }
            } else if (CE_IsKnownImm (E, 0)) {
                /* A and $00 does always give zero */
                Out->RegA = 0;
            }
            DeduceZN (Out, Out->RegA);
            break;

        case OP65_ASL:
            Out->PFlags |= UNKNOWN_PFVAL_CZN;
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ACC) {
                Out->RegA = AnyOpASLDeduceCZN (Out, In->RegA);
                Out->ZNRegs = ZNREG_A;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->Tmp1 = AnyOpASLDeduceCZN (Out, In->Tmp1);
                        Out->ZNRegs = ZNREG_TMP1;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = AnyOpASLDeduceCZN (Out, In->Ptr1Lo);
                        Out->ZNRegs = ZNREG_PTR1_LO;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = AnyOpASLDeduceCZN (Out, In->Ptr1Hi);
                        Out->ZNRegs = ZNREG_PTR1_HI;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = AnyOpASLDeduceCZN (Out, In->SRegLo);
                        Out->ZNRegs = ZNREG_SREG_LO;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = AnyOpASLDeduceCZN (Out, In->SRegHi);
                        Out->ZNRegs = ZNREG_SREG_HI;
                        break;
                }
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_BCC:
            BranchDeduceOnProcessorFlag (Out, BranchOut, PFVAL_C);
            break;

        case OP65_BCS:
            BranchDeduceOnProcessorFlag (BranchOut, Out, PFVAL_C);
            break;

        case OP65_BEQ:
            BranchDeduceOnProcessorFlag (BranchOut, Out, PFVAL_Z);
            break;

        case OP65_BIT:
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Val = In->Tmp1;
                        break;
                    case REG_PTR1_LO:
                        Val = In->Ptr1Lo;
                        break;
                    case REG_PTR1_HI:
                        Val = In->Ptr1Hi;
                        break;
                    case REG_SREG_LO:
                        Val = In->SRegLo;
                        break;
                    case REG_SREG_HI:
                        Val = In->SRegHi;
                        break;
                }
            } else if (CE_IsConstImm (E)) {
                /* 65C02 special */
                Val = (short) E->Num;
            }

            /* BIT is unique with regards to the Z/V/N flags:
            ** - The Z is set/cleared according to whether the AND result is zero.
            ** - The V is coped directly from Bit 6 of the orginal argument.
            ** - The N is coped directly from Bit 7 of the orginal argument.
            ** Note the V/N flags are not affected in imm addressing mode supported by 65c02!
            */
            if (E->AM == AM65_IMM) {
                if (RegValIsKnown (Val)) {
                    Out->PFlags &= ~(UNKNOWN_PFVAL_V | UNKNOWN_PFVAL_N);
                    if (Val & PFVAL_V) {
                        Out->PFlags |= PFVAL_V;
                    }
                    Out->PFlags &= ~UNKNOWN_PFVAL_V;
                    if (Val & PFVAL_V) {
                        Out->PFlags |= PFVAL_V;
                    }
                } else {
                    Out->PFlags |= UNKNOWN_PFVAL_V | UNKNOWN_PFVAL_N;
                }
            }
            if ((RegValIsKnown (Val) && RegValIsKnown (In->RegA))) {
                Val &= In->RegA;
            } else if (((RegValIsKnown (Val) && Val == 0) ||
                        (RegValIsKnown (In->RegA) && In->RegA == 0))) {
                Val = 0;
            }
            DeduceZ (Out, Val);
            break;

        case OP65_BMI:
            BranchDeduceOnProcessorFlag (BranchOut, Out, PFVAL_N);
            break;

        case OP65_BNE:
            BranchDeduceOnProcessorFlag (Out, BranchOut, PFVAL_Z);
            break;

        case OP65_BPL:
            BranchDeduceOnProcessorFlag (Out, BranchOut, PFVAL_N);
            break;

        case OP65_BRA:
            break;

        case OP65_BRK:
            Out->PFlags &= ~UNKNOWN_PFVAL_B;
            Out->PFlags |= PFVAL_B;
            break;

        case OP65_BVC:
            BranchDeduceOnProcessorFlag (Out, BranchOut, PFVAL_V);
            break;

        case OP65_BVS:
            BranchDeduceOnProcessorFlag (BranchOut, Out, PFVAL_V);
            break;

        case OP65_CLC:
            Out->PFlags &= ~UNKNOWN_PFVAL_C;
            break;

        case OP65_CLD:
            Out->PFlags &= ~UNKNOWN_PFVAL_D;
            break;

        case OP65_CLI:
            Out->PFlags &= ~UNKNOWN_PFVAL_I;
            break;

        case OP65_CLV:
            Out->PFlags &= ~UNKNOWN_PFVAL_V;
            break;

        case OP65_CMP:
            Out->PFlags |= UNKNOWN_PFVAL_CZN;
            Out->ZNRegs = ZNREG_NONE;
            if (RegValIsKnown (In->RegA)) {
                if (CE_IsConstImm (E)) {
                    KnownOpCmpDeduceCZN (Out, In->RegA, (short)E->Num);
                } else if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            KnownOpCmpDeduceCZN (Out, In->RegA, In->Tmp1);
                            break;
                        case REG_PTR1_LO:
                            KnownOpCmpDeduceCZN (Out, In->RegA, In->Ptr1Lo);
                            break;
                        case REG_PTR1_HI:
                            KnownOpCmpDeduceCZN (Out, In->RegA, In->Ptr1Hi);
                            break;
                        case REG_SREG_LO:
                            KnownOpCmpDeduceCZN (Out, In->RegA, In->SRegLo);
                            break;
                        case REG_SREG_HI:
                            KnownOpCmpDeduceCZN (Out, In->RegA, In->SRegHi);
                            break;
                        default:
                            break;
                    }
                }
            }
            break;

        case OP65_CPX:
            Out->PFlags |= UNKNOWN_PFVAL_CZN;
            Out->ZNRegs = ZNREG_NONE;
            if (RegValIsKnown (In->RegX)) {
                if (CE_IsConstImm (E)) {
                    KnownOpCmpDeduceCZN (Out, In->RegX, (short)E->Num);
                } else if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            KnownOpCmpDeduceCZN (Out, In->RegX, In->Tmp1);
                            break;
                        case REG_PTR1_LO:
                            KnownOpCmpDeduceCZN (Out, In->RegX, In->Ptr1Lo);
                            break;
                        case REG_PTR1_HI:
                            KnownOpCmpDeduceCZN (Out, In->RegX, In->Ptr1Hi);
                            break;
                        case REG_SREG_LO:
                            KnownOpCmpDeduceCZN (Out, In->RegX, In->SRegLo);
                            break;
                        case REG_SREG_HI:
                            KnownOpCmpDeduceCZN (Out, In->RegX, In->SRegHi);
                            break;
                        default:
                            break;
                    }
                }
            }
            break;

        case OP65_CPY:
            Out->PFlags |= UNKNOWN_PFVAL_CZN;
            Out->ZNRegs = ZNREG_NONE;
            if (RegValIsKnown (In->RegY)) {
                if (CE_IsConstImm (E)) {
                    KnownOpCmpDeduceCZN (Out, In->RegY, (short)E->Num);
                } else if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            KnownOpCmpDeduceCZN (Out, In->RegY, In->Tmp1);
                            break;
                        case REG_PTR1_LO:
                            KnownOpCmpDeduceCZN (Out, In->RegY, In->Ptr1Lo);
                            break;
                        case REG_PTR1_HI:
                            KnownOpCmpDeduceCZN (Out, In->RegY, In->Ptr1Hi);
                            break;
                        case REG_SREG_LO:
                            KnownOpCmpDeduceCZN (Out, In->RegY, In->SRegLo);
                            break;
                        case REG_SREG_HI:
                            KnownOpCmpDeduceCZN (Out, In->RegY, In->SRegHi);
                            break;
                        default:
                            break;
                    }
                }
            }
            break;

        case OP65_DEA:
            Out->ZNRegs = ZNREG_A;
            if (RegValIsKnown (In->RegA)) {
                Out->RegA = (In->RegA - 1) & 0xFF;
            }
            DeduceZN (Out, Out->RegA);
            break;

        case OP65_DEC:
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ACC) {
                if (RegValIsKnown (In->RegA)) {
                    Val = Out->RegA = (In->RegA - 1) & 0xFF;
                }
                Out->ZNRegs = ZNREG_A;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                    case REG_TMP1:
                        Val = Out->Tmp1 = (In->Tmp1 - 1) & 0xFF;
                        Out->ZNRegs = ZNREG_TMP1;
                        break;
                    case REG_PTR1_LO:
                        Val = Out->Ptr1Lo = (In->Ptr1Lo - 1) & 0xFF;
                        Out->ZNRegs = ZNREG_PTR1_LO;
                        break;
                    case REG_PTR1_HI:
                        Val = Out->Ptr1Hi = (In->Ptr1Hi - 1) & 0xFF;
                        Out->ZNRegs = ZNREG_PTR1_HI;
                        break;
                    case REG_SREG_LO:
                        Val = Out->SRegLo = (In->SRegLo - 1) & 0xFF;
                        Out->ZNRegs = ZNREG_SREG_LO;
                        break;
                    case REG_SREG_HI:
                        Val = Out->SRegHi = (In->SRegHi - 1) & 0xFF;
                        Out->ZNRegs = ZNREG_SREG_HI;
                        break;
                }
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            DeduceZN (Out, Val);
            break;

        case OP65_DEX:
            Out->ZNRegs = ZNREG_X;
            if (RegValIsKnown (In->RegX)) {
                Out->RegX = (In->RegX - 1) & 0xFF;
            }
            DeduceZN (Out, Out->RegX);
            break;

        case OP65_DEY:
            Out->ZNRegs = ZNREG_Y;
            if (RegValIsKnown (In->RegY)) {
                Out->RegY = (In->RegY - 1) & 0xFF;
            }
            DeduceZN (Out, Out->RegY);
            break;

        case OP65_EOR:
            Out->RegA = UNKNOWN_REGVAL;
            Out->ZNRegs = ZNREG_A;
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
                            break;
                    }
                }
            }
            DeduceZN (Out, Out->RegA);
            break;

        case OP65_INA:
            Out->ZNRegs = ZNREG_A;
            if (RegValIsKnown (In->RegA)) {
                Out->RegA = (In->RegA + 1) & 0xFF;
            }
            DeduceZN (Out, Out->RegA);
            break;

        case OP65_INC:
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ACC) {
                if (RegValIsKnown (In->RegA)) {
                    Val = Out->RegA = (In->RegA + 1) & 0xFF;
                }
                Out->ZNRegs = ZNREG_A;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
                    case REG_TMP1:
                        Val = Out->Tmp1 = (In->Tmp1 + 1) & 0xFF;
                        Out->ZNRegs = ZNREG_TMP1;
                        break;
                    case REG_PTR1_LO:
                        Val = Out->Ptr1Lo = (In->Ptr1Lo + 1) & 0xFF;
                        Out->ZNRegs = ZNREG_PTR1_LO;
                        break;
                    case REG_PTR1_HI:
                        Val = Out->Ptr1Hi = (In->Ptr1Hi + 1) & 0xFF;
                        Out->ZNRegs = ZNREG_PTR1_HI;
                        break;
                    case REG_SREG_LO:
                        Val = Out->SRegLo = (In->SRegLo + 1) & 0xFF;
                        Out->ZNRegs = ZNREG_SREG_LO;
                        break;
                    case REG_SREG_HI:
                        Val = Out->SRegHi = (In->SRegHi + 1) & 0xFF;
                        Out->ZNRegs = ZNREG_SREG_HI;
                        break;
                }
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            DeduceZN (Out, Val);
            break;

        case OP65_INX:
            Out->ZNRegs = ZNREG_X;
            if (RegValIsKnown (In->RegX)) {
                Out->RegX = (In->RegX + 1) & 0xFF;
            }
            DeduceZN (Out, Out->RegX);
            break;

        case OP65_INY:
            Out->ZNRegs = ZNREG_Y;
            if (RegValIsKnown (In->RegY)) {
                Out->RegY = (In->RegY + 1) & 0xFF;
            }
            DeduceZN (Out, Out->RegY);
            break;

        case OP65_JCC:
            BranchDeduceOnProcessorFlag (Out, BranchOut, PFVAL_C);
            break;

        case OP65_JCS:
            BranchDeduceOnProcessorFlag (BranchOut, Out, PFVAL_C);
            break;

        case OP65_JEQ:
            BranchDeduceOnProcessorFlag (BranchOut, Out, PFVAL_Z);
            break;

        case OP65_JMI:
            BranchDeduceOnProcessorFlag (BranchOut, Out, PFVAL_N);
            break;

        case OP65_JMP:
            break;

        case OP65_JNE:
            BranchDeduceOnProcessorFlag (Out, BranchOut, PFVAL_Z);
            break;

        case OP65_JPL:
            BranchDeduceOnProcessorFlag (Out, BranchOut, PFVAL_N);
            break;

        case OP65_JSR:
            Out->ZNRegs = ZNREG_NONE;

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
            /* FIXME: Quick hack to set flags on process status: */
            Out->PFlags |= ((Chg & PSTATE_ALL) >> PSTATE_BITS_SHIFT) * 0x0101U;

            /* ## FIXME: Quick hack for some known functions: */
            if (strcmp (E->Arg, "complax") == 0) {
                if (RegValIsKnown (In->RegA)) {
                    Out->RegA = (In->RegA ^ 0xFF);
                }
                if (RegValIsKnown (In->RegX)) {
                    Out->RegX = (In->RegX ^ 0xFF);
                }
            } else if (strcmp (E->Arg, "tosandax") == 0) {
                if (RegValIsKnown (In->RegA) && In->RegA == 0) {
                    Out->RegA = 0;
                }
                if (RegValIsKnown (In->RegX) && In->RegX == 0) {
                    Out->RegX = 0;
                }
            } else if (strcmp (E->Arg, "tosaslax") == 0) {
                if (RegValIsKnown (In->RegA) && (In->RegA & 0x0F) >= 8) {
                    printf ("Hey!\n");
                    Out->RegA = 0;
                }
            } else if (strcmp (E->Arg, "tosorax") == 0) {
                if (RegValIsKnown (In->RegA) && In->RegA == 0xFF) {
                    Out->RegA = 0xFF;
                }
                if (RegValIsKnown (In->RegX) && In->RegX == 0xFF) {
                    Out->RegX = 0xFF;
                }
            } else if (strcmp (E->Arg, "tosshlax") == 0) {
                if (RegValIsKnown (In->RegA) && (In->RegA & 0x0F) >= 8) {
                    Out->RegA = 0;
                }
            } else if (strcmp (E->Arg, "tosshrax") == 0) {
                if (RegValIsKnown (In->RegA) && (In->RegA & 0x0F) >= 8) {
                    Out->RegX = 0;
                }
            } else if (strcmp (E->Arg, "bcastax") == 0     ||
                       strcmp (E->Arg, "bnegax") == 0      ||
                       FindBoolCmpCond (E->Arg) != CMP_INV ||
                       FindTosCmpCond (E->Arg) != CMP_INV) {
                /* Result is boolean value, so X is zero on output */
                Out->RegX = 0;
            }
            break;

        case OP65_JVC:
            BranchDeduceOnProcessorFlag (Out, BranchOut, PFVAL_V);
            break;

        case OP65_JVS:
            BranchDeduceOnProcessorFlag (BranchOut, Out, PFVAL_V);
            break;

        case OP65_LDA:
            Out->RegA = UNKNOWN_REGVAL;
            Out->ZNRegs = ZNREG_A;
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
                        break;
                }
            }
            DeduceZN (Out, Out->RegA);
            break;

        case OP65_LDX:
            Out->RegX = UNKNOWN_REGVAL;
            Out->ZNRegs = ZNREG_X;
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
                        break;
                }
            }
            DeduceZN (Out, Out->RegX);
            break;

        case OP65_LDY:
            Out->RegY = UNKNOWN_REGVAL;
            Out->ZNRegs = ZNREG_Y;
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
                        break;
                }
            }
            DeduceZN (Out, Out->RegY);
            break;

        case OP65_LSR:
            Out->PFlags |= UNKNOWN_PFVAL_CZN;
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ACC) {
                Out->RegA = AnyOpLSRDeduceCZN (Out, In->RegA);
                Out->ZNRegs = ZNREG_A;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
                    case REG_TMP1:
                        Out->Tmp1 = AnyOpLSRDeduceCZN (Out, In->Tmp1);
                        Out->ZNRegs = ZNREG_TMP1;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = AnyOpLSRDeduceCZN (Out, In->Ptr1Lo);
                        Out->ZNRegs = ZNREG_PTR1_LO;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = AnyOpLSRDeduceCZN (Out, In->Ptr1Hi);
                        Out->ZNRegs = ZNREG_PTR1_HI;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = AnyOpLSRDeduceCZN (Out, In->SRegLo);
                        Out->ZNRegs = ZNREG_SREG_LO;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = AnyOpLSRDeduceCZN (Out, In->SRegHi);
                        Out->ZNRegs = ZNREG_SREG_HI;
                        break;
                }
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_NOP:
            break;

        case OP65_ORA:
            Out->RegA = UNKNOWN_REGVAL;
            Out->ZNRegs = ZNREG_A;
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
                            break;
                    }
                }
            } else if (CE_IsKnownImm (E, 0xFF)) {
                /* ORA with 0xFF does always give 0xFF */
                Out->RegA = 0xFF;
            }
            DeduceZN (Out, Out->RegA);
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
            Out->PFlags |= UNKNOWN_PFVAL_ZN;
            Out->ZNRegs = ZNREG_A;
            break;

        case OP65_PLP:
            Out->PFlags = UNKNOWN_PFVAL_ALL;
            Out->ZNRegs = ZNREG_NONE;
            break;

        case OP65_PLX:
            Out->RegX = UNKNOWN_REGVAL;
            Out->PFlags |= UNKNOWN_PFVAL_ZN;
            Out->ZNRegs = ZNREG_X;
            break;

        case OP65_PLY:
            Out->RegY = UNKNOWN_REGVAL;
            Out->PFlags |= UNKNOWN_PFVAL_ZN;
            Out->ZNRegs = ZNREG_Y;
            break;

        case OP65_ROL:
            Out->PFlags |= UNKNOWN_PFVAL_CZN;
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ACC) {
                Out->RegA = AnyOpROLDeduceCZN (Out, In->PFlags, In->RegA);
                Out->ZNRegs = ZNREG_A;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
                    case REG_TMP1:
                        Out->Tmp1 = AnyOpROLDeduceCZN (Out, In->PFlags, In->Tmp1);
                        Out->ZNRegs = ZNREG_TMP1;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = AnyOpROLDeduceCZN (Out, In->PFlags, In->Ptr1Lo);
                        Out->ZNRegs = ZNREG_PTR1_LO;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = AnyOpROLDeduceCZN (Out, In->PFlags, In->Ptr1Hi);
                        Out->ZNRegs = ZNREG_PTR1_HI;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = AnyOpROLDeduceCZN (Out, In->PFlags, In->SRegLo);
                        Out->ZNRegs = ZNREG_SREG_LO;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = AnyOpROLDeduceCZN (Out, In->PFlags, In->SRegHi);
                        Out->ZNRegs = ZNREG_SREG_HI;
                        break;
                }
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_ROR:
            Out->PFlags |= UNKNOWN_PFVAL_CZN;
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ACC) {
                Out->RegA = AnyOpRORDeduceCZN (Out, In->PFlags, In->RegA);
                Out->ZNRegs = ZNREG_A;
            } else if (E->AM == AM65_ZP) {
                switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
                    case REG_TMP1:
                        Out->Tmp1 = AnyOpRORDeduceCZN (Out, In->PFlags, In->Tmp1);
                        Out->ZNRegs = ZNREG_TMP1;
                        break;
                    case REG_PTR1_LO:
                        Out->Ptr1Lo = AnyOpRORDeduceCZN (Out, In->PFlags, In->Ptr1Lo);
                        Out->ZNRegs = ZNREG_PTR1_LO;
                        break;
                    case REG_PTR1_HI:
                        Out->Ptr1Hi = AnyOpRORDeduceCZN (Out, In->PFlags, In->Ptr1Hi);
                        Out->ZNRegs = ZNREG_PTR1_HI;
                        break;
                    case REG_SREG_LO:
                        Out->SRegLo = AnyOpRORDeduceCZN (Out, In->PFlags, In->SRegLo);
                        Out->ZNRegs = ZNREG_SREG_LO;
                        break;
                    case REG_SREG_HI:
                        Out->SRegHi = AnyOpRORDeduceCZN (Out, In->PFlags, In->SRegHi);
                        Out->ZNRegs = ZNREG_SREG_HI;
                        break;
                }
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_RTI:
            Out->PFlags |= UNKNOWN_PFVAL_ALL;
            break;

        case OP65_RTS:
            break;

        case OP65_SBC:
            Out->RegA = UNKNOWN_REGVAL;
            Out->PFlags |= UNKNOWN_PFVAL_CZVN;
            Out->ZNRegs = ZNREG_A;
            if (PStatesAreKnown (In->PFlags, PSTATE_C) &&
                RegValIsKnown (In->RegA)) {
                if (CE_IsConstImm (E)) {
                    Out->RegA = KnownOpSBCDeduceCZVN (Out, In, (short) E->Num);
                }
                else if (E->AM == AM65_ZP) {
                    switch (GetKnownReg(E->Use & REG_ZP, In)) {
                    case REG_TMP1:
                        Out->RegA = KnownOpSBCDeduceCZVN (Out, In, In->Tmp1);
                        break;
                    case REG_PTR1_LO:
                        Out->RegA = KnownOpSBCDeduceCZVN (Out, In, In->Ptr1Lo);
                        break;
                    case REG_PTR1_HI:
                        Out->RegA = KnownOpSBCDeduceCZVN (Out, In, In->Ptr1Hi);
                        break;
                    case REG_SREG_LO:
                        Out->RegA = KnownOpSBCDeduceCZVN (Out, In, In->SRegLo);
                        break;
                    case REG_SREG_HI:
                        Out->RegA = KnownOpSBCDeduceCZVN (Out, In, In->SRegHi);
                        break;
                    default:
                        break;
                    }
                }
            }
            break;

        case OP65_SEC:
            Out->PFlags &= ~UNKNOWN_PFVAL_C;
            Out->PFlags |= PFVAL_C;
            break;

        case OP65_SED:
            Out->PFlags &= ~UNKNOWN_PFVAL_D;
            Out->PFlags |= PFVAL_D;
            break;

        case OP65_SEI:
            Out->PFlags &= ~UNKNOWN_PFVAL_I;
            Out->PFlags |= PFVAL_I;
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
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_STP:
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
            } else if (MightAffectKnownZP (E, In)) {
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
            } else if (MightAffectKnownZP (E, In)) {
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
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            break;

        case OP65_TAX:
            Out->RegX = In->RegA;
            Out->ZNRegs = ZNREG_AX;
            DeduceZN (Out, Out->RegX);
            break;

        case OP65_TAY:
            Out->RegY = In->RegA;
            Out->ZNRegs = ZNREG_AY;
            DeduceZN (Out, Out->RegY);
            break;

        case OP65_TRB:
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ZP) {
                if (RegValIsKnown (In->RegA)) {
                    switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                        case REG_TMP1:
                            Val = Out->Tmp1 & In->RegA;
                            Out->Tmp1 &= ~In->RegA;
                            break;
                        case REG_PTR1_LO:
                            Val = Out->Ptr1Lo & In->RegA;
                            Out->Ptr1Lo &= ~In->RegA;
                            break;
                        case REG_PTR1_HI:
                            Val = Out->Ptr1Hi & In->RegA;
                            Out->Ptr1Hi &= ~In->RegA;
                            break;
                        case REG_SREG_LO:
                            Val = Out->SRegLo & In->RegA;
                            Out->SRegLo &= ~In->RegA;
                            break;
                        case REG_SREG_HI:
                            Val = Out->SRegHi & In->RegA;
                            Out->SRegHi &= ~In->RegA;
                            break;
                    }
                } else {
                    switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
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
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            DeduceZ (Out, Val);
            break;

        case OP65_TSB:
            Out->ZNRegs = ZNREG_NONE;
            if (E->AM == AM65_ZP) {
                if (RegValIsKnown (In->RegA)) {
                    switch (GetKnownReg (E->Chg & REG_ZP, In)) {
                        case REG_TMP1:
                            Val = Out->Tmp1 & In->RegA;
                            Out->Tmp1 |= In->RegA;
                            break;
                        case REG_PTR1_LO:
                            Val = Out->Ptr1Lo & In->RegA;
                            Out->Ptr1Lo |= In->RegA;
                            break;
                        case REG_PTR1_HI:
                            Val = Out->Ptr1Hi & In->RegA;
                            Out->Ptr1Hi |= In->RegA;
                            break;
                        case REG_SREG_LO:
                            Val = Out->SRegLo & In->RegA;
                            Out->SRegLo |= In->RegA;
                            break;
                        case REG_SREG_HI:
                            Val = Out->SRegHi & In->RegA;
                            Out->SRegHi |= In->RegA;
                            break;
                    }
                } else {
                    switch (GetKnownReg (E->Chg & REG_ZP, 0)) {
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
            } else if (MightAffectKnownZP (E, In)) {
                /* Invalidates all ZP registers */
                RC_InvalidateZP (Out);
            }
            DeduceZ (Out, Val);
            break;

        case OP65_TSX:
            Out->RegX = UNKNOWN_REGVAL;
            Out->PFlags |= UNKNOWN_PFVAL_ZN;
            Out->ZNRegs = ZNREG_X;
            break;

        case OP65_TXA:
            Out->RegA = In->RegX;
            Out->ZNRegs = ZNREG_AX;
            DeduceZN (Out, Out->RegA);
            break;

        case OP65_TXS:
            Out->ZNRegs = ZNREG_X;
            DeduceZN (Out, In->RegX);
            break;

        case OP65_TYA:
            Out->RegA = In->RegY;
            Out->ZNRegs = ZNREG_AY;
            DeduceZN (Out, Out->RegA);
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
    sprintf (Buf + 10, "_%02X", (U & PSTATE_ALL) >> PSTATE_BITS_SHIFT);

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
        strcpy (B, "Y:XX ");
    } else {
        sprintf (B, "Y:%02X ", RC->RegY);
    }
    B += 5;
    if (PStatesAreUnknown (RC->PFlags, PSTATE_C)) {
        strcpy (B, "~");
    } else {
        strcpy (B, RC->PFlags & PFVAL_C ? "C" : "_");
    }
    B += 1;
    if (PStatesAreUnknown (RC->PFlags, PSTATE_Z)) {
        strcpy (B, "~");
    } else {
        strcpy (B, RC->PFlags & PFVAL_Z ? "Z" : "_");
    }
    B += 1;
    if (PStatesAreUnknown (RC->PFlags, PSTATE_I)) {
        strcpy (B, "~");
    } else {
        strcpy (B, RC->PFlags & PFVAL_I ? "I" : "_");
    }
    B += 1;
    if (PStatesAreUnknown (RC->PFlags, PSTATE_D)) {
        strcpy (B, "~");
    } else {
        strcpy (B, RC->PFlags & PFVAL_D ? "D" : "_");
    }
    B += 1;
    if (PStatesAreUnknown (RC->PFlags, PSTATE_U)) {
        strcpy (B, "~");
    } else {
        strcpy (B, RC->PFlags & PFVAL_U ? "U" : "_");
    }
    B += 1;
    if (PStatesAreUnknown (RC->PFlags, PSTATE_B)) {
        strcpy (B, "~");
    } else {
        strcpy (B, RC->PFlags & PFVAL_B ? "B" : "_");
    }
    B += 1;
    if (PStatesAreUnknown (RC->PFlags, PSTATE_V)) {
        strcpy (B, "~");
    } else {
        strcpy (B, RC->PFlags & PFVAL_V ? "V" : "_");
    }
    B += 1;
    if (PStatesAreUnknown (RC->PFlags, PSTATE_N)) {
        strcpy (B, "~");
    } else {
        strcpy (B, RC->PFlags & PFVAL_N ? "N" : "_");
    }
    B += 1;

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
        WriteOutput ("%*s; USE: %-15s CHG: %-15s SIZE: %u",
                     (int)(30-Chars), "",
                     RegInfoDesc (E->Use, Use),
                     RegInfoDesc (E->Chg, Chg),
                     E->Size);

        if (E->RI) {
            char RegIn[32];
            char RegOut[32];
            WriteOutput ("   In %s   Out %s",
                         RegContentDesc (&E->RI->In, RegIn),
                         RegContentDesc (&E->RI->Out, RegOut));
        }
    }

    /* Terminate the line */
    WriteOutput ("\n");
}
