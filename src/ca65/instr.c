/*****************************************************************************/
/*                                                                           */
/*				    instr.c				     */
/*                                                                           */
/*	       Instruction encoding for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* common */
#include "assertdefs.h"
#include "bitops.h"
#include "check.h"

/* ca65 */
#include "asserts.h"
#include "ea.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "instr.h"
#include "nexttok.h"
#include "objcode.h"
#include "spool.h"
#include "symtab.h"



/*****************************************************************************/
/*     	     	    		     Data   			  	     */
/*****************************************************************************/



/* Forwards for handler functions */
static void PutPCRel8 (const InsDesc* Ins);
static void PutPCRel16 (const InsDesc* Ins);
static void PutBlockMove (const InsDesc* Ins);
static void PutREP (const InsDesc* Ins);
static void PutSEP (const InsDesc* Ins);
static void PutJmp (const InsDesc* Ins);
static void PutAll (const InsDesc* Ins);



/* Instruction table for the 6502 */
#define INS_COUNT_6502         	56
static const struct {
    unsigned Count;
    InsDesc  Ins[INS_COUNT_6502];
} InsTab6502 = {
    INS_COUNT_6502,
    {
	{ "ADC", 0x080A26C, 0x60, 0, PutAll },
	{ "AND", 0x080A26C, 0x20, 0, PutAll },
	{ "ASL", 0x000006e, 0x02, 1, PutAll },
	{ "BCC", 0x0020000, 0x90, 0, PutPCRel8 },
	{ "BCS", 0x0020000, 0xb0, 0, PutPCRel8 },
	{ "BEQ", 0x0020000, 0xf0, 0, PutPCRel8 },
	{ "BIT", 0x000000C, 0x00, 2, PutAll },
	{ "BMI", 0x0020000, 0x30, 0, PutPCRel8 },
	{ "BNE", 0x0020000, 0xd0, 0, PutPCRel8 },
	{ "BPL", 0x0020000, 0x10, 0, PutPCRel8 },
	{ "BRK", 0x0000001, 0x00, 0, PutAll },
	{ "BVC", 0x0020000, 0x50, 0, PutPCRel8 },
      	{ "BVS", 0x0020000, 0x70, 0, PutPCRel8 },
	{ "CLC", 0x0000001, 0x18, 0, PutAll },
	{ "CLD", 0x0000001, 0xd8, 0, PutAll },
	{ "CLI", 0x0000001, 0x58, 0, PutAll },
	{ "CLV", 0x0000001, 0xb8, 0, PutAll },
	{ "CMP", 0x080A26C, 0xc0, 0, PutAll },
	{ "CPX", 0x080000C, 0xe0, 1, PutAll },
	{ "CPY", 0x080000C, 0xc0, 1, PutAll },
	{ "DEC", 0x000006C, 0x00, 3, PutAll },
	{ "DEX", 0x0000001, 0xca, 0, PutAll },
	{ "DEY", 0x0000001, 0x88, 0, PutAll },
	{ "EOR", 0x080A26C, 0x40, 0, PutAll },
	{ "INC", 0x000006c, 0x00, 4, PutAll },
	{ "INX", 0x0000001, 0xe8, 0, PutAll },
	{ "INY", 0x0000001, 0xc8, 0, PutAll },
	{ "JMP", 0x0000808, 0x4c, 6, PutJmp },
	{ "JSR", 0x0000008, 0x20, 7, PutAll },
	{ "LDA", 0x080A26C, 0xa0, 0, PutAll },
	{ "LDX", 0x080030C, 0xa2, 1, PutAll },
	{ "LDY", 0x080006C, 0xa0, 1, PutAll },
	{ "LSR", 0x000006F, 0x42, 1, PutAll },
	{ "NOP", 0x0000001, 0xea, 0, PutAll },
	{ "ORA", 0x080A26C, 0x00, 0, PutAll },
	{ "PHA", 0x0000001, 0x48, 0, PutAll },
	{ "PHP", 0x0000001, 0x08, 0, PutAll },
	{ "PLA", 0x0000001, 0x68, 0, PutAll },
	{ "PLP", 0x0000001, 0x28, 0, PutAll },
	{ "ROL", 0x000006F, 0x22, 1, PutAll },
	{ "ROR", 0x000006F, 0x62, 1, PutAll },
	{ "RTI", 0x0000001, 0x40, 0, PutAll },
	{ "RTS", 0x0000001, 0x60, 0, PutAll },
	{ "SBC", 0x080A26C, 0xe0, 0, PutAll },
	{ "SEC", 0x0000001, 0x38, 0, PutAll },
	{ "SED", 0x0000001, 0xf8, 0, PutAll },
	{ "SEI", 0x0000001, 0x78, 0, PutAll },
	{ "STA", 0x000A26C, 0x80, 0, PutAll },
	{ "STX", 0x000010c, 0x82, 1, PutAll },
	{ "STY", 0x000002c, 0x80, 1, PutAll },
	{ "TAX", 0x0000001, 0xaa, 0, PutAll },
	{ "TAY", 0x0000001, 0xa8, 0, PutAll },
	{ "TSX", 0x0000001, 0xba, 0, PutAll },
	{ "TXA", 0x0000001, 0x8a, 0, PutAll },
	{ "TXS", 0x0000001, 0x9a, 0, PutAll },
      	{ "TYA", 0x0000001, 0x98, 0, PutAll }
    }
};

