/*****************************************************************************/
/*                                                                           */
/*                                 opcodes.c                                 */
/*                                                                           */
/*                  Opcode and addressing mode definitions                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2004 Ullrich von Bassewitz                                       */
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



#include "stdlib.h"
#include <string.h>
#include <ctype.h>

/* common */
#include "check.h"
#include "cpu.h"

/* cc65 */
#include "codeinfo.h"
#include "error.h"
#include "opcodes.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Opcode description table */
const OPCDesc OPCTable[OP65_COUNT] = {

    /* 65XX opcodes */
    {   OP65_ADC,                               /* opcode */
        "adc",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_READ,                      /* flags */
        REG_A | PSTATE_C,                       /* use */
        REG_A | PSTATE_CZVN                     /* chg */
    },
    {   OP65_AND,                               /* opcode */
        "and",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_READ,                      /* flags */
        REG_A,                                  /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
    {   OP65_ASL,                               /* opcode */
        "asl",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_NOIMP | OF_RMW,            /* flags */
        REG_NONE,                               /* use */
        PSTATE_CZN                              /* chg */
    },
    {   OP65_BCC,                               /* opcode */
        "bcc",                                  /* mnemonic */
        2,                                      /* size */
        OF_CBRA,                                /* flags */
        PSTATE_C,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_BCS,                               /* opcode */
        "bcs",                                  /* mnemonic */
        2,                                      /* size */
        OF_CBRA,                                /* flags */
        PSTATE_C,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_BEQ,                               /* opcode */
        "beq",                                  /* mnemonic */
        2,                                      /* size */
        OF_CBRA | OF_ZBRA | OF_FBRA,            /* flags */
        PSTATE_Z,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_BIT,                               /* opcode */
        "bit",                                  /* mnemonic */
        0,                                      /* size */
        OF_READ,                                /* flags */
        REG_A,                                  /* use */
        PSTATE_ZVN                              /* chg */
    },
    {   OP65_BMI,                               /* opcode */
        "bmi",                                  /* mnemonic */
        2,                                      /* size */
        OF_CBRA | OF_FBRA,                      /* flags */
        PSTATE_N,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_BNE,                               /* opcode */
        "bne",                                  /* mnemonic */
        2,                                      /* size */
        OF_CBRA | OF_ZBRA | OF_FBRA,            /* flags */
        PSTATE_Z,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_BPL,                               /* opcode */
        "bpl",                                  /* mnemonic */
        2,                                      /* size */
        OF_CBRA | OF_FBRA,                      /* flags */
        PSTATE_N,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_BRA,                               /* opcode */
        "bra",                                  /* mnemonic */
        2,                                      /* size */
        OF_UBRA,                                /* flags */
        REG_NONE,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_BRK,                               /* opcode */
        "brk",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_B                                /* chg */
    },
    {   OP65_BVC,                               /* opcode */
        "bvc",                                  /* mnemonic */
        2,                                      /* size */
        OF_CBRA,                                /* flags */
        PSTATE_V,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_BVS,                               /* opcode */
        "bvs",                                  /* mnemonic */
        2,                                      /* size */
        OF_CBRA,                                /* flags */
        PSTATE_V,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_CLC,                               /* opcode */
        "clc",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_C                                /* chg */
    },
    {   OP65_CLD,                               /* opcode */
        "cld",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_D                                /* chg */
    },
    {   OP65_CLI,                               /* opcode */
        "cli",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_I                                /* chg */
    },
    {   OP65_CLV,                               /* opcode */
        "clv",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_V                                /* chg */
    },
    {   OP65_CMP,                               /* opcode */
        "cmp",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_CMP | OF_READ,             /* flags */
        REG_A,                                  /* use */
        PSTATE_CZN                              /* chg */
    },
    {   OP65_CPX,                               /* opcode */
        "cpx",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_CMP | OF_READ,             /* flags */
        REG_X,                                  /* use */
        PSTATE_CZN                              /* chg */
    },
    {   OP65_CPY,                               /* opcode */
        "cpy",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_CMP | OF_READ,             /* flags */
        REG_Y,                                  /* use */
        PSTATE_CZN                              /* chg */
    },
    {   OP65_DEA,                               /* opcode */
        "dea",                                  /* mnemonic */
        1,                                      /* size */
        OF_REG_INCDEC | OF_SETF,                /* flags */
        REG_A,                                  /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
    {   OP65_DEC,                               /* opcode */
        "dec",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_NOIMP | OF_RMW,            /* flags */
        REG_NONE,                               /* use */
        PSTATE_ZN                               /* chg */
    },
    {   OP65_DEX,                               /* opcode */
        "dex",                                  /* mnemonic */
        1,                                      /* size */
        OF_REG_INCDEC | OF_SETF,                /* flags */
        REG_X,                                  /* use */
        REG_X | PSTATE_ZN                       /* chg */
    },
    {   OP65_DEY,                               /* opcode */
        "dey",                                  /* mnemonic */
        1,                                      /* size */
        OF_REG_INCDEC | OF_SETF,                /* flags */
        REG_Y,                                  /* use */
        REG_Y | PSTATE_ZN                       /* chg */
    },
    {   OP65_EOR,                               /* opcode */
        "eor",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_READ,                      /* flags */
        REG_A,                                  /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
    {   OP65_INA,                               /* opcode */
        "ina",                                  /* mnemonic */
        1,                                      /* size */
        OF_REG_INCDEC | OF_SETF,                /* flags */
        REG_A,                                  /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
    {   OP65_INC,                               /* opcode */
        "inc",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_NOIMP | OF_RMW,            /* flags */
        REG_NONE,                               /* use */
        PSTATE_ZN                               /* chg */
    },
    {   OP65_INX,                               /* opcode */
        "inx",                                  /* mnemonic */
        1,                                      /* size */
        OF_REG_INCDEC | OF_SETF,                /* flags */
        REG_X,                                  /* use */
        REG_X | PSTATE_ZN                       /* chg */
    },
    {   OP65_INY,                               /* opcode */
        "iny",                                  /* mnemonic */
        1,                                      /* size */
        OF_REG_INCDEC | OF_SETF,                /* flags */
        REG_Y,                                  /* use */
        REG_Y | PSTATE_ZN                       /* chg */
    },
    {   OP65_JCC,                               /* opcode */
        "jcc",                                  /* mnemonic */
        5,                                      /* size */
        OF_CBRA | OF_LBRA,                      /* flags */
        PSTATE_C,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JCS,                               /* opcode */
        "jcs",                                  /* mnemonic */
        5,                                      /* size */
        OF_CBRA | OF_LBRA,                      /* flags */
        PSTATE_C,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JEQ,                               /* opcode */
        "jeq",                                  /* mnemonic */
        5,                                      /* size */
        OF_CBRA | OF_LBRA | OF_ZBRA | OF_FBRA,  /* flags */
        PSTATE_Z,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JMI,                               /* opcode */
        "jmi",                                  /* mnemonic */
        5,                                      /* size */
        OF_CBRA | OF_LBRA | OF_FBRA,            /* flags */
        PSTATE_N,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JMP,                               /* opcode */
        "jmp",                                  /* mnemonic */
        3,                                      /* size */
        OF_UBRA | OF_LBRA | OF_READ,            /* flags */
        REG_NONE,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JNE,                               /* opcode */
        "jne",                                  /* mnemonic */
        5,                                      /* size */
        OF_CBRA | OF_LBRA | OF_ZBRA | OF_FBRA,  /* flags */
        PSTATE_Z,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JPL,                               /* opcode */
        "jpl",                                  /* mnemonic */
        5,                                      /* size */
        OF_CBRA | OF_LBRA | OF_FBRA,            /* flags */
        PSTATE_N,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JSR,                               /* opcode */
        "jsr",                                  /* mnemonic */
        3,                                      /* size */
        OF_CALL | OF_READ,                      /* flags */
        REG_NONE,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JVC,                               /* opcode */
        "jvc",                                  /* mnemonic */
        5,                                      /* size */
        OF_CBRA | OF_LBRA,                      /* flags */
        PSTATE_V,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_JVS,                               /* opcode */
        "jvs",                                  /* mnemonic */
        5,                                      /* size */
        OF_CBRA | OF_LBRA,                      /* flags */
        PSTATE_V,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_LDA,                               /* opcode */
        "lda",                                  /* mnemonic */
        0,                                      /* size */
        OF_LOAD | OF_SETF | OF_READ,            /* flags */
        REG_NONE,                               /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
    {   OP65_LDX,                               /* opcode */
        "ldx",                                  /* mnemonic */
        0,                                      /* size */
        OF_LOAD | OF_SETF | OF_READ,            /* flags */
        REG_NONE,                               /* use */
        REG_X | PSTATE_ZN                       /* chg */
    },
    {   OP65_LDY,                               /* opcode */
        "ldy",                                  /* mnemonic */
        0,                                      /* size */
        OF_LOAD | OF_SETF | OF_READ,            /* flags */
        REG_NONE,                               /* use */
        REG_Y | PSTATE_ZN                       /* chg */
    },
    {   OP65_LSR,                               /* opcode */
        "lsr",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_NOIMP | OF_RMW,            /* flags */
        REG_NONE,                               /* use */
        PSTATE_CZN                              /* chg */
    },
    {   OP65_NOP,                               /* opcode */
        "nop",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_ORA,                               /* opcode */
        "ora",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_READ,                      /* flags */
        REG_A,                                  /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
    {   OP65_PHA,                               /* opcode */
        "pha",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_A,                                  /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_PHP,                               /* opcode */
        "php",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        PSTATE_ALL,                             /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_PHX,                               /* opcode */
        "phx",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_X,                                  /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_PHY,                               /* opcode */
        "phy",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_Y,                                  /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_PLA,                               /* opcode */
        "pla",                                  /* mnemonic */
        1,                                      /* size */
        OF_SETF,                                /* flags */
        REG_NONE,                               /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
    {   OP65_PLP,                               /* opcode */
        "plp",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_ALL                              /* chg */
    },
    {   OP65_PLX,                               /* opcode */
        "plx",                                  /* mnemonic */
        1,                                      /* size */
        OF_SETF,                                /* flags */
        REG_NONE,                               /* use */
        REG_X | PSTATE_ZN                       /* chg */
    },
    {   OP65_PLY,                               /* opcode */
        "ply",                                  /* mnemonic */
        1,                                      /* size */
        OF_SETF,                                /* flags */
        REG_NONE,                               /* use */
        REG_Y | PSTATE_ZN                       /* chg */
    },
    {   OP65_ROL,                               /* opcode */
        "rol",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_NOIMP | OF_RMW,            /* flags */
        PSTATE_C,                               /* use */
        PSTATE_CZN                              /* chg */
    },
    {   OP65_ROR,                               /* opcode */
        "ror",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_NOIMP | OF_RMW,            /* flags */
        PSTATE_C,                               /* use */
        PSTATE_CZN                              /* chg */
    },
    /* Mark RTI as "uses all registers but doesn't change them", so the
    ** optimizer won't remove preceeding loads.
    */
    {   OP65_RTI,                               /* opcode */
        "rti",                                  /* mnemonic */
        1,                                      /* size */
        OF_RET,                                 /* flags */
        REG_AXY,                                /* use */
        PSTATE_ALL                              /* chg */
    },
    {   OP65_RTS,                               /* opcode */
        "rts",                                  /* mnemonic */
        1,                                      /* size */
        OF_RET,                                 /* flags */
        REG_NONE,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_SBC,                               /* opcode */
        "sbc",                                  /* mnemonic */
        0,                                      /* size */
        OF_SETF | OF_READ,                      /* flags */
        REG_A | PSTATE_C,                       /* use */
        REG_A | PSTATE_CZVN                     /* chg */
    },
    {   OP65_SEC,                               /* opcode */
        "sec",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_C                                /* chg */
    },
    {   OP65_SED,                               /* opcode */
        "sed",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_D                                /* chg */
    },
    {   OP65_SEI,                               /* opcode */
        "sei",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        PSTATE_I                                /* chg */
    },
    {   OP65_STA,                               /* opcode */
        "sta",                                  /* mnemonic */
        0,                                      /* size */
        OF_STORE | OF_WRITE,                    /* flags */
        REG_A,                                  /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_STP,                               /* opcode */
        "stp",                                  /* mnemonic */
        1,                                      /* size */
        OF_NONE,                                /* flags */
        REG_NONE,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_STX,                               /* opcode */
        "stx",                                  /* mnemonic */
        0,                                      /* size */
        OF_STORE | OF_WRITE,                    /* flags */
        REG_X,                                  /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_STY,                               /* opcode */
        "sty",                                  /* mnemonic */
        0,                                      /* size */
        OF_STORE | OF_WRITE,                    /* flags */
        REG_Y,                                  /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_STZ,                               /* opcode */
        "stz",                                  /* mnemonic */
        0,                                      /* size */
        OF_STORE | OF_WRITE,                    /* flags */
        REG_NONE,                               /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_TAX,                               /* opcode */
        "tax",                                  /* mnemonic */
        1,                                      /* size */
        OF_XFR | OF_SETF,                       /* flags */
        REG_A,                                  /* use */
        REG_X | PSTATE_ZN                       /* chg */
    },
    {   OP65_TAY,                               /* opcode */
        "tay",                                  /* mnemonic */
        1,                                      /* size */
        OF_XFR | OF_SETF,                       /* flags */
        REG_A,                                  /* use */
        REG_Y | PSTATE_ZN                       /* chg */
    },
    {   OP65_TRB,                               /* opcode */
        "trb",                                  /* mnemonic */
        0,                                      /* size */
        OF_RMW,                                 /* flags */
        REG_A,                                  /* use */
        PSTATE_Z                                /* chg */
    },
    {   OP65_TSB,                               /* opcode */
        "tsb",                                  /* mnemonic */
        0,                                      /* size */
        OF_RMW,                                 /* flags */
        REG_A,                                  /* use */
        PSTATE_Z                                /* chg */
    },
    {   OP65_TSX,                               /* opcode */
        "tsx",                                  /* mnemonic */
        1,                                      /* size */
        OF_XFR | OF_SETF,                       /* flags */
        REG_NONE,                               /* use */
        REG_X | PSTATE_ZN                       /* chg */
    },
    {   OP65_TXA,                               /* opcode */
        "txa",                                  /* mnemonic */
        1,                                      /* size */
        OF_XFR | OF_SETF,                       /* flags */
        REG_X,                                  /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
    {   OP65_TXS,                               /* opcode */
        "txs",                                  /* mnemonic */
        1,                                      /* size */
        OF_XFR,                                 /* flags */
        REG_X,                                  /* use */
        REG_NONE                                /* chg */
    },
    {   OP65_TYA,                               /* opcode */
        "tya",                                  /* mnemonic */
        1,                                      /* size */
        OF_XFR | OF_SETF,                       /* flags */
        REG_Y,                                  /* use */
        REG_A | PSTATE_ZN                       /* chg */
    },
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int FindCmp (const void* Key, const void* Desc)
/* Compare function for FindOpcode */
{
    return strcmp (Key, ((OPCDesc*)Desc)->Mnemo);
}



const OPCDesc* FindOP65 (const char* M)
/* Find the given opcode and return the opcode number. If the opcode was not
** found, return NULL.
*/
{
    unsigned I;
    unsigned Len;

    /* Check the length of the given string, then copy it into local
    ** storage, converting it to upper case.
    */
    char Mnemo[sizeof (OPCTable[0].Mnemo)];
    Len = strlen (M);
    if (Len >= sizeof (OPCTable[0].Mnemo)) {
        /* Invalid length means invalid opcode */
        return 0;
    }
    for (I = 0; I < Len; ++I) {
        Mnemo[I] = tolower (M[I]);
    }
    Mnemo[I] = '\0';

    /* Search for the mnemonic in the table and return the result */
    return bsearch (Mnemo, OPCTable, OP65_COUNT,
                    sizeof (OPCTable[0]), FindCmp );
}



unsigned GetInsnSize (opc_t OPC, am_t AM)
/* Return the size of the given instruction */
{
    /* Get the opcode desc and check the size given there */
    const OPCDesc* D = &OPCTable[OPC];
    if (D->Size != 0) {
        return D->Size;
    }

    /* Check the addressing mode. */
    switch (AM) {
        case AM65_IMP:     return 1;
        case AM65_ACC:     return 1;
        case AM65_IMM:     return 2;
        case AM65_ZP:      return 2;
        case AM65_ZPX:     return 2;
        case AM65_ZPY:     return 2;
        case AM65_ABS:     return 3;
        case AM65_ABSX:    return 3;
        case AM65_ABSY:    return 3;
        case AM65_ZPX_IND: return 2;
        case AM65_ZP_INDY: return 2;
        case AM65_ZP_IND:  return 2;
        default:
            Internal ("Invalid addressing mode");
            return 0;
    }
}



unsigned char GetAMUseInfo (am_t AM)
/* Get usage info for the given addressing mode (addressing modes that use
** index registers return REG_r info for these registers).
*/
{
    /* Check the addressing mode. */
    switch (AM) {
        case AM65_ACC:     return REG_A;
        case AM65_ZPX:     return REG_X;
        case AM65_ABSX:    return REG_X;
        case AM65_ABSY:    return REG_Y;
        case AM65_ZPX_IND: return REG_X;
        case AM65_ZP_INDY: return REG_Y;
        default:           return REG_NONE;
    }
}



opc_t GetInverseBranch (opc_t OPC)
/* Return a branch that reverse the condition of the branch given in OPC */
{
    switch (OPC) {
        case OP65_BCC:  return OP65_BCS;
        case OP65_BCS:  return OP65_BCC;
        case OP65_BEQ:  return OP65_BNE;
        case OP65_BMI:  return OP65_BPL;
        case OP65_BNE:  return OP65_BEQ;
        case OP65_BPL:  return OP65_BMI;
        case OP65_BVC:  return OP65_BVS;
        case OP65_BVS:  return OP65_BVC;
        case OP65_JCC:  return OP65_JCS;
        case OP65_JCS:  return OP65_JCC;
        case OP65_JEQ:  return OP65_JNE;
        case OP65_JMI:  return OP65_JPL;
        case OP65_JNE:  return OP65_JEQ;
        case OP65_JPL:  return OP65_JMI;
        case OP65_JVC:  return OP65_JVS;
        case OP65_JVS:  return OP65_JVC;
        default:
            Internal ("GetInverseBranch: Invalid opcode: %d", OPC);
            return 0;
    }
}



opc_t MakeShortBranch (opc_t OPC)
/* Return the short version of the given branch. If the branch is already
** a short branch, return the opcode unchanged.
*/
{
    switch (OPC) {
        case OP65_BCC:
        case OP65_JCC:  return OP65_BCC;
        case OP65_BCS:
        case OP65_JCS:  return OP65_BCS;
        case OP65_BEQ:
        case OP65_JEQ:  return OP65_BEQ;
        case OP65_BMI:
        case OP65_JMI:  return OP65_BMI;
        case OP65_BNE:
        case OP65_JNE:  return OP65_BNE;
        case OP65_BPL:
        case OP65_JPL:  return OP65_BPL;
        case OP65_BVC:
        case OP65_JVC:  return OP65_BVC;
        case OP65_BVS:
        case OP65_JVS:  return OP65_BVS;
        case OP65_BRA:
        case OP65_JMP:  return (CPUIsets[CPU] & (CPU_ISET_65SC02 | CPU_ISET_6502DTV)) ? OP65_BRA : OP65_JMP;
        default:
            Internal ("MakeShortBranch: Invalid opcode: %d", OPC);
            return 0;
    }
}



opc_t MakeLongBranch (opc_t OPC)
/* Return the long version of the given branch. If the branch is already
** a long branch, return the opcode unchanged.
*/
{
    switch (OPC) {
        case OP65_BCC:
        case OP65_JCC:  return OP65_JCC;
        case OP65_BCS:
        case OP65_JCS:  return OP65_JCS;
        case OP65_BEQ:
        case OP65_JEQ:  return OP65_JEQ;
        case OP65_BMI:
        case OP65_JMI:  return OP65_JMI;
        case OP65_BNE:
        case OP65_JNE:  return OP65_JNE;
        case OP65_BPL:
        case OP65_JPL:  return OP65_JPL;
        case OP65_BVC:
        case OP65_JVC:  return OP65_JVC;
        case OP65_BVS:
        case OP65_JVS:  return OP65_JVS;
        case OP65_BRA:
        case OP65_JMP:  return OP65_JMP;
        default:
            Internal ("MakeLongBranch: Invalid opcode: %d", OPC);
            return 0;
    }
}



bc_t GetBranchCond (opc_t OPC)
/* Get the condition for the conditional branch in OPC */
{
    switch (OPC) {
        case OP65_BCC:  return BC_CC;
        case OP65_BCS:  return BC_CS;
        case OP65_BEQ:  return BC_EQ;
        case OP65_BMI:  return BC_MI;
        case OP65_BNE:  return BC_NE;
        case OP65_BPL:  return BC_PL;
        case OP65_BVC:  return BC_VC;
        case OP65_BVS:  return BC_VS;
        case OP65_JCC:  return BC_CC;
        case OP65_JCS:  return BC_CS;
        case OP65_JEQ:  return BC_EQ;
        case OP65_JMI:  return BC_MI;
        case OP65_JNE:  return BC_NE;
        case OP65_JPL:  return BC_PL;
        case OP65_JVC:  return BC_VC;
        case OP65_JVS:  return BC_VS;
        default:
            Internal ("GetBranchCond: Invalid opcode: %d", OPC);
            return 0;
    }
}



bc_t GetInverseCond (bc_t BC)
/* Return the inverse condition of the given one */
{
    switch (BC) {
        case BC_CC:     return BC_CS;
        case BC_CS:     return BC_CC;
        case BC_EQ:     return BC_NE;
        case BC_MI:     return BC_PL;
        case BC_NE:     return BC_EQ;
        case BC_PL:     return BC_MI;
        case BC_VC:     return BC_VS;
        case BC_VS:     return BC_VC;
        default:
            Internal ("GetInverseCond: Invalid condition: %d", BC);
            return 0;
    }
}
