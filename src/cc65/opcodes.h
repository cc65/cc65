/*****************************************************************************/
/*                                                                           */
/*				   opcodes.h				     */
/*                                                                           */
/*		    Opcode and addressing mode definitions		     */
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



#ifndef OPCODES_H
#define OPCODES_H



/* common */
#include "inline.h"



/*****************************************************************************/
/*  	       	 	  	     Data				     */
/*****************************************************************************/



/* Definitions for the possible opcodes */
typedef enum {
    OPC_ADC,
    OPC_AND,
    OPC_ASL,
    OPC_BCC,
    OPC_BCS,
    OPC_BEQ,
    OPC_BIT,
    OPC_BMI,
    OPC_BNE,
    OPC_BPL,
    OPC_BRA,
    OPC_BRK,
    OPC_BVC,
    OPC_BVS,
    OPC_CLC,
    OPC_CLD,
    OPC_CLI,
    OPC_CLV,
    OPC_CMP,
    OPC_CPX,
    OPC_CPY,
    OPC_DEA,
    OPC_DEC,
    OPC_DEX,
    OPC_DEY,
    OPC_EOR,
    OPC_INA,
    OPC_INC,
    OPC_INX,
    OPC_INY,
    OPC_JCC,
    OPC_JCS,
    OPC_JEQ,
    OPC_JMI,
    OPC_JMP,
    OPC_JNE,
    OPC_JPL,
    OPC_JSR,
    OPC_JVC,
    OPC_JVS,
    OPC_LDA,
    OPC_LDX,
    OPC_LDY,
    OPC_LSR,
    OPC_NOP,
    OPC_ORA,
    OPC_PHA,
    OPC_PHP,
    OPC_PHX,
    OPC_PHY,
    OPC_PLA,
    OPC_PLP,
    OPC_PLX,
    OPC_PLY,
    OPC_ROL,
    OPC_ROR,
    OPC_RTI,
    OPC_RTS,
    OPC_SBC,
    OPC_SEC,
    OPC_SED,
    OPC_SEI,
    OPC_STA,
    OPC_STX,
    OPC_STY,
    OPC_TAX,
    OPC_TAY,
    OPC_TRB,
    OPC_TSB,
    OPC_TSX,
    OPC_TXA,
    OPC_TXS,
    OPC_TYA,
    OPC_COUNT  	       	       		/* Number of opcodes available */
} opc_t;

/* Addressing modes (bitmapped). */
typedef enum {
    AM_IMP     	= 0x0001,      		/* implicit */
    AM_ACC     	= 0x0002,      		/* accumulator */
    AM_IMM     	= 0x0004,      		/* immidiate */
    AM_ZP      	= 0x0008,      		/* zeropage */
    AM_ZPX     	= 0x0010,      		/* zeropage,X */
    AM_ABS     	= 0x0020,      		/* absolute */
    AM_ABSX    	= 0x0040,      		/* absolute,X */
    AM_ABSY    	= 0x0080,      		/* absolute,Y */
    AM_ZPX_IND 	= 0x0100,      		/* (zeropage,x) */
    AM_ZP_INDY 	= 0x0200,      		/* (zeropage),y */
    AM_ZP_IND  	= 0x0400,      		/* (zeropage) */
    AM_BRA     	= 0x0800       		/* branch */
} am_t;

/* Branch conditions */
typedef enum {
    BC_CC,
    BC_CS,
    BC_EQ,
    BC_MI,
    BC_NE,
    BC_PL,
    BC_SR,
    BC_VC,
    BC_VS
} bc_t;

/* Opcode info */
#define OF_NONE	0x0000U	       		/* No additional information */
#define OF_UBRA	0x0001U	       	 	/* Unconditional branch */
#define OF_CBRA	0x0002U	       	 	/* Conditional branch */
#define OF_ZBRA 0x0004U		 	/* Branch on zero flag condition */
#define OF_FBRA 0x0008U                 /* Branch on cond set by a load */
#define OF_LBRA 0x0010U		  	/* Jump/branch is long */
#define OF_RET 	0x0020U	       	 	/* Return from function */
#define OF_LOAD 0x0040U			/* Register load */
#define OF_XFR  0x0080                  /* Transfer instruction */
#define OF_BRA 	(OF_UBRA|OF_CBRA)	/* Operation is a jump/branch */
#define OF_DEAD	(OF_UBRA|OF_RET)	/* Dead end - no exec behind this point */

/* Opcode description */
typedef struct {
    opc_t      	    OPC;       		/* Opcode */
    char       	    Mnemo[4];  		/* Mnemonic */
    unsigned char   Size;      		/* Size, 0 = check addressing mode */
    unsigned char   Use;       		/* Registers used by this insn */
    unsigned char   Chg;       		/* Registers changed by this insn */
    unsigned char   Info;      		/* Additional information */
} OPCDesc;

/* Opcode description table */
extern const OPCDesc OPCTable[OPC_COUNT];



/*****************************************************************************/
/*     	       	      	  	     Code		    		     */
/*****************************************************************************/



const OPCDesc* FindOpcode (const char* OPC);
/* Find the given opcode and return the opcode description. If the opcode was
 * not found, NULL is returned.
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
#  define GetOPCDesc(OPC)	(&OPCTable [(OPC)])
#endif

#if defined(HAVE_INLINE)
INLINE unsigned char GetOPCInfo (opc_t OPC)
/* Get opcode information */
{
    /* Return the info */
    return OPCTable[OPC].Info;
}
#else
#  define GetOPCInfo(OPC)	(OPCTable[(OPC)].Info)
#endif

unsigned char GetAMUseInfo (am_t AM);
/* Get usage info for the given addressing mode (addressing modes that use
 * index registers return REG_r info for these registers).
 */

opc_t GetInverseBranch (opc_t OPC);
/* Return a branch that reverse the condition of the branch given in OPC */

opc_t MakeShortBranch (opc_t OPC);
/* Return the short version of the given branch. If the branch is already
 * a short branch, return the opcode unchanged.
 */

opc_t MakeLongBranch (opc_t OPC);
/* Return the long version of the given branch. If the branch is already
 * a long branch, return the opcode unchanged.
 */

bc_t GetBranchCond (opc_t OPC);
/* Get the condition for the conditional branch in OPC */

bc_t GetInverseCond (bc_t BC);
/* Return the inverse condition of the given one */

opc_t GetLongBranch (bc_t BC);
/* Return a long branch for the given branch condition */

opc_t GetShortBranch (bc_t BC);
/* Return a short branch for the given branch condition */



/* End of opcodes.h */
#endif