/* Instruction table for the 65SC02 */
#define INS_COUNT_65SC02      	66
static const struct {
    unsigned Count;
    InsDesc  Ins[INS_COUNT_65SC02];
} InsTab65SC02 = {
    INS_COUNT_65SC02,
    {
	{ "ADC", 0x080A66C, 0x60, 0, PutAll },
	{ "AND", 0x080A66C, 0x20, 0, PutAll },
	{ "ASL", 0x000006e, 0x02, 1, PutAll },
	{ "BCC", 0x0020000, 0x90, 0, PutPCRel8 },
	{ "BCS", 0x0020000, 0xb0, 0, PutPCRel8 },
	{ "BEQ", 0x0020000, 0xf0, 0, PutPCRel8 },
	{ "BIT", 0x0A0006C, 0x00, 2, PutAll },
	{ "BMI", 0x0020000, 0x30, 0, PutPCRel8 },
	{ "BNE", 0x0020000, 0xd0, 0, PutPCRel8 },
	{ "BPL", 0x0020000, 0x10, 0, PutPCRel8 },
	{ "BRA", 0x0020000, 0x80, 0, PutPCRel8 },
	{ "BRK", 0x0000001, 0x00, 0, PutAll },
	{ "BVC", 0x0020000, 0x50, 0, PutPCRel8 },
	{ "BVS", 0x0020000, 0x70, 0, PutPCRel8 },
	{ "CLC", 0x0000001, 0x18, 0, PutAll },
	{ "CLD", 0x0000001, 0xd8, 0, PutAll },
	{ "CLI", 0x0000001, 0x58, 0, PutAll },
	{ "CLV", 0x0000001, 0xb8, 0, PutAll },
	{ "CMP", 0x080A66C, 0xc0, 0, PutAll },
	{ "CPX", 0x080000C, 0xe0, 1, PutAll },
	{ "CPY", 0x080000C, 0xc0, 1, PutAll },
	{ "DEA", 0x0000001, 0x00, 3, PutAll },   /* == DEC */
	{ "DEC", 0x000006F, 0x00, 3, PutAll },
	{ "DEX", 0x0000001, 0xca, 0, PutAll },
	{ "DEY", 0x0000001, 0x88, 0, PutAll },
	{ "EOR", 0x080A66C, 0x40, 0, PutAll },
	{ "INA", 0x0000001, 0x00, 4, PutAll },   /* == INC */
	{ "INC", 0x000006f, 0x00, 4, PutAll },
	{ "INX", 0x0000001, 0xe8, 0, PutAll },
	{ "INY", 0x0000001, 0xc8, 0, PutAll },
	{ "JMP", 0x0010808, 0x4c, 6, PutAll },
      	{ "JSR", 0x0000008, 0x20, 7, PutAll },
	{ "LDA", 0x080A66C, 0xa0, 0, PutAll },
	{ "LDX", 0x080030C, 0xa2, 1, PutAll },
	{ "LDY", 0x080006C, 0xa0, 1, PutAll },
	{ "LSR", 0x000006F, 0x42, 1, PutAll },
	{ "NOP", 0x0000001, 0xea, 0, PutAll },
	{ "ORA", 0x080A66C, 0x00, 0, PutAll },
	{ "PHA", 0x0000001, 0x48, 0, PutAll },
	{ "PHP", 0x0000001, 0x08, 0, PutAll },
	{ "PHX", 0x0000001, 0xda, 0, PutAll },
	{ "PHY", 0x0000001, 0x5a, 0, PutAll },
	{ "PLA", 0x0000001, 0x68, 0, PutAll },
	{ "PLP", 0x0000001, 0x28, 0, PutAll },
	{ "PLX", 0x0000001, 0xfa, 0, PutAll },
	{ "PLY", 0x0000001, 0x7a, 0, PutAll },
	{ "ROL", 0x000006F, 0x22, 1, PutAll },
	{ "ROR", 0x000006F, 0x62, 1, PutAll },
	{ "RTI", 0x0000001, 0x40, 0, PutAll },
	{ "RTS", 0x0000001, 0x60, 0, PutAll },
	{ "SBC", 0x080A66C, 0xe0, 0, PutAll },
	{ "SEC", 0x0000001, 0x38, 0, PutAll },
	{ "SED", 0x0000001, 0xf8, 0, PutAll },
	{ "SEI", 0x0000001, 0x78, 0, PutAll },
	{ "STA", 0x000A66C, 0x80, 0, PutAll },
	{ "STX", 0x000010c, 0x82, 1, PutAll },
      	{ "STY", 0x000002c, 0x80, 1, PutAll },
      	{ "STZ", 0x000006c, 0x04, 5, PutAll },
      	{ "TAX", 0x0000001, 0xaa, 0, PutAll },
      	{ "TAY", 0x0000001, 0xa8, 0, PutAll },
      	{ "TRB", 0x000000c, 0x10, 1, PutAll },
      	{ "TSB", 0x000000c, 0x00, 1, PutAll },
      	{ "TSX", 0x0000001, 0xba, 0, PutAll },
      	{ "TXA", 0x0000001, 0x8a, 0, PutAll },
      	{ "TXS", 0x0000001, 0x9a, 0, PutAll },
      	{ "TYA", 0x0000001, 0x98, 0, PutAll }
    }
};

