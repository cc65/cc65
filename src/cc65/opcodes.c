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
const OPCDesc OPCTable[OPCODE_COUNT] = {

    /* 65XX opcodes */
    { OP65_ADC, "adc", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_AND, "and", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_ASL, "asl", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_BCC, "bcc", 2, REG_NONE, REG_NONE, OF_CBRA			    },
    { OP65_BCS, "bcs", 2, REG_NONE, REG_NONE, OF_CBRA			    },
    { OP65_BEQ, "beq", 2, REG_NONE, REG_NONE, OF_CBRA | OF_ZBRA | OF_FBRA    },
    { OP65_BIT, "bit", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OP65_BMI, "bmi", 2, REG_NONE, REG_NONE, OF_CBRA | OF_FBRA 	  	    },
    { OP65_BNE, "bne", 2, REG_NONE, REG_NONE, OF_CBRA | OF_ZBRA | OF_FBRA    },
    { OP65_BPL, "bpl", 2, REG_NONE, REG_NONE, OF_CBRA | OF_FBRA 	  	    },
    { OP65_BRA, "bra", 2, REG_NONE, REG_NONE, OF_UBRA			    },
    { OP65_BRK, "brk", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_BVC, "bvc", 2, REG_NONE, REG_NONE, OF_CBRA			    },
    { OP65_BVS, "bvs", 2, REG_NONE, REG_NONE, OF_CBRA			    },
    { OP65_CLC, "clc", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_CLD, "cld", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_CLI, "cli", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_CLV, "clv", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_CMP, "cmp", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OP65_CPX, "cpx", 0, REG_X,    REG_NONE, OF_NONE			    },
    { OP65_CPY, "cpy", 0, REG_Y,    REG_NONE, OF_NONE			    },
    { OP65_DEA, "dea", 1, REG_A,    REG_A,    OF_NONE			    },
    { OP65_DEC, "dec", 0, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_DEX, "dex", 1, REG_X,    REG_X,    OF_NONE			    },
    { OP65_DEY, "dey", 1, REG_Y,    REG_Y,    OF_NONE			    },
    { OP65_EOR, "eor", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_INA, "ina", 1, REG_A,    REG_A,    OF_NONE			    },
    { OP65_INC, "inc", 0, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_INX, "inx", 1, REG_X,    REG_X,    OF_NONE			    },
    { OP65_INY, "iny", 1, REG_Y,    REG_Y,    OF_NONE			    },
    { OP65_JCC, "jcc", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA  	    },
    { OP65_JCS, "jcs", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA  	    },
    { OP65_JEQ, "jeq", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA | OF_ZBRA | OF_FBRA },
    { OP65_JMI, "jmi", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA | OF_FBRA    },
    { OP65_JMP, "jmp", 3, REG_NONE, REG_NONE, OF_UBRA | OF_LBRA  	    },
    { OP65_JNE, "jne", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA | OF_ZBRA | OF_FBRA },
    { OP65_JPL, "jpl", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA | OF_FBRA    },
    { OP65_JSR, "jsr", 3, REG_NONE, REG_NONE, OF_CALL			    },
    { OP65_JVC, "jvc", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA  	    },
    { OP65_JVS, "jvs", 5, REG_NONE, REG_NONE, OF_CBRA | OF_LBRA  	    },
    { OP65_LDA, "lda", 0, REG_NONE, REG_A,    OF_LOAD   			    },
    { OP65_LDX, "ldx", 0, REG_NONE, REG_X,    OF_LOAD			    },
    { OP65_LDY, "ldy", 0, REG_NONE, REG_Y,    OF_LOAD			    },
    { OP65_LSR, "lsr", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_NOP, "nop", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_ORA, "ora", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_PHA, "pha", 1, REG_A,    REG_NONE, OF_NONE			    },
    { OP65_PHP, "php", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_PHX, "phx", 1, REG_X,    REG_NONE, OF_NONE			    },
    { OP65_PHY, "phy", 1, REG_Y,    REG_NONE, OF_NONE			    },
    { OP65_PLA, "pla", 1, REG_NONE, REG_A,    OF_NONE			    },
    { OP65_PLP, "plp", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_PLX, "plx", 1, REG_NONE, REG_X,    OF_NONE			    },
    { OP65_PLY, "ply", 1, REG_NONE, REG_Y,    OF_NONE			    },
    { OP65_ROL, "rol", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_ROR, "ror", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_RTI, "rti", 1, REG_NONE, REG_NONE, OF_RET   			    },
    { OP65_RTS, "rts", 1, REG_NONE, REG_NONE, OF_RET   			    },
    { OP65_SBC, "sbc", 0, REG_A,    REG_A,    OF_NONE			    },
    { OP65_SEC, "sec", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_SED, "sed", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_SEI, "sei", 1, REG_NONE, REG_NONE, OF_NONE			    },
    { OP65_STA, "sta", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OP65_STX, "stx", 0, REG_X,    REG_NONE, OF_NONE			    },
    { OP65_STY, "sty", 0, REG_Y,    REG_NONE, OF_NONE			    },
    { OP65_TAX, "tax", 1, REG_A,    REG_X,    OF_XFR			    },
    { OP65_TAY, "tay", 1, REG_A,    REG_Y,    OF_XFR			    },
    { OP65_TRB, "trb", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OP65_TSB, "tsb", 0, REG_A,    REG_NONE, OF_NONE			    },
    { OP65_TSX, "tsx", 1, REG_NONE, REG_X,    OF_XFR			    },
    { OP65_TXA, "txa", 1, REG_X,    REG_A,    OF_XFR			    },
    { OP65_TXS, "txs", 1, REG_X,    REG_NONE, OF_XFR			    },
    { OP65_TYA, "tya", 1, REG_A,    REG_A,    OF_XFR			    },
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
 * found, return OP65_INVALID.
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
    return bsearch (Mnemo, OPCTable, OPCODE_COUNT, sizeof (OPCTable[0]), Compare);
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
 	case AM65_IMP: 	   return 1;
 	case AM65_ACC:	   return 1;
 	case AM65_IMM:	   return 2;
 	case AM65_ZP: 	   return 2;
 	case AM65_ZPX:	   return 2;
 	case AM65_ABS:	   return 3;
 	case AM65_ABSX:	   return 3;
 	case AM65_ABSY:	   return 3;
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
 * index registers return REG_r info for these registers).
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
       	default:       	   return REG_NONE;
    }
}



