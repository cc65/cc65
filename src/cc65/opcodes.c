/*****************************************************************************/
/*                                                                           */
/*				   opcodes.c				     */
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



#include "stdlib.h"
#include <string.h>
#include <ctype.h>

/* common */
#include "check.h"

/* cc65 */
#include "codeinfo.h"
#include "cpu.h"
#include "error.h"
#include "opcodes.h"



/*****************************************************************************/
/*  	       	     	       	     Data				     */
/*****************************************************************************/



/* Opcode description table */
const OPCDesc OPCTable[OPC_COUNT] = {
    { OPC_ADC, "adc", 0, REG_A,	   REG_A,    OF_NONE			    },
    { OPC_AND, "and", 0, REG_A,    REG_A,    OF_NONE			    },
    { OPC_ASL, "asl", 0, REG_A,    REG_A,    OF_NONE			    },
    { OPC_BCC, "bcc", 2, REG_NONE, REG_NONE, OF_CBRA			    },
    { OPC_BCS, "bcs", 2, REG_NONE, REG_NONE, OF_CBRA			    },
    { OPC_BEQ, "beq", 2, REG_NONE, REG_NONE, OF_CBRA | OF_ZBRA | OF_FBRA    },
    { OPC_BIT, "bit", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OPC_BMI, "bmi", 2, REG_NONE, REG_NONE, OF_CBRA | OF_FBRA 	  	    },
    { OPC_BNE, "bne", 2, REG_NONE, REG_NONE, OF_CBRA | OF_ZBRA | OF_FBRA    },
    { OPC_BPL, "bpl", 2, REG_NONE, REG_NONE, OF_CBRA | OF_FBRA 	  	    },
    { OPC_BRA, "bra", 2, REG_NONE, REG_NONE, OF_UBRA			    },
    { OPC_BRK, "brk", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_BVC, "bvc", 2, REG_NONE, REG_NONE, OF_CBRA			    },
    { OPC_BVS, "bvs", 2, REG_NONE, REG_NONE, OF_CBRA			    },
    { OPC_CLC, "clc", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_CLD, "cld", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_CLI, "cli", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_CLV, "clv", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_CMP, "cmp", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OPC_CPX, "cpx", 0, REG_X,    REG_NONE, OF_NONE			    },
    { OPC_CPY, "cpy", 0, REG_Y,    REG_NONE, OF_NONE			    },
    { OPC_DEA, "dea", 1, REG_A,    REG_A,    OF_NONE			    },
    { OPC_DEC, "dec", 0, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_DEX, "dex", 1, REG_X,    REG_X,    OF_NONE			    },
    { OPC_DEY, "dey", 1, REG_Y,    REG_Y,    OF_NONE			    },
    { OPC_EOR, "eor", 0, REG_A,    REG_A,    OF_NONE			    },
    { OPC_INA, "ina", 1, REG_A,    REG_A,    OF_NONE			    },
    { OPC_INC, "inc", 0, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_INX, "inx", 1, REG_X,    REG_X,    OF_NONE			    },
    { OPC_INY, "iny", 1, REG_Y,    REG_Y,    OF_NONE			    },
    { OPC_JCC, "jcc", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA  	    },
    { OPC_JCS, "jcs", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA  	    },
    { OPC_JEQ, "jeq", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA | OF_ZBRA | OF_FBRA },
    { OPC_JMI, "jmi", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA | OF_FBRA    },
    { OPC_JMP, "jmp", 3, REG_NONE, REG_NONE, OF_UBRA | OF_LBRA  	    },
    { OPC_JNE, "jne", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA | OF_ZBRA | OF_FBRA },
    { OPC_JPL, "jpl", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA | OF_FBRA    },
    { OPC_JSR, "jsr", 3, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_JVC, "jvc", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA  	    },
    { OPC_JVS, "jvs", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA  	    },
    { OPC_LDA, "lda", 0, REG_NONE, REG_A,    OF_LOAD   			    },
    { OPC_LDX, "ldx", 0, REG_NONE, REG_X,    OF_LOAD			    },
    { OPC_LDY, "ldy", 0, REG_NONE, REG_Y,    OF_LOAD			    },
    { OPC_LSR, "lsr", 0, REG_A,    REG_A,    OF_NONE			    },
    { OPC_NOP, "nop", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_ORA, "ora", 0, REG_A,    REG_A,    OF_NONE			    },
    { OPC_PHA, "pha", 1, REG_A,    REG_NONE, OF_NONE			    },
    { OPC_PHP, "php", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_PHX, "phx", 1, REG_X,    REG_NONE, OF_NONE			    },
    { OPC_PHY, "phy", 1, REG_Y,    REG_NONE, OF_NONE			    },
    { OPC_PLA, "pla", 1, REG_NONE, REG_A,    OF_NONE			    },
    { OPC_PLP, "plp", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_PLX, "plx", 1, REG_NONE, REG_X,    OF_NONE			    },
    { OPC_PLY, "ply", 1, REG_NONE, REG_Y,    OF_NONE			    },
    { OPC_ROL, "rol", 0, REG_A,    REG_A,    OF_NONE			    },
    { OPC_ROR, "ror", 0, REG_A,    REG_A,    OF_NONE			    },
    { OPC_RTI, "rti", 1, REG_NONE, REG_NONE, OF_RET   			    },
    { OPC_RTS, "rts", 1, REG_NONE, REG_NONE, OF_RET   			    },
    { OPC_SBC, "sbc", 0, REG_A,    REG_A,    OF_NONE			    },
    { OPC_SEC, "sec", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_SED, "sed", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_SEI, "sei", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OPC_STA, "sta", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OPC_STX, "stx", 0, REG_X,    REG_NONE, OF_NONE			    },
    { OPC_STY, "sty", 0, REG_Y,    REG_NONE, OF_NONE			    },
    { OPC_TAX, "tax", 1, REG_A,    REG_X,    OF_XFR			    },
    { OPC_TAY, "tay", 1, REG_A,    REG_Y,    OF_XFR			    },
    { OPC_TRB, "trb", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OPC_TSB, "tsb", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OPC_TSX, "tsx", 1, REG_NONE, REG_X,    OF_XFR			    },
    { OPC_TXA, "txa", 1, REG_X,    REG_A,    OF_XFR			    },
    { OPC_TXS, "txs", 1, REG_X,    REG_NONE, OF_XFR			    },
    { OPC_TYA, "tya", 1, REG_A,    REG_A,    OF_XFR			    },
};