/* Instruction table for the 65816 */
#define INS_COUNT_65816	101
static const struct {
    unsigned Count;
    InsDesc  Ins[INS_COUNT_65816];
} InsTab65816 = {
    INS_COUNT_65816,
    {
      	{ "ADC", 0x0b8f6fc, 0x60, 0, PutAll },
      	{ "AND", 0x0b8f6fc, 0x20, 0, PutAll },
      	{ "ASL", 0x000006e, 0x02, 1, PutAll },
      	{ "BCC", 0x0020000, 0x90, 0, PutPCRel8 },
      	{ "BCS", 0x0020000, 0xb0, 0, PutPCRel8 },
      	{ "BEQ", 0x0020000, 0xf0, 0, PutPCRel8 },
      	{ "BGE", 0x0020000, 0xb0, 0, PutPCRel8 },   /* == BCS */
      	{ "BIT", 0x0a0006c, 0x00, 2, PutAll },
      	{ "BLT", 0x0020000, 0x90, 0, PutPCRel8 },   /* == BCC */
      	{ "BMI", 0x0020000, 0x30, 0, PutPCRel8 },
      	{ "BNE", 0x0020000, 0xd0, 0, PutPCRel8 },
      	{ "BPL", 0x0020000, 0x10, 0, PutPCRel8 },
      	{ "BRA", 0x0020000, 0x80, 0, PutPCRel8 },
	{ "BRK", 0x0000001, 0x00, 0, PutAll },
	{ "BRL", 0x0040000, 0x82, 0, PutPCRel16 },
	{ "BVC", 0x0020000, 0x50, 0, PutPCRel8 },
	{ "BVS", 0x0020000, 0x70, 0, PutPCRel8 },
     	{ "CLC", 0x0000001, 0x18, 0, PutAll },
	{ "CLD", 0x0000001, 0xd8, 0, PutAll },
	{ "CLI", 0x0000001, 0x58, 0, PutAll },
	{ "CLV", 0x0000001, 0xb8, 0, PutAll },
      	{ "CMP", 0x0b8f6fc, 0xc0, 0, PutAll },
	{ "COP", 0x0000004, 0x02, 6, PutAll },
	{ "CPA", 0x0b8f6fc, 0xc0, 0, PutAll },   /* == CMP */
	{ "CPX", 0x0c0000c, 0xe0, 1, PutAll },
	{ "CPY", 0x0c0000c, 0xc0, 1, PutAll },
	{ "DEA", 0x0000001, 0x00, 3, PutAll },   /* == DEC */
	{ "DEC", 0x000006F, 0x00, 3, PutAll },
	{ "DEX", 0x0000001, 0xca, 0, PutAll },
	{ "DEY", 0x0000001, 0x88, 0, PutAll },
	{ "EOR", 0x0b8f6fc, 0x40, 0, PutAll },
	{ "INA", 0x0000001, 0x00, 4, PutAll },   /* == INC */
	{ "INC", 0x000006F, 0x00, 4, PutAll },
	{ "INX", 0x0000001, 0xe8, 0, PutAll },
	{ "INY", 0x0000001, 0xc8, 0, PutAll },
	{ "JML", 0x0000810, 0x5c, 1, PutAll },
	{ "JMP", 0x0010818, 0x4c, 6, PutAll },
	{ "JSL", 0x0000010, 0x20, 7, PutAll },
	{ "JSR", 0x0010018, 0x20, 7, PutAll },
	{ "LDA", 0x0b8f6fc, 0xa0, 0, PutAll },
      	{ "LDX", 0x0c0030c, 0xa2, 1, PutAll },
	{ "LDY", 0x0c0006c, 0xa0, 1, PutAll },
	{ "LSR", 0x000006F, 0x42, 1, PutAll },
	{ "MVN", 0x1000000, 0x54, 0, PutBlockMove },
	{ "MVP", 0x1000000, 0x44, 0, PutBlockMove },
	{ "NOP", 0x0000001, 0xea, 0, PutAll },
	{ "ORA", 0x0b8f6fc, 0x00, 0, PutAll },
	{ "PEA", 0x0000008, 0xf4, 6, PutAll },
	{ "PEI", 0x0800000, 0xd4, 0, PutAll },
	{ "PER", 0x0040000, 0x62, 0, PutPCRel16 },
	{ "PHA", 0x0000001, 0x48, 0, PutAll },
	{ "PHB", 0x0000001, 0x8b, 0, PutAll },
	{ "PHD", 0x0000001, 0x0b, 0, PutAll },
	{ "PHK", 0x0000001, 0x4b, 0, PutAll },
	{ "PHP", 0x0000001, 0x08, 0, PutAll },
	{ "PHX", 0x0000001, 0xda, 0, PutAll },
	{ "PHY", 0x0000001, 0x5a, 0, PutAll },
	{ "PLA", 0x0000001, 0x68, 0, PutAll },
	{ "PLB", 0x0000001, 0xab, 0, PutAll },
     	{ "PLD", 0x0000001, 0x2b, 0, PutAll },
	{ "PLP", 0x0000001, 0x28, 0, PutAll },
	{ "PLX", 0x0000001, 0xfa, 0, PutAll },
	{ "PLY", 0x0000001, 0x7a, 0, PutAll },
      	{ "REP", 0x0800000, 0xc2, 1, PutREP },
	{ "ROL", 0x000006F, 0x22, 1, PutAll },
	{ "ROR", 0x000006F, 0x62, 1, PutAll },
	{ "RTI", 0x0000001, 0x40, 0, PutAll },
	{ "RTL", 0x0000001, 0x6b, 0, PutAll },
	{ "RTS", 0x0000001, 0x60, 0, PutAll },
	{ "SBC", 0x0b8f6fc, 0xe0, 0, PutAll },
	{ "SEC", 0x0000001, 0x38, 0, PutAll },
	{ "SED", 0x0000001, 0xf8, 0, PutAll },
	{ "SEI", 0x0000001, 0x78, 0, PutAll },
	{ "SEP", 0x0800000, 0xe2, 1, PutSEP },
	{ "STA", 0x018f6fc, 0x80, 0, PutAll },
	{ "STP", 0x0000001, 0xdb, 0, PutAll },
	{ "STX", 0x000010c, 0x82, 1, PutAll },
	{ "STY", 0x000002c, 0x80, 1, PutAll },
	{ "STZ", 0x000006c, 0x04, 5, PutAll },
	{ "SWA", 0x0000001, 0xeb, 0, PutAll },   /* == XBA */
	{ "TAD", 0x0000001, 0x5b, 0, PutAll },   /* == TCD */
	{ "TAS", 0x0000001, 0x1b, 0, PutAll },   /* == TCS */
	{ "TAX", 0x0000001, 0xaa, 0, PutAll },
      	{ "TAY", 0x0000001, 0xa8, 0, PutAll },
	{ "TCD", 0x0000001, 0x5b, 0, PutAll },
	{ "TCS", 0x0000001, 0x1b, 0, PutAll },
	{ "TDA", 0x0000001, 0x7b, 0, PutAll },   /* == TDC */
	{ "TDC", 0x0000001, 0x7b, 0, PutAll },
	{ "TRB", 0x000000c, 0x10, 1, PutAll },
	{ "TSA", 0x0000001, 0x3b, 0, PutAll },   /* == TSC */
	{ "TSB", 0x000000c, 0x00, 1, PutAll },
	{ "TSC", 0x0000001, 0x3b, 0, PutAll },
	{ "TSX", 0x0000001, 0xba, 0, PutAll },
	{ "TXA", 0x0000001, 0x8a, 0, PutAll },
	{ "TXS", 0x0000001, 0x9a, 0, PutAll },
	{ "TXY", 0x0000001, 0x9b, 0, PutAll },
	{ "TYA", 0x0000001, 0x98, 0, PutAll },
	{ "TYX", 0x0000001, 0xbb, 0, PutAll },
	{ "WAI", 0x0000001, 0xcb, 0, PutAll },
	{ "XBA", 0x0000001, 0xeb, 0, PutAll },
	{ "XCE", 0x0000001, 0xfb, 0, PutAll }
    }
};