opc_t GetInverseBranch (opc_t OPC)
/* Return a branch that reverse the condition of the branch given in OPC */
{
    switch (OPC) {
	case OP65_BCC:	return OP65_BCS;
	case OP65_BCS:	return OP65_BCC;
	case OP65_BEQ:	return OP65_BNE;
	case OP65_BMI:	return OP65_BPL;
	case OP65_BNE:	return OP65_BEQ;
	case OP65_BPL:	return OP65_BMI;
	case OP65_BVC:  	return OP65_BVS;
	case OP65_BVS:	return OP65_BVC;
       	case OP65_JCC:  	return OP65_JCS;
       	case OP65_JCS:  	return OP65_JCC;
       	case OP65_JEQ:  	return OP65_JNE;
       	case OP65_JMI:  	return OP65_JPL;
       	case OP65_JNE:  	return OP65_JEQ;
       	case OP65_JPL:  	return OP65_JMI;
       	case OP65_JVC:  	return OP65_JVS;
       	case OP65_JVS:  	return OP65_JVC;
	default:
	    Internal ("GetInverseBranch: Invalid opcode: %d", OPC);
	    return 0;
    }
}



opc_t MakeShortBranch (opc_t OPC)
/* Return the short version of the given branch. If the branch is already
 * a short branch, return the opcode unchanged.
 */
{
    switch (OPC) {
       	case OP65_BCC:
       	case OP65_JCC:  	return OP65_BCC;
       	case OP65_BCS:
       	case OP65_JCS:  	return OP65_BCS;
       	case OP65_BEQ:
       	case OP65_JEQ:  	return OP65_BEQ;
       	case OP65_BMI:
       	case OP65_JMI:  	return OP65_BMI;
       	case OP65_BNE:
       	case OP65_JNE:  	return OP65_BNE;
       	case OP65_BPL:
       	case OP65_JPL:  	return OP65_BPL;
       	case OP65_BVC:
       	case OP65_JVC:  	return OP65_BVC;
       	case OP65_BVS:
       	case OP65_JVS:  	return OP65_BVS;
       	case OP65_BRA:
	case OP65_JMP:	return (CPU == CPU_65C02)? OP65_BRA : OP65_JMP;
       	default:
	    Internal ("MakeShortBranch: Invalid opcode: %d", OPC);
	    return 0;
    }
}



opc_t MakeLongBranch (opc_t OPC)
/* Return the long version of the given branch. If the branch is already
 * a long branch, return the opcode unchanged.
 */
{
    switch (OPC) {
       	case OP65_BCC:
       	case OP65_JCC:  	return OP65_JCC;
       	case OP65_BCS:
       	case OP65_JCS:  	return OP65_JCS;
       	case OP65_BEQ:
       	case OP65_JEQ:  	return OP65_JEQ;
       	case OP65_BMI:
       	case OP65_JMI:  	return OP65_JMI;
       	case OP65_BNE:
       	case OP65_JNE:  	return OP65_JNE;
       	case OP65_BPL:
       	case OP65_JPL:  	return OP65_JPL;
       	case OP65_BVC:
       	case OP65_JVC:  	return OP65_JVC;
       	case OP65_BVS:
       	case OP65_JVS:  	return OP65_JVS;
	case OP65_BRA:
	case OP65_JMP:	return OP65_JMP;
       	default:
	    Internal ("MakeLongBranch: Invalid opcode: %d", OPC);
	    return 0;
    }
}



bc_t GetBranchCond (opc_t OPC)
/* Get the condition for the conditional branch in OPC */
{
    switch (OPC) {
       	case OP65_BCC:  	return BC_CC;
       	case OP65_BCS:  	return BC_CS;
       	case OP65_BEQ:  	return BC_EQ;
       	case OP65_BMI:  	return BC_MI;
       	case OP65_BNE:  	return BC_NE;
       	case OP65_BPL:  	return BC_PL;
       	case OP65_BVC:  	return BC_VC;
       	case OP65_BVS:  	return BC_VS;
       	case OP65_JCC:  	return BC_CC;
       	case OP65_JCS:  	return BC_CS;
       	case OP65_JEQ:  	return BC_EQ;
       	case OP65_JMI:  	return BC_MI;
       	case OP65_JNE:  	return BC_NE;
       	case OP65_JPL:  	return BC_PL;
       	case OP65_JVC:  	return BC_VC;
       	case OP65_JVS:  	return BC_VS;
	default:
	    Internal ("GetBranchCond: Invalid opcode: %d", OPC);
	    return 0;
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
	default:
	    Internal ("GetInverseCond: Invalid condition: %d", BC);
	    return 0;
    }
}



