/*****************************************************************************/
/*                                                                           */
/*                                 opcodes.h                                 */
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



#ifndef OPCODES_H
#define OPCODES_H



/* common */
#include "inline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* 65XX opcodes */
typedef enum {
    OP65_ADC,
    OP65_AND,
    OP65_ASL,
    OP65_BCC,
    OP65_BCS,
    OP65_BEQ,
    OP65_BIT,
    OP65_BMI,
    OP65_BNE,
    OP65_BPL,
    OP65_BRA,
    OP65_BRK,
    OP65_BVC,
    OP65_BVS,
    OP65_CLC,
    OP65_CLD,
    OP65_CLI,
    OP65_CLV,
    OP65_CMP,
    OP65_CPX,
    OP65_CPY,
    OP65_DEA,
    OP65_DEC,
    OP65_DEX,
    OP65_DEY,
    OP65_EOR,
    OP65_INA,
    OP65_INC,
    OP65_INX,
    OP65_INY,
    OP65_JCC,
    OP65_JCS,
    OP65_JEQ,
    OP65_JMI,
    OP65_JMP,
    OP65_JNE,
    OP65_JPL,
    OP65_JSR,
    OP65_JVC,
    OP65_JVS,
    OP65_LDA,
    OP65_LDX,
    OP65_LDY,
    OP65_LSR,
    OP65_NOP,
    OP65_ORA,
    OP65_PHA,
    OP65_PHP,
    OP65_PHX,
    OP65_PHY,
    OP65_PLA,
    OP65_PLP,
    OP65_PLX,
    OP65_PLY,
    OP65_ROL,
    OP65_ROR,
    OP65_RTI,
    OP65_RTS,
    OP65_SBC,
    OP65_SEC,
    OP65_SED,
    OP65_SEI,
    OP65_STA,
    OP65_STX,
    OP65_STY,
    OP65_STZ,
    OP65_TAX,
    OP65_TAY,
    OP65_TRB,
    OP65_TSB,
    OP65_TSX,
    OP65_TXA,
    OP65_TXS,
    OP65_TYA,

    /* Number of opcodes available */
    OP65_COUNT
} opc_t;

/* 65XX addressing modes */
typedef enum {
    AM65_IMP,                   /* implicit */
    AM65_ACC,                   /* accumulator */
    AM65_IMM,                   /* immidiate */
    AM65_ZP,                    /* zeropage */
    AM65_ZPX,                   /* zeropage,X */
    AM65_ZPY,                   /* zeropage,Y */
    AM65_ABS,                   /* absolute */
    AM65_ABSX,                  /* absolute,X */
    AM65_ABSY,                  /* absolute,Y */
    AM65_ZPX_IND,               /* (zeropage,x) */
    AM65_ZP_INDY,               /* (zeropage),y */
    AM65_ZP_IND,                /* (zeropage) */
    AM65_BRA                    /* branch */
} am_t;

/* Branch conditions */
typedef enum {
    BC_CC,
    BC_CS,
    BC_EQ,
    BC_MI,
    BC_NE,
    BC_PL,
    BC_VC,
    BC_VS
} bc_t;

/* Opcode info */
#define OF_NONE         0x0000U /* No additional information */
#define OF_UBRA         0x0001U /* Unconditional branch */
#define OF_CBRA         0x0002U /* Conditional branch */
#define OF_ZBRA         0x0004U /* Branch on zero flag condition */
#define OF_FBRA         0x0008U /* Branch on cond set by a load */
#define OF_LBRA         0x0010U /* Jump/branch is long */
#define OF_RET          0x0020U /* Return from function */
#define OF_LOAD         0x0040U /* Register load */
#define OF_STORE        0x0080U /* Register store */
#define OF_XFR          0x0100U /* Transfer instruction */
#define OF_CALL         0x0200U /* A subroutine call */
#define OF_REG_INCDEC   0x0400U /* A register increment or decrement */
#define OF_SETF         0x0800U /* Insn will set all load flags (not carry) */
#define OF_CMP          0x1000U /* A compare A/X/Y instruction */
#define OF_NOIMP        0x2000U /* Implicit addressing mode is actually A */

/* Combined infos */
#define OF_BRA  (OF_UBRA | OF_CBRA)     /* Operation is a jump/branch */
#define OF_DEAD (OF_UBRA | OF_RET)      /* Dead end - no exec behind this point */

/* Opcode description */
typedef struct {
    opc_t           OPC;                /* Opcode */
    char            Mnemo[9];           /* Mnemonic */
    unsigned char   Size;               /* Size, 0 = check addressing mode */
    unsigned short  Use;                /* Registers used by this insn */
    unsigned short  Chg;                /* Registers changed by this insn */
    unsigned short  Info;               /* Additional information */
} OPCDesc;

/* Opcode description table */
extern const OPCDesc OPCTable[OP65_COUNT];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const OPCDesc* FindOP65 (const char* OPC);
/* Find the given opcode and return the opcode description. If the opcode was
** not found, NULL is returned.
*/

unsigned GetInsnSize (opc_t OPC, am_t AM);
/* Return the size of the given instruction */

#if defined(HAVE_INLINE)
INLINE const OPCDesc* GetOPCDesc (opc_t OPC)
/* Get an opcode description */
{
    /* Return the description */
    return &OPCTable [OPC];
}
#else
#  define GetOPCDesc(OPC)       (&OPCTable [(OPC)])
#endif

#if defined(HAVE_INLINE)
INLINE unsigned GetOPCInfo (opc_t OPC)
/* Get opcode information */
{
    /* Return the info */
    return OPCTable[OPC].Info;
}
#else
#  define GetOPCInfo(OPC)       (OPCTable[(OPC)].Info)
#endif

unsigned char GetAMUseInfo (am_t AM);
/* Get usage info for the given addressing mode (addressing modes that use
** index registers return REG_r info for these registers).
*/

opc_t GetInverseBranch (opc_t OPC);
/* Return a branch that reverse the condition of the branch given in OPC */

opc_t MakeShortBranch (opc_t OPC);
/* Return the short version of the given branch. If the branch is already
** a short branch, return the opcode unchanged.
*/

opc_t MakeLongBranch (opc_t OPC);
/* Return the long version of the given branch. If the branch is already
** a long branch, return the opcode unchanged.
*/

bc_t GetBranchCond (opc_t OPC);
/* Get the condition for the conditional branch in OPC */

bc_t GetInverseCond (bc_t BC);
/* Return the inverse condition of the given one */



/* End of opcodes.h */

#endif