#ifdef SUNPLUS
/* Table for the SUNPLUS CPU */
#include "sunplus.inc"
#endif



/* An array with instruction tables */
static const InsTable* InsTabs[CPU_COUNT] = {
    (const InsTable*) &InsTab6502,
    (const InsTable*) &InsTab65SC02,
    (const InsTable*) &InsTab65816,
#ifdef SUNPLUS
    (const InsTable*) &InsTabSunPlus,
#else
    NULL,
#endif
};
const InsTable* InsTab = (const InsTable*) &InsTab6502;

/* Table to build the effective opcode from a base opcode and an addressing
 * mode.
 */
unsigned char EATab [9][AMI_COUNT] = {
    {   /* Table 0 */
     	0x00, 0x00, 0x05, 0x0D, 0x0F, 0x15, 0x1D, 0x1F,
     	0x00, 0x19, 0x12, 0x00, 0x07, 0x11, 0x17, 0x01,
     	0x00, 0x00, 0x00, 0x03, 0x13, 0x09, 0x00, 0x09,
     	0x00
    },
    {   /* Table 1 */
     	0x08, 0x08, 0x04, 0x0C, 0x00, 0x14, 0x1C, 0x00,
     	0x14, 0x1C, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00
    },
    {   /* Table 2 */
     	0x00, 0x00, 0x24, 0x2C, 0x0F, 0x34, 0x3C, 0x00,
      	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00,
     	0x00
    },
    {   /* Table 3 */
     	0x3A, 0x3A, 0xC6, 0xCE, 0x00, 0xD6, 0xDE, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00
    },
    {   /* Table 4 */
     	0x1A, 0x1A, 0xE6, 0xEE, 0x00, 0xF6, 0xFE, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00
    },
    {   /* Table 5 */
     	0x00, 0x00, 0x60, 0x98, 0x00, 0x70, 0x9E, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00
    },
    {   /* Table 6 */
     	0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
     	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
     	0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00
    },
    {   /* Table 7 */
     	0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     	0x00
    },
    {   /* Table 8 */
       	0x00, 0x40, 0x01, 0x41, 0x00, 0x09, 0x49, 0x00,
     	0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00,
     	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
       	0x00
    },
};