/*****************************************************************************/
/*     	       	       	  	     Code				     */
/*****************************************************************************/



static int Compare (const void* Key, const void* Desc)
/* Compare function for bsearch */
{
    return strcmp (Key, ((OPCDesc*)Desc)->Mnemo);
}



const OPCDesc* FindOpcode (const char* M)
/* Find the given opcode and return the opcode number. If the opcode was not
 * found, return OPC_INVALID.
 */
{
    unsigned I;
    unsigned Len;

    /* Check the length of the given string, then copy it into local
     * storage, converting it to upper case.
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
    return bsearch (Mnemo, OPCTable, OPC_COUNT, sizeof (OPCTable[0]), Compare);
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
	case AM_IMP:	  return 1;
	case AM_ACC:	  return 1;
	case AM_IMM:	  return 2;
	case AM_ZP:	  return 2;
	case AM_ZPX:	  return 2;
	case AM_ABS:	  return 3;
	case AM_ABSX:	  return 3;
	case AM_ABSY:	  return 3;
	case AM_ZPX_IND:  return 2;
	case AM_ZP_INDY:  return 2;
	case AM_ZP_IND:   return 2;
	default:	  FAIL ("Invalid addressing mode");
    }
}



unsigned char GetAMUseInfo (am_t AM)
/* Get usage info for the given addressing mode (addressing modes that use
 * index registers return REG_r info for these registers).
 */
{
    /* Check the addressing mode. */
    switch (AM) {
       	case AM_ACC:   	  return REG_A;
       	case AM_ZPX:   	  return REG_X;
       	case AM_ABSX:  	  return REG_X;
       	case AM_ABSY:  	  return REG_Y;
       	case AM_ZPX_IND:  return REG_X;
       	case AM_ZP_INDY:  return REG_Y;
       	default:       	  return REG_NONE;
    }
}



opc_t GetInverseBranch (opc_t OPC)
/* Return a branch that reverse the condition of the branch given in OPC */
{
    switch (OPC) {
	case OPC_BCC:	return OPC_BCS;
	case OPC_BCS:	return OPC_BCC;
	case OPC_BEQ:	return OPC_BNE;
	case OPC_BMI:	return OPC_BPL;
	case OPC_BNE:	return OPC_BEQ;
	case OPC_BPL:	return OPC_BMI;
	case OPC_BVC:	return OPC_BVS;
	case OPC_BVS:	return OPC_BVC;
       	case OPC_JCC:  	return OPC_JCS;
       	case OPC_JCS:  	return OPC_JCC;
       	case OPC_JEQ:  	return OPC_JNE;
       	case OPC_JMI:  	return OPC_JPL;
       	case OPC_JNE:  	return OPC_JEQ;
       	case OPC_JPL:  	return OPC_JMI;
       	case OPC_JVC:  	return OPC_JVS;
       	case OPC_JVS:  	return OPC_JVC;
	default:  	Internal ("GetInverseBranch: Invalid opcode: %d", OPC);
    }
}



opc_t MakeShortBranch (opc_t OPC)
/* Return the short version of the given branch. If the branch is already
 * a short branch, return the opcode unchanged.
 */
{
    switch (OPC) {
       	case OPC_BCC:
       	case OPC_JCC:  	return OPC_BCC;
       	case OPC_BCS:
       	case OPC_JCS:  	return OPC_BCS;
       	case OPC_BEQ:
       	case OPC_JEQ:  	return OPC_BEQ;
       	case OPC_BMI:
       	case OPC_JMI:  	return OPC_BMI;
       	case OPC_BNE:
       	case OPC_JNE:  	return OPC_BNE;
       	case OPC_BPL:
       	case OPC_JPL:  	return OPC_BPL;
       	case OPC_BVC:
       	case OPC_JVC:  	return OPC_BVC;
       	case OPC_BVS:
       	case OPC_JVS:  	return OPC_BVS;
       	case OPC_BRA:
	case OPC_JMP:	return (CPU == CPU_65C02)? OPC_BRA : OPC_JMP;
       	default:       	Internal ("GetShortBranch: Invalid opcode: %d", OPC);
    }
}



opc_t MakeLongBranch (opc_t OPC)
/* Return the long version of the given branch. If the branch is already
 * a long branch, return the opcode unchanged.
 */
{
    switch (OPC) {
       	case OPC_BCC:
       	case OPC_JCC:  	return OPC_JCC;
       	case OPC_BCS:
       	case OPC_JCS:  	return OPC_JCS;
       	case OPC_BEQ:
       	case OPC_JEQ:  	return OPC_JEQ;
       	case OPC_BMI:
       	case OPC_JMI:  	return OPC_JMI;
       	case OPC_BNE:
       	case OPC_JNE:  	return OPC_JNE;
       	case OPC_BPL:
       	case OPC_JPL:  	return OPC_JPL;
       	case OPC_BVC:
       	case OPC_JVC:  	return OPC_JVC;
       	case OPC_BVS:
       	case OPC_JVS:  	return OPC_JVS;
	case OPC_BRA:
	case OPC_JMP:	return OPC_JMP;
       	default:       	Internal ("GetShortBranch: Invalid opcode: %d", OPC);
    }
}



bc_t GetBranchCond (opc_t OPC)
/* Get the condition for the conditional branch in OPC */
{
    switch (OPC) {
       	case OPC_BCC:  	return BC_CC;
       	case OPC_BCS:  	return BC_CS;
       	case OPC_BEQ:  	return BC_EQ;
       	case OPC_BMI:  	return BC_MI;
       	case OPC_BNE:  	return BC_NE;
       	case OPC_BPL:  	return BC_PL;
       	case OPC_BVC:  	return BC_VC;
       	case OPC_BVS:  	return BC_VS;
       	case OPC_JCC:  	return BC_CC;
       	case OPC_JCS:  	return BC_CS;
       	case OPC_JEQ:  	return BC_EQ;
       	case OPC_JMI:  	return BC_MI;
       	case OPC_JNE:  	return BC_NE;
       	case OPC_JPL:  	return BC_PL;
       	case OPC_JVC:  	return BC_VC;
       	case OPC_JVS:  	return BC_VS;
	default:  	Internal ("GetBranchCond: Invalid opcode: %d", OPC);
    }
}



bc_t GetInverseCond (bc_t BC)
/* Return the inverse condition of the given one */
{
    switch (BC) {
       	case BC_CC:  	return BC_CS;
       	case BC_CS:  	return BC_CC;
       	case BC_EQ:  	return BC_NE;
       	case BC_MI:  	return BC_PL;
       	case BC_NE:  	return BC_EQ;
       	case BC_PL:  	return BC_MI;
       	case BC_VC:  	return BC_VS;
       	case BC_VS:  	return BC_VC;
	default:  	Internal ("GetInverseCond: Invalid condition: %d", BC);
    }
}



opc_t GetLongBranch (bc_t BC)
/* Return a long branch for the given branch condition */
{
    switch (BC) {
	case BC_CC:	return OPC_JCC;
	case BC_CS:	return OPC_JCS;
	case BC_EQ:	return OPC_JEQ;
	case BC_MI:	return OPC_JMI;
	case BC_NE:	return OPC_JNE;
	case BC_PL:	return OPC_JPL;
	case BC_VC:	return OPC_JVC;
	case BC_VS:	return OPC_JVS;
       	default:       	Internal ("GetLongBranch: Invalid condition: %d", BC);
    }
}



opc_t GetShortBranch (bc_t BC)
/* Return a short branch for the given branch condition */
{
    switch (BC) {
       	case BC_CC:    	return OPC_BCC;
       	case BC_CS:    	return OPC_BCS;
       	case BC_EQ:    	return OPC_BEQ;
       	case BC_MI:    	return OPC_BMI;
       	case BC_NE:    	return OPC_BNE;
       	case BC_PL:    	return OPC_BPL;
       	case BC_VC:    	return OPC_BVC;
       	case BC_VS:    	return OPC_BVS;
       	default:       	Internal ("GetShortBranch: Invalid condition: %d", BC);
    }
}



