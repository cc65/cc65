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

/* b6502 */
#include "codeinfo.h"
#include "opcodes.h"



/*****************************************************************************/
/*  	       	     	  	     Data				     */
/*****************************************************************************/



/* Mapper table, mnemonic --> opcode */
static const OPCDesc OPCTable[OPC_COUNT] = {
    { "adc", OPC_ADC, 0, CI_USE_A | CI_CHG_A 	},
    { "and", OPC_AND, 0, CI_USE_A | CI_CHG_A   	},
    { "asl", OPC_ASL, 0, CI_USE_A | CI_CHG_A 	},
    { "bcc", OPC_BCC, 2, CI_BRA		  	},
    { "bcs", OPC_BCS, 2, CI_BRA		  	},
    { "beq", OPC_BEQ, 2, CI_BRA		  	},
    { "bit", OPC_BIT, 0, CI_USE_A      	       	},
    { "bmi", OPC_BMI, 2, CI_BRA		   	},
    { "bne", OPC_BNE, 2, CI_BRA		  	},
    { "bpl", OPC_BPL, 2, CI_BRA		  	},
    { "bra", OPC_BRA, 2, CI_BRA		  	},
    { "brk", OPC_BRK, 1, CI_NONE	  	},
    { "bvc", OPC_BVC, 2, CI_BRA		  	},
    { "bvs", OPC_BVS, 2, CI_BRA		  	},
    { "clc", OPC_CLC, 1, CI_CHG_NONE	  	},
    { "cld", OPC_CLD, 1, CI_CHG_NONE	 	},
    { "cli", OPC_CLI, 1, CI_CHG_NONE	 	},
    { "clv", OPC_CLV, 1, CI_CHG_NONE	 	},
    { "cmp", OPC_CMP, 0, CI_USE_A      	       	},
    { "cpx", OPC_CPX, 0, CI_USE_X      	       	},
    { "cpy", OPC_CPY, 0, CI_USE_Y      	       	},
    { "dea", OPC_DEA, 1, CI_USE_A | CI_CHG_A   	},
    { "dec", OPC_DEC, 0, CI_NONE       	       	},
    { "dex", OPC_DEX, 1, CI_USE_X | CI_CHG_X   	},
    { "dey", OPC_DEY, 1, CI_USE_Y | CI_CHG_Y   	},
    { "eor", OPC_EOR, 0, CI_USE_A | CI_CHG_A   	},
    { "ina", OPC_INA, 1, CI_USE_A | CI_CHG_A	},
    { "inc", OPC_INC, 0, CI_NONE       	       	},
    { "inx", OPC_INX, 1, CI_USE_X | CI_CHG_X   	},
    { "iny", OPC_INY, 1, CI_USE_Y | CI_CHG_Y  	},
    { "jcc", OPC_JCC, 5, CI_BRA		  	},
    { "jcs", OPC_JCS, 5, CI_BRA		  	},
    { "jeq", OPC_JEQ, 5, CI_BRA		  	},
    { "jmi", OPC_JMI, 5, CI_BRA		   	},
    { "jmp", OPC_JMP, 3, CI_BRA        	       	},
    { "jne", OPC_JNE, 5, CI_BRA		  	},
    { "jpl", OPC_JPL, 5, CI_BRA		  	},
    { "jsr", OPC_JSR, 3, CI_NONE		},
    { "jvc", OPC_JVC, 5, CI_BRA		  	},
    { "jvs", OPC_JVS, 5, CI_BRA		  	},
    { "lda", OPC_LDA, 0, CI_CHG_A      	       	},
    { "ldx", OPC_LDX, 0, CI_CHG_X      	       	},
    { "ldy", OPC_LDY, 0, CI_CHG_Y      	       	},
    { "lsr", OPC_LSR, 0, CI_USE_A | CI_CHG_A	},
    { "nop", OPC_NOP, 1, CI_NONE       	       	},
    { "ora", OPC_ORA, 0, CI_USE_A | CI_CHG_A	},
    { "pha", OPC_PHA, 1, CI_USE_A      	       	},
    { "php", OPC_PHP, 1, CI_NONE       	       	},
    { "phx", OPC_PHX, 1, CI_USE_X      	       	},
    { "phy", OPC_PHY, 1, CI_USE_Y      	       	},
    { "pla", OPC_PLA, 1, CI_CHG_A      	       	},
    { "plp", OPC_PLP, 1, CI_NONE       	       	},
    { "plx", OPC_PLX, 1, CI_CHG_X      	       	},
    { "ply", OPC_PLY, 1, CI_CHG_Y      	       	},
    { "rol", OPC_ROL, 0, CI_USE_A | CI_CHG_A	},
    { "ror", OPC_ROR, 0, CI_USE_A | CI_CHG_A	},
    { "rti", OPC_RTI, 1, CI_NONE       	       	},
    { "rts", OPC_RTS, 1, CI_NONE       	       	},
    { "sbc", OPC_SBC, 0, CI_USE_A | CI_CHG_A	},
    { "sec", OPC_SEC, 1, CI_NONE       	       	},
    { "sed", OPC_SED, 1, CI_NONE       	       	},
    { "sei", OPC_SEI, 1, CI_NONE       	       	},
    { "sta", OPC_STA, 0, CI_USE_A      	       	},
    { "stx", OPC_STX, 0, CI_USE_X      	       	},
    { "sty", OPC_STY, 0, CI_USE_Y      	       	},
    { "tax", OPC_TAX, 1, CI_USE_A | CI_CHG_X   	},
    { "tay", OPC_TAY, 1, CI_USE_A | CI_CHG_Y   	},
    { "trb", OPC_TRB, 0, CI_USE_A      	       	},
    { "tsb", OPC_TSB, 0, CI_USE_A      	       	},
    { "tsx", OPC_TSX, 1, CI_CHG_X      	       	},
    { "txa", OPC_TXA, 1, CI_USE_X | CI_CHG_A   	},
    { "txs", OPC_TXS, 1, CI_USE_X      	       	},
    { "tya", OPC_TYA, 1, CI_USE_Y | CI_CHG_A   	}
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



const OPCDesc* GetOPCDesc (opc_t OPC)
/* Get an opcode description */
{
    /* Check the range */
    PRECONDITION (OPC >= (opc_t)0 && OPC < OPC_COUNT);

    /* Return the description */
    return &OPCTable [OPC];
}



unsigned GetAMUseInfo (am_t AM)
/* Get usage info for the given addressing mode (addressing modes that use
 * index registers return CI_USE... info for these registers).
 */			       
{
    /* Check the addressing mode. */
    switch (AM) {
	case AM_ACC:	  return CI_USE_A;
	case AM_ZPX:	  return CI_USE_X;
	case AM_ABSX:	  return CI_USE_X;
	case AM_ABSY:	  return CI_USE_Y;
	case AM_ZPX_IND:  return CI_USE_X;
	case AM_ZP_INDY:  return CI_USE_Y;
	default:	  return CI_USE_NONE;
    }
}