/* Table that encodes the additional bytes for each instruction */
unsigned char ExtBytes [AMI_COUNT] = {
    0,		/* Implicit */
    0,		/* Accu */
    1,		/* Direct */
    2,		/* Absolute */
    3, 		/* Absolute long */
    1,		/* Direct,X */
    2,		/* Absolute,X */
    3,		/* Absolute long,X */
    1,		/* Direct,Y */
    2,		/* Absolute,Y */
    1,		/* (Direct) */
    2,		/* (Absolute) */
    1,		/* [Direct] */
    1,		/* (Direct),Y */
    1,		/* [Direct],Y */
    1,		/* (Direct,X) */
    2,		/* (Absolute,X) */
    1,		/* Relative short */
    2,		/* Relative long */
    1,		/* r,s */
    1,		/* (r,s),y */
    1,		/* Immidiate accu */
    1,		/* Immidiate index */
    1,	     	/* Immidiate byte */
    2 		/* Blockmove */
};



/*****************************************************************************/
/*    	       	 	       Handler functions		    	     */
/*****************************************************************************/



static int EvalEA (const InsDesc* Ins, EffAddr* A)
/* Evaluate the effective address. All fields in A will be valid after calling
 * this function. The function returns true on success and false on errors.
 */
{
    /* Get the set of possible addressing modes */
    GetEA (A);

    /* From the possible addressing modes, remove the ones that are invalid
     * for this instruction or CPU.
     */
    A->AddrModeSet &= Ins->AddrMode;

    /* If we have possible zero page addressing modes, and the expression
     * involved (if any) is not in byte range, remove the zero page addressing
     * modes.
     */
    if (A->Expr && (A->AddrModeSet & AM_ZP) && !IsByteExpr (A->Expr)) {
       	A->AddrModeSet &= ~AM_ZP;
    }

    /* Check if we have any adressing modes left */
    if (A->AddrModeSet == 0) {
       	Error (ERR_ILLEGAL_ADDR_MODE);
       	return 0;
    }
    A->AddrMode    = BitFind (A->AddrModeSet);
    A->AddrModeBit = (0x01UL << A->AddrMode);

    /* If the instruction has a one byte operand and immediate addressing is
     * allowed but not used, check for an operand expression in the form
     * <label or >label, where label is a far or absolute label. If found,
     * emit a warning. This warning protects against a typo, where the '#'
     * for the immediate operand is omitted.
     */
    if (A->Expr && (Ins->AddrMode & AM_IMM)                &&
        (A->AddrModeSet & (AM_DIR | AM_ABS | AM_ABS_LONG)) &&
        ExtBytes[A->AddrMode] == 1) {

        /* Found, check the expression */
        ExprNode* Left = A->Expr->Left;
        if ((A->Expr->Op == EXPR_BYTE0 || A->Expr->Op == EXPR_BYTE1) &&
            Left->Op == EXPR_SYMBOL                                &&
            !SymIsZP (Left->V.Sym)) {

            /* Output a warning */
            Warning (WARN_SUSPICIOUS_ADDREXPR);
        }
    }

    /* Build the opcode */
    A->Opcode = Ins->BaseCode | EATab[Ins->ExtCode][A->AddrMode];

    /* Success */
    return 1;
}



