/*****************************************************************************/
/*                                                                           */
/*				   opcodes.c				     */
/*                                                                           */
/*		    Opcode and addressing mode definitions		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
#include "opcodes.h"



/*****************************************************************************/
/*  	       	     	  	     Data				     */
/*****************************************************************************/



/* Mapper table, mnemonic --> opcode */
static const OPCDesc OPCTable[OPC_COUNT] = {
    { "adc", OPC_ADC, CI_USE_A | CI_CHG_A 	},
    { "and", OPC_AND, CI_USE_A | CI_CHG_A   	},
    { "asl", OPC_ASL, CI_USE_A | CI_CHG_A 	},
    { "bcc", OPC_BCC, CI_CHG_NONE	  	},
    { "bcs", OPC_BCS, CI_CHG_NONE	  	},
    { "beq", OPC_BEQ, CI_CHG_NONE	  	},
    { "bit", OPC_BIT, CI_USE_A 			},
    { "bmi", OPC_BMI, CI_CHG_NONE	   	},
    { "bne", OPC_BNE, CI_CHG_NONE	  	},
    { "bpl", OPC_BPL, CI_CHG_NONE	  	},
    { "bra", OPC_BRA, CI_CHG_NONE	  	},
    { "brk", OPC_BRK, CI_CHG_NONE	  	},
    { "bvc", OPC_BVC, CI_CHG_NONE	  	},
    { "bvs", OPC_BVS, CI_CHG_NONE	  	},
    { "clc", OPC_CLC, CI_CHG_NONE	  	},
    { "cld", OPC_CLD, CI_CHG_NONE	 	},
    { "cli", OPC_CLI, CI_CHG_NONE	 	},
    { "clv", OPC_CLV, CI_CHG_NONE	 	},
    { "cmp", OPC_CMP, CI_USE_A		 	},
    { "cpx", OPC_CPX, CI_USE_X		 	},
    { "cpy", OPC_CPY, CI_USE_Y		   	},
    { "dea", OPC_DEA, CI_USE_A | CI_CHG_A   	},
    { "dec", OPC_DEC, CI_NONE		 	},
    { "dex", OPC_DEX, CI_USE_X | CI_CHG_X   	},
    { "dey", OPC_DEY, CI_USE_Y | CI_CHG_Y   	},
    { "eor", OPC_EOR, CI_USE_A | CI_CHG_A   	},
    { "ina", OPC_INA, CI_USE_A | CI_CHG_A	},
    { "inc", OPC_INC, CI_NONE		 	},
    { "inx", OPC_INX, CI_USE_X | CI_CHG_X   	},
    { "iny", OPC_INY, CI_USE_Y | CI_CHG_Y  	},
    { "jmp", OPC_JMP, CI_NONE		   	},
    { "jsr", OPC_JSR, CI_NONE		   	},
    { "lda", OPC_LDA, CI_CHG_A		   	},
    { "ldx", OPC_LDX, CI_CHG_X		   	},
    { "ldy", OPC_LDY, CI_CHG_Y		   	},
    { "lsr", OPC_LSR, CI_USE_A | CI_CHG_A	},
    { "nop", OPC_NOP, CI_NONE		   	},
    { "ora", OPC_ORA, CI_USE_A | CI_CHG_A	},
    { "pha", OPC_PHA, CI_USE_A		   	},
    { "php", OPC_PHP, CI_NONE		   	},
    { "phx", OPC_PHX, CI_USE_X		   	},
    { "phy", OPC_PHY, CI_USE_Y		   	},
    { "pla", OPC_PLA, CI_CHG_A		   	},
    { "plp", OPC_PLP, CI_NONE		   	},
    { "plx", OPC_PLX, CI_CHG_X		   	},
    { "ply", OPC_PLY, CI_CHG_Y		   	},
    { "rol", OPC_ROL, CI_USE_A | CI_CHG_A	},
    { "ror", OPC_ROR, CI_USE_A | CI_CHG_A	},
    { "rti", OPC_RTI, CI_NONE		   	},
    { "rts", OPC_RTS, CI_NONE		   	},
    { "sbc", OPC_SBC, CI_USE_A | CI_CHG_A	},
    { "sec", OPC_SEC, CI_NONE		   	},
    { "sed", OPC_SED, CI_NONE		   	},
    { "sei", OPC_SEI, CI_NONE		   	},
    { "sta", OPC_STA, CI_USE_A		   	},
    { "stx", OPC_STX, CI_USE_X		   	},
    { "sty", OPC_STY, CI_USE_Y		   	},
    { "tax", OPC_TAX, CI_USE_A | CI_CHG_X   	},
    { "tay", OPC_TAY, CI_USE_A | CI_CHG_Y   	},
    { "trb", OPC_TRB, CI_USE_A		   	},
    { "tsb", OPC_TSB, CI_USE_A		   	},
    { "tsx", OPC_TSX, CI_CHG_X		 	},
    { "txa", OPC_TXA, CI_USE_X | CI_CHG_A   	},
    { "txs", OPC_TXS, CI_USE_X		   	},
    { "tya", OPC_TYA, CI_USE_Y | CI_CHG_A   	}
};



/*****************************************************************************/
/*     	       	      	  	     Code				     */
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
    /* On the 6502 (and 65C02), the instruction size is determined only by the
     * addressing mode.
     */
    switch (AM) {
	case AM_IMP:	  return 1;
	case AM_IMM:	  return 2;
	case AM_ZP:	  return 2;
	case AM_ZPX:	  return 2;
	case AM_ABS:	  return 3;
	case AM_ABSX:	  return 3;
	case AM_ABSY:	  return 3;
	case AM_ZPX_IND:  return 2;
	case AM_ZP_INDY:  return 2;
	case AM_ZP_IND:   return 2;
     	case AM_BRA:	  return 2;
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



			  