static void EmitCode (EffAddr* A)
/* Output code for the data in A */
{
    /* Check how many extension bytes are needed and output the instruction */
    switch (ExtBytes[A->AddrMode]) {

        case 0:
      	    Emit0 (A->Opcode);
      	    break;

      	case 1:
      	    Emit1 (A->Opcode, A->Expr);
      	    break;

      	case 2:
	    if (CPU == CPU_65816 && (A->AddrModeBit & (AM_ABS | AM_ABS_X | AM_ABS_Y))) {
	      	/* This is a 16 bit mode that uses an address. If in 65816,
	      	 * mode, force this address into 16 bit range to allow
	      	 * addressing inside a 64K segment.
	      	 */
       	      	Emit2 (A->Opcode, ForceWordExpr (A->Expr));
	    } else {
	      	Emit2 (A->Opcode, A->Expr);
	    }
	    break;

	case 3:
	    if (A->Bank) {
	      	/* Separate bank given */
	       	Emit3b (A->Opcode, A->Expr, A->Bank);
	    } else {
	      	/* One far argument */
	      	Emit3 (A->Opcode, A->Expr);
	    }
	    break;

	default:
	    Internal ("Invalid operand byte count: %u", ExtBytes[A->AddrMode]);

    }
}



static long PutImmed8 (const InsDesc* Ins)
/* Parse and emit an immediate 8 bit instruction. Return the value of the
 * operand if it's available and const.
 */
{
    EffAddr A;
    long Val = -1;

    /* Evaluate the addressing mode */
    if (EvalEA (Ins, &A) == 0) {
        /* An error occurred */
        return -1L;
    }

    /* If we have an expression and it's const, get it's value */
    if (A.Expr && IsConstExpr (A.Expr)) {
      	Val = GetExprVal (A.Expr);
    }

    /* Check how many extension bytes are needed and output the instruction */
    switch (ExtBytes[A.AddrMode]) {

      	case 1:
      	    Emit1 (A.Opcode, A.Expr);
     	    break;

     	default:
     	    Internal ("Invalid operand byte count: %u", ExtBytes[A.AddrMode]);
    }

    /* Return the expression value */
    return Val;
}



static void PutPCRel8 (const InsDesc* Ins)
/* Handle branches with a 8 bit distance */
{
    EmitPCRel (Ins->BaseCode, BranchExpr (2), 1);
}



static void PutPCRel16 (const InsDesc* Ins)
/* Handle branches with an 16 bit distance and PER */
{
    EmitPCRel (Ins->BaseCode, BranchExpr (3), 2);
}



static void PutBlockMove (const InsDesc* Ins)
/* Handle the blockmove instructions */
{
    Emit0 (Ins->BaseCode);
    EmitByte (Expression ());
    ConsumeComma ();
    EmitByte (Expression ());
}



static void PutREP (const InsDesc* Ins)
/* Emit a REP instruction, track register sizes */
{
    /* Use the generic handler */
    long Val = PutImmed8 (Ins);

    /* We track the status only for the 816 CPU and in smart mode */
    if (CPU == CPU_65816 && SmartMode) {

	/* Check the range for Val. */
	if (Val < 0) {
	    /* We had an error */
	    Warning (WARN_CANNOT_TRACK_STATUS);
	} else {
	    if (Val & 0x10) {
	       	/* Index registers to 16 bit */
	       	ExtBytes[AMI_IMM_INDEX] = 2;
     	    }
     	    if (Val & 0x20) {
     	       	/* Accu to 16 bit */
	       	ExtBytes[AMI_IMM_ACCU] = 2;
     	    }
     	}
    }
}



static void PutSEP (const InsDesc* Ins)
/* Emit a SEP instruction, track register sizes */
{
    /* Use the generic handler */
    long Val = PutImmed8 (Ins);

    /* We track the status only for the 816 CPU and in smart mode */
    if (CPU == CPU_65816 && SmartMode) {

	/* Check the range for Val. */
	if (Val < 0) {
	    /* We had an error */
     	    Warning (WARN_CANNOT_TRACK_STATUS);
     	} else {
     	    if (Val & 0x10) {
     	   	/* Index registers to 8 bit */
     	   	ExtBytes [AMI_IMM_INDEX] = 1;
     	    }
     	    if (Val & 0x20) {
     	   	/* Accu to 8 bit */
     	   	ExtBytes [AMI_IMM_ACCU] = 1;
     	    }
     	}
    }
}



static void PutJmp (const InsDesc* Ins)
/* Handle the jump instruction for the 6502. Problem is that these chips have
 * a bug: If the address crosses a page, the upper byte gets not corrected and
 * the instruction will fail. The PutJmp function will add a linker assertion
 * to check for this case and is otherwise identical to PutAll.
 */
{
    EffAddr A;

    /* Evaluate the addressing mode used */
    if (EvalEA (Ins, &A)) {

        /* Check for indirect addressing */
        if (A.AddrModeBit & AM_ABS_IND) {

            /* Compare the low byte of the expression to 0xFF to check for
             * a page cross. Be sure to use a copy of the expression otherwise
             * things will go weird later.
             */
            ExprNode* E = CompareExpr (ForceByteExpr (CloneExpr (A.Expr)), 0xFF);

            /* Generate the message */
            unsigned Msg = GetStringId ("\"jmp (abs)\" across page border");

            /* Generate the assertion */
            AddAssertion (E, ASSERT_ACT_WARN, Msg);
        }

        /* No error, output code */
        EmitCode (&A);
    }
}



static void PutAll (const InsDesc* Ins)
/* Handle all other instructions */
{
    EffAddr A;

    /* Evaluate the addressing mode used */
    if (EvalEA (Ins, &A)) {
        /* No error, output code */
        EmitCode (&A);
    }
}



/*****************************************************************************/
/*     	     	    	   	     Code   				     */
/*****************************************************************************/



static int CmpName (const void* Key, const void* Instr)
/* Compare function for bsearch */
{
    return strcmp ((const char*)Key, ((const InsDesc*) Instr)->Mnemonic);
}



void SetCPU (cpu_t NewCPU)
/* Set a new CPU */
{
    /* Make sure the parameter is correct */
    CHECK (NewCPU < CPU_COUNT);

    /* Check if we have support for the new CPU, if so, use it */
    if (InsTabs[NewCPU]) {
    	CPU = NewCPU;
    	InsTab = InsTabs[CPU];
    } else {
     	Error (ERR_CPU_NOT_SUPPORTED);
    }
}



cpu_t GetCPU (void)
/* Return the current CPU */
{
    return CPU;
}



int FindInstruction (const char* Ident)
/* Check if Ident is a valid mnemonic. If so, return the index in the
 * instruction table. If not, return -1.
 */
{
    const InsDesc* I;
    char Key [sizeof (I->Mnemonic)];

    /* Accept only strings with the right length */
    if (strlen (Ident) != sizeof (I->Mnemonic)-1) {
       	/* Wrong length */
       	return -1;
    }

    /* Make a copy, and uppercase that copy */
    Key [0] = toupper (Ident [0]);
    Key [1] = toupper (Ident [1]);
    Key [2] = toupper (Ident [2]);
    Key [3] = '\0';

    /* Search for the key */
    I = bsearch (Key, InsTab->Ins, InsTab->Count, sizeof (InsDesc), CmpName);
    if (I == 0) {
       	/* Not found */
       	return -1;
    } else {
       	/* Found, return the entry */
       	return I - InsTab->Ins;
    }
}



void HandleInstruction (unsigned Index)
/* Handle the mnemonic with the given index */
{
    /* Safety check */
    PRECONDITION (Index < InsTab->Count);

    /* Skip the mnemonic token */
    NextTok ();

    /* Call the handler */
    InsTab->Ins[Index].Emit (&InsTab->Ins[Index]);
}



