/*****************************************************************************/
/*                                                                           */
/*                                  instr.c                                  */
/*                                                                           */
/*             Instruction encoding for the ca65 macroassembler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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
#include <string.h>
#include <ctype.h>

/* common */
#include "addrsize.h"
#include "attrib.h"
#include "bitops.h"
#include "check.h"
#include "mmodel.h"

/* ca65 */
#include "asserts.h"
#include "ea.h"
#include "ea65.h"
#include "easw16.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "instr.h"
#include "nexttok.h"
#include "objcode.h"
#include "spool.h"
#include "studyexpr.h"
#include "symtab.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void PutPCRel8 (const InsDesc* Ins);
/* Handle branches with a 8 bit distance */

static void PutPCRel16 (const InsDesc* Ins);
/* Handle branches with an 16 bit distance and PER */

static void PutPCRel4510 (const InsDesc* Ins);
/* Handle branches with a 16 bit distance for 4510 */

static void PutBlockMove (const InsDesc* Ins);
/* Handle the blockmove instructions (65816) */

static void PutBlockTransfer (const InsDesc* Ins);
/* Handle the block transfer instructions (HuC6280) */

static void PutBitBranch (const InsDesc* Ins);
/* Handle 65C02 branch on bit condition */

static void PutREP (const InsDesc* Ins);
/* Emit a REP instruction, track register sizes */

static void PutSEP (const InsDesc* Ins);
/* Emit a SEP instruction (65816), track register sizes */

static void PutTAMn (const InsDesc* Ins);
/* Emit a TAMn instruction (HuC6280). Because this is a two-byte instruction with
** implicit addressing mode, the opcode byte in the table is actually the
** second operand byte. The TAM instruction is the more generic form, it takes
** an immediate argument.
*/

static void PutTMA (const InsDesc* Ins);
/* Emit a TMA instruction (HuC6280) with an immediate argument. Only one bit
** in the argument byte may be set.
*/

static void PutTMAn (const InsDesc* Ins);
/* Emit a TMAn instruction (HuC6280). Because this is a two-byte instruction with
** implicit addressing mode, the opcode byte in the table is actually the
** second operand byte. The TMA instruction is the more generic form, it takes
** an immediate argument.
*/

static void PutTST (const InsDesc* Ins);
/* Emit a TST instruction (HuC6280). */

static void PutJMP (const InsDesc* Ins);
/* Handle the jump instruction for the 6502. Problem is that these chips have
** a bug: If the address crosses a page, the upper byte gets not corrected and
** the instruction will fail. The PutJmp function will add a linker assertion
** to check for this case and is otherwise identical to PutAll.
*/

static void PutJMP816 (const InsDesc* Ins);
/* Handle the JMP instruction for the 816.
** Allowing the long_jsr_jmp_rts feature to permit a long JMP.
** Note that JMP [abs] and JML [abs] are always both permitted for instruction $DC,
** because the [] notation for long indirection makes the generated instruction unambiguous.
*/

static void PutJSR816 (const InsDesc* Ins);
/* Handle the JSR instruction for the 816.
** Allowing the long_jsr_jmp_rts feature to permit a long JSR.
*/

static void PutRTS (const InsDesc* Ins attribute ((unused)));
/* Handle the RTS instruction for the 816. In smart mode emit a RTL opcode if
** the enclosing scope is FAR, but only if the long_jsr_jmp_rts feature applies.
*/

static void PutAll (const InsDesc* Ins);
/* Handle all other instructions */

static void Put4510 (const InsDesc* Ins);
/* Handle instructions of 4510 not matching any EATab */

static void PutSweet16 (const InsDesc* Ins);
/* Handle a generic sweet16 instruction */

static void PutSweet16Branch (const InsDesc* Ins);
/* Handle a sweet16 branch instruction */



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Empty instruction table */
static const struct {
    unsigned Count;
} InsTabNone = {
    0
};

/* Instruction table for the 6502 */
static const struct {
    unsigned Count;
    InsDesc  Ins[56];
} InsTab6502 = {
    sizeof (InsTab6502.Ins) / sizeof (InsTab6502.Ins[0]),
    {
        { "ADC",  0x080A26C, 0x60, 0, PutAll },
        { "AND",  0x080A26C, 0x20, 0, PutAll },
        { "ASL",  0x000006e, 0x02, 1, PutAll },
        { "BCC",  0x0020000, 0x90, 0, PutPCRel8 },
        { "BCS",  0x0020000, 0xb0, 0, PutPCRel8 },
        { "BEQ",  0x0020000, 0xf0, 0, PutPCRel8 },
        { "BIT",  0x000000C, 0x00, 2, PutAll },
        { "BMI",  0x0020000, 0x30, 0, PutPCRel8 },
        { "BNE",  0x0020000, 0xd0, 0, PutPCRel8 },
        { "BPL",  0x0020000, 0x10, 0, PutPCRel8 },
        { "BRK",  0x0800005, 0x00, 6, PutAll },
        { "BVC",  0x0020000, 0x50, 0, PutPCRel8 },
        { "BVS",  0x0020000, 0x70, 0, PutPCRel8 },
        { "CLC",  0x0000001, 0x18, 0, PutAll },
        { "CLD",  0x0000001, 0xd8, 0, PutAll },
        { "CLI",  0x0000001, 0x58, 0, PutAll },
        { "CLV",  0x0000001, 0xb8, 0, PutAll },
        { "CMP",  0x080A26C, 0xc0, 0, PutAll },
        { "CPX",  0x080000C, 0xe0, 1, PutAll },
        { "CPY",  0x080000C, 0xc0, 1, PutAll },
        { "DEC",  0x000006C, 0x00, 3, PutAll },
        { "DEX",  0x0000001, 0xca, 0, PutAll },
        { "DEY",  0x0000001, 0x88, 0, PutAll },
        { "EOR",  0x080A26C, 0x40, 0, PutAll },
        { "INC",  0x000006c, 0x00, 4, PutAll },
        { "INX",  0x0000001, 0xe8, 0, PutAll },
        { "INY",  0x0000001, 0xc8, 0, PutAll },
        { "JMP",  0x0000808, 0x4c, 6, PutJMP },
        { "JSR",  0x0000008, 0x20, 7, PutAll },
        { "LDA",  0x080A26C, 0xa0, 0, PutAll },
        { "LDX",  0x080030C, 0xa2, 1, PutAll },
        { "LDY",  0x080006C, 0xa0, 1, PutAll },
        { "LSR",  0x000006F, 0x42, 1, PutAll },
        { "NOP",  0x0000001, 0xea, 0, PutAll },
        { "ORA",  0x080A26C, 0x00, 0, PutAll },
        { "PHA",  0x0000001, 0x48, 0, PutAll },
        { "PHP",  0x0000001, 0x08, 0, PutAll },
        { "PLA",  0x0000001, 0x68, 0, PutAll },
        { "PLP",  0x0000001, 0x28, 0, PutAll },
        { "ROL",  0x000006F, 0x22, 1, PutAll },
        { "ROR",  0x000006F, 0x62, 1, PutAll },
        { "RTI",  0x0000001, 0x40, 0, PutAll },
        { "RTS",  0x0000001, 0x60, 0, PutAll },
        { "SBC",  0x080A26C, 0xe0, 0, PutAll },
        { "SEC",  0x0000001, 0x38, 0, PutAll },
        { "SED",  0x0000001, 0xf8, 0, PutAll },
        { "SEI",  0x0000001, 0x78, 0, PutAll },
        { "STA",  0x000A26C, 0x80, 0, PutAll },
        { "STX",  0x000010c, 0x82, 1, PutAll },
        { "STY",  0x000002c, 0x80, 1, PutAll },
        { "TAX",  0x0000001, 0xaa, 0, PutAll },
        { "TAY",  0x0000001, 0xa8, 0, PutAll },
        { "TSX",  0x0000001, 0xba, 0, PutAll },
        { "TXA",  0x0000001, 0x8a, 0, PutAll },
        { "TXS",  0x0000001, 0x9a, 0, PutAll },
        { "TYA",  0x0000001, 0x98, 0, PutAll }
    }
};

/* Instruction table for the 6502 with illegal instructions */
static const struct {
    unsigned Count;
    InsDesc  Ins[75];
} InsTab6502X = {
    sizeof (InsTab6502X.Ins) / sizeof (InsTab6502X.Ins[0]),
    {
        { "ADC",  0x080A26C, 0x60, 0, PutAll },
        { "ALR",  0x0800000, 0x4B, 0, PutAll },         /* X */
        { "ANC",  0x0800000, 0x0B, 0, PutAll },         /* X */
        { "AND",  0x080A26C, 0x20, 0, PutAll },
        { "ANE",  0x0800000, 0x8B, 0, PutAll },         /* X */
        { "ARR",  0x0800000, 0x6B, 0, PutAll },         /* X */
        { "ASL",  0x000006e, 0x02, 1, PutAll },
        { "AXS",  0x0800000, 0xCB, 0, PutAll },         /* X */
        { "BCC",  0x0020000, 0x90, 0, PutPCRel8 },
        { "BCS",  0x0020000, 0xb0, 0, PutPCRel8 },
        { "BEQ",  0x0020000, 0xf0, 0, PutPCRel8 },
        { "BIT",  0x000000C, 0x00, 2, PutAll },
        { "BMI",  0x0020000, 0x30, 0, PutPCRel8 },
        { "BNE",  0x0020000, 0xd0, 0, PutPCRel8 },
        { "BPL",  0x0020000, 0x10, 0, PutPCRel8 },
        { "BRK",  0x0800005, 0x00, 6, PutAll },
        { "BVC",  0x0020000, 0x50, 0, PutPCRel8 },
        { "BVS",  0x0020000, 0x70, 0, PutPCRel8 },
        { "CLC",  0x0000001, 0x18, 0, PutAll },
        { "CLD",  0x0000001, 0xd8, 0, PutAll },
        { "CLI",  0x0000001, 0x58, 0, PutAll },
        { "CLV",  0x0000001, 0xb8, 0, PutAll },
        { "CMP",  0x080A26C, 0xc0, 0, PutAll },
        { "CPX",  0x080000C, 0xe0, 1, PutAll },
        { "CPY",  0x080000C, 0xc0, 1, PutAll },
        { "DCP",  0x000A26C, 0xC3, 0, PutAll },         /* X */
        { "DEC",  0x000006C, 0x00, 3, PutAll },
        { "DEX",  0x0000001, 0xca, 0, PutAll },
        { "DEY",  0x0000001, 0x88, 0, PutAll },
        { "EOR",  0x080A26C, 0x40, 0, PutAll },
        { "INC",  0x000006c, 0x00, 4, PutAll },
        { "INX",  0x0000001, 0xe8, 0, PutAll },
        { "INY",  0x0000001, 0xc8, 0, PutAll },
        { "ISC",  0x000A26C, 0xE3, 0, PutAll },         /* X */
        { "JAM",  0x0000001, 0x02, 0, PutAll },         /* X */
        { "JMP",  0x0000808, 0x4c, 6, PutJMP },
        { "JSR",  0x0000008, 0x20, 7, PutAll },
        { "LAS",  0x0000200, 0xBB, 0, PutAll },         /* X */
        { "LAX",  0x080A30C, 0xA3, 11, PutAll },        /* X */
        { "LDA",  0x080A26C, 0xa0, 0, PutAll },
        { "LDX",  0x080030C, 0xa2, 1, PutAll },
        { "LDY",  0x080006C, 0xa0, 1, PutAll },
        { "LSR",  0x000006F, 0x42, 1, PutAll },
        { "NOP",  0x080006D, 0x00, 10, PutAll },        /* X */
        { "ORA",  0x080A26C, 0x00, 0, PutAll },
        { "PHA",  0x0000001, 0x48, 0, PutAll },
        { "PHP",  0x0000001, 0x08, 0, PutAll },
        { "PLA",  0x0000001, 0x68, 0, PutAll },
        { "PLP",  0x0000001, 0x28, 0, PutAll },
        { "RLA",  0x000A26C, 0x23, 0, PutAll },         /* X */
        { "ROL",  0x000006F, 0x22, 1, PutAll },
        { "ROR",  0x000006F, 0x62, 1, PutAll },
        { "RRA",  0x000A26C, 0x63, 0, PutAll },         /* X */
        { "RTI",  0x0000001, 0x40, 0, PutAll },
        { "RTS",  0x0000001, 0x60, 0, PutAll },
        { "SAX",  0x000810C, 0x83, 1, PutAll },         /* X */
        { "SBC",  0x080A26C, 0xe0, 0, PutAll },
        { "SEC",  0x0000001, 0x38, 0, PutAll },
        { "SED",  0x0000001, 0xf8, 0, PutAll },
        { "SEI",  0x0000001, 0x78, 0, PutAll },
        { "SHA",  0x0002200, 0x93, 1, PutAll },         /* X */
        { "SHX",  0x0000200, 0x9e, 1, PutAll },         /* X */
        { "SHY",  0x0000040, 0x9c, 1, PutAll },         /* X */
        { "SLO",  0x000A26C, 0x03, 0, PutAll },         /* X */
        { "SRE",  0x000A26C, 0x43, 0, PutAll },         /* X */
        { "STA",  0x000A26C, 0x80, 0, PutAll },
        { "STX",  0x000010c, 0x82, 1, PutAll },
        { "STY",  0x000002c, 0x80, 1, PutAll },
        { "TAS",  0x0000200, 0x9b, 0, PutAll },         /* X */
        { "TAX",  0x0000001, 0xaa, 0, PutAll },
        { "TAY",  0x0000001, 0xa8, 0, PutAll },
        { "TSX",  0x0000001, 0xba, 0, PutAll },
        { "TXA",  0x0000001, 0x8a, 0, PutAll },
        { "TXS",  0x0000001, 0x9a, 0, PutAll },
        { "TYA",  0x0000001, 0x98, 0, PutAll }
    }
};

/* Instruction table for the 6502 with DTV extra opcodes (DTV) and
** those illegal instructions (X) which are supported by DTV.
** Note: illegals opcodes which contain more subinstructions
** (ASO, DCM, LSE, LXA, SBX and SHS) are not enlisted.
*/
static const struct {
    unsigned Count;
    InsDesc  Ins[71];
} InsTab6502DTV = {
    sizeof (InsTab6502DTV.Ins) / sizeof (InsTab6502DTV.Ins[0]),
    {
        { "ADC",  0x080A26C, 0x60, 0, PutAll },
        { "ALR",  0x0800000, 0x4B, 0, PutAll },         /* X */
        { "ANC",  0x0800000, 0x0B, 0, PutAll },         /* X */
        { "AND",  0x080A26C, 0x20, 0, PutAll },
        { "ANE",  0x0800000, 0x8B, 0, PutAll },         /* X */
        { "ARR",  0x0800000, 0x6B, 0, PutAll },         /* X */
        { "ASL",  0x000006e, 0x02, 1, PutAll },
        { "AXS",  0x0800000, 0xCB, 0, PutAll },         /* X */
        { "BCC",  0x0020000, 0x90, 0, PutPCRel8 },
        { "BCS",  0x0020000, 0xb0, 0, PutPCRel8 },
        { "BEQ",  0x0020000, 0xf0, 0, PutPCRel8 },
        { "BIT",  0x000000C, 0x00, 2, PutAll },
        { "BMI",  0x0020000, 0x30, 0, PutPCRel8 },
        { "BNE",  0x0020000, 0xd0, 0, PutPCRel8 },
        { "BPL",  0x0020000, 0x10, 0, PutPCRel8 },
        { "BRA",  0x0020000, 0x12, 0, PutPCRel8 },      /* DTV */
        { "BRK",  0x0800005, 0x00, 6, PutAll },
        { "BVC",  0x0020000, 0x50, 0, PutPCRel8 },
        { "BVS",  0x0020000, 0x70, 0, PutPCRel8 },
        { "CLC",  0x0000001, 0x18, 0, PutAll },
        { "CLD",  0x0000001, 0xd8, 0, PutAll },
        { "CLI",  0x0000001, 0x58, 0, PutAll },
        { "CLV",  0x0000001, 0xb8, 0, PutAll },
        { "CMP",  0x080A26C, 0xc0, 0, PutAll },
        { "CPX",  0x080000C, 0xe0, 1, PutAll },
        { "CPY",  0x080000C, 0xc0, 1, PutAll },
        { "DEC",  0x000006C, 0x00, 3, PutAll },
        { "DEX",  0x0000001, 0xca, 0, PutAll },
        { "DEY",  0x0000001, 0x88, 0, PutAll },
        { "EOR",  0x080A26C, 0x40, 0, PutAll },
        { "INC",  0x000006c, 0x00, 4, PutAll },
        { "INX",  0x0000001, 0xe8, 0, PutAll },
        { "INY",  0x0000001, 0xc8, 0, PutAll },
        { "JMP",  0x0000808, 0x4c, 6, PutJMP },
        { "JSR",  0x0000008, 0x20, 7, PutAll },
        { "LAS",  0x0000200, 0xBB, 0, PutAll },         /* X */
        { "LAX",  0x080A30C, 0xA3, 11, PutAll },        /* X */
        { "LDA",  0x080A26C, 0xa0, 0, PutAll },
        { "LDX",  0x080030C, 0xa2, 1, PutAll },
        { "LDY",  0x080006C, 0xa0, 1, PutAll },
        { "LSR",  0x000006F, 0x42, 1, PutAll },
        { "NOP",  0x080006D, 0x00, 10, PutAll },        /* X */
        { "ORA",  0x080A26C, 0x00, 0, PutAll },
        { "PHA",  0x0000001, 0x48, 0, PutAll },
        { "PHP",  0x0000001, 0x08, 0, PutAll },
        { "PLA",  0x0000001, 0x68, 0, PutAll },
        { "PLP",  0x0000001, 0x28, 0, PutAll },
        { "RLA",  0x000A26C, 0x23, 0, PutAll },         /* X */
        { "ROL",  0x000006F, 0x22, 1, PutAll },
        { "ROR",  0x000006F, 0x62, 1, PutAll },
        { "RRA",  0x000A26C, 0x63, 0, PutAll },         /* X */
        { "RTI",  0x0000001, 0x40, 0, PutAll },
        { "RTS",  0x0000001, 0x60, 0, PutAll },
        { "SAC",  0x0800000, 0x32, 1, PutAll },         /* DTV */
        { "SBC",  0x080A26C, 0xe0, 0, PutAll },
        { "SEC",  0x0000001, 0x38, 0, PutAll },
        { "SED",  0x0000001, 0xf8, 0, PutAll },
        { "SEI",  0x0000001, 0x78, 0, PutAll },
        { "SHA",  0x0002200, 0x93, 1, PutAll },         /* X */
        { "SHX",  0x0000200, 0x9e, 1, PutAll },         /* X */
        { "SHY",  0x0000040, 0x9c, 1, PutAll },         /* X */
        { "SIR",  0x0800000, 0x42, 1, PutAll },         /* DTV */
        { "STA",  0x000A26C, 0x80, 0, PutAll },
        { "STX",  0x000010c, 0x82, 1, PutAll },
        { "STY",  0x000002c, 0x80, 1, PutAll },
        { "TAX",  0x0000001, 0xaa, 0, PutAll },
        { "TAY",  0x0000001, 0xa8, 0, PutAll },
        { "TSX",  0x0000001, 0xba, 0, PutAll },
        { "TXA",  0x0000001, 0x8a, 0, PutAll },
        { "TXS",  0x0000001, 0x9a, 0, PutAll },
        { "TYA",  0x0000001, 0x98, 0, PutAll }
    }
};

/* Instruction table for the 65SC02 */
static const struct {
    unsigned Count;
    InsDesc  Ins[66];
} InsTab65SC02 = {
    sizeof (InsTab65SC02.Ins) / sizeof (InsTab65SC02.Ins[0]),
    {
        { "ADC",  0x080A66C, 0x60, 0, PutAll },
        { "AND",  0x080A66C, 0x20, 0, PutAll },
        { "ASL",  0x000006e, 0x02, 1, PutAll },
        { "BCC",  0x0020000, 0x90, 0, PutPCRel8 },
        { "BCS",  0x0020000, 0xb0, 0, PutPCRel8 },
        { "BEQ",  0x0020000, 0xf0, 0, PutPCRel8 },
        { "BIT",  0x0A0006C, 0x00, 2, PutAll },
        { "BMI",  0x0020000, 0x30, 0, PutPCRel8 },
        { "BNE",  0x0020000, 0xd0, 0, PutPCRel8 },
        { "BPL",  0x0020000, 0x10, 0, PutPCRel8 },
        { "BRA",  0x0020000, 0x80, 0, PutPCRel8 },
        { "BRK",  0x0800005, 0x00, 6, PutAll },
        { "BVC",  0x0020000, 0x50, 0, PutPCRel8 },
        { "BVS",  0x0020000, 0x70, 0, PutPCRel8 },
        { "CLC",  0x0000001, 0x18, 0, PutAll },
        { "CLD",  0x0000001, 0xd8, 0, PutAll },
        { "CLI",  0x0000001, 0x58, 0, PutAll },
        { "CLV",  0x0000001, 0xb8, 0, PutAll },
        { "CMP",  0x080A66C, 0xc0, 0, PutAll },
        { "CPX",  0x080000C, 0xe0, 1, PutAll },
        { "CPY",  0x080000C, 0xc0, 1, PutAll },
        { "DEA",  0x0000001, 0x00, 3, PutAll },   /* == DEC */
        { "DEC",  0x000006F, 0x00, 3, PutAll },
        { "DEX",  0x0000001, 0xca, 0, PutAll },
        { "DEY",  0x0000001, 0x88, 0, PutAll },
        { "EOR",  0x080A66C, 0x40, 0, PutAll },
        { "INA",  0x0000001, 0x00, 4, PutAll },   /* == INC */
        { "INC",  0x000006f, 0x00, 4, PutAll },
        { "INX",  0x0000001, 0xe8, 0, PutAll },
        { "INY",  0x0000001, 0xc8, 0, PutAll },
        { "JMP",  0x0010808, 0x4c, 6, PutAll },
        { "JSR",  0x0000008, 0x20, 7, PutAll },
        { "LDA",  0x080A66C, 0xa0, 0, PutAll },
        { "LDX",  0x080030C, 0xa2, 1, PutAll },
        { "LDY",  0x080006C, 0xa0, 1, PutAll },
        { "LSR",  0x000006F, 0x42, 1, PutAll },
        { "NOP",  0x0000001, 0xea, 0, PutAll },
        { "ORA",  0x080A66C, 0x00, 0, PutAll },
        { "PHA",  0x0000001, 0x48, 0, PutAll },
        { "PHP",  0x0000001, 0x08, 0, PutAll },
        { "PHX",  0x0000001, 0xda, 0, PutAll },
        { "PHY",  0x0000001, 0x5a, 0, PutAll },
        { "PLA",  0x0000001, 0x68, 0, PutAll },
        { "PLP",  0x0000001, 0x28, 0, PutAll },
        { "PLX",  0x0000001, 0xfa, 0, PutAll },
        { "PLY",  0x0000001, 0x7a, 0, PutAll },
        { "ROL",  0x000006F, 0x22, 1, PutAll },
        { "ROR",  0x000006F, 0x62, 1, PutAll },
        { "RTI",  0x0000001, 0x40, 0, PutAll },
        { "RTS",  0x0000001, 0x60, 0, PutAll },
        { "SBC",  0x080A66C, 0xe0, 0, PutAll },
        { "SEC",  0x0000001, 0x38, 0, PutAll },
        { "SED",  0x0000001, 0xf8, 0, PutAll },
        { "SEI",  0x0000001, 0x78, 0, PutAll },
        { "STA",  0x000A66C, 0x80, 0, PutAll },
        { "STX",  0x000010c, 0x82, 1, PutAll },
        { "STY",  0x000002c, 0x80, 1, PutAll },
        { "STZ",  0x000006c, 0x04, 5, PutAll },
        { "TAX",  0x0000001, 0xaa, 0, PutAll },
        { "TAY",  0x0000001, 0xa8, 0, PutAll },
        { "TRB",  0x000000c, 0x10, 1, PutAll },
        { "TSB",  0x000000c, 0x00, 1, PutAll },
        { "TSX",  0x0000001, 0xba, 0, PutAll },
        { "TXA",  0x0000001, 0x8a, 0, PutAll },
        { "TXS",  0x0000001, 0x9a, 0, PutAll },
        { "TYA",  0x0000001, 0x98, 0, PutAll }
    }
};

/* Instruction table for the 65C02 */
static const struct {
    unsigned Count;
    InsDesc  Ins[100];
} InsTab65C02 = {
    sizeof (InsTab65C02.Ins) / sizeof (InsTab65C02.Ins[0]),
    {
        { "ADC",  0x080A66C, 0x60, 0, PutAll },
        { "AND",  0x080A66C, 0x20, 0, PutAll },
        { "ASL",  0x000006e, 0x02, 1, PutAll },
        { "BBR0", 0x0000000, 0x0F, 0, PutBitBranch },
        { "BBR1", 0x0000000, 0x1F, 0, PutBitBranch },
        { "BBR2", 0x0000000, 0x2F, 0, PutBitBranch },
        { "BBR3", 0x0000000, 0x3F, 0, PutBitBranch },
        { "BBR4", 0x0000000, 0x4F, 0, PutBitBranch },
        { "BBR5", 0x0000000, 0x5F, 0, PutBitBranch },
        { "BBR6", 0x0000000, 0x6F, 0, PutBitBranch },
        { "BBR7", 0x0000000, 0x7F, 0, PutBitBranch },
        { "BBS0", 0x0000000, 0x8F, 0, PutBitBranch },
        { "BBS1", 0x0000000, 0x9F, 0, PutBitBranch },
        { "BBS2", 0x0000000, 0xAF, 0, PutBitBranch },
        { "BBS3", 0x0000000, 0xBF, 0, PutBitBranch },
        { "BBS4", 0x0000000, 0xCF, 0, PutBitBranch },
        { "BBS5", 0x0000000, 0xDF, 0, PutBitBranch },
        { "BBS6", 0x0000000, 0xEF, 0, PutBitBranch },
        { "BBS7", 0x0000000, 0xFF, 0, PutBitBranch },
        { "BCC",  0x0020000, 0x90, 0, PutPCRel8 },
        { "BCS",  0x0020000, 0xb0, 0, PutPCRel8 },
        { "BEQ",  0x0020000, 0xf0, 0, PutPCRel8 },
        { "BIT",  0x0A0006C, 0x00, 2, PutAll },
        { "BMI",  0x0020000, 0x30, 0, PutPCRel8 },
        { "BNE",  0x0020000, 0xd0, 0, PutPCRel8 },
        { "BPL",  0x0020000, 0x10, 0, PutPCRel8 },
        { "BRA",  0x0020000, 0x80, 0, PutPCRel8 },
        { "BRK",  0x0800005, 0x00, 6, PutAll },
        { "BVC",  0x0020000, 0x50, 0, PutPCRel8 },
        { "BVS",  0x0020000, 0x70, 0, PutPCRel8 },
        { "CLC",  0x0000001, 0x18, 0, PutAll },
        { "CLD",  0x0000001, 0xd8, 0, PutAll },
        { "CLI",  0x0000001, 0x58, 0, PutAll },
        { "CLV",  0x0000001, 0xb8, 0, PutAll },
        { "CMP",  0x080A66C, 0xc0, 0, PutAll },
        { "CPX",  0x080000C, 0xe0, 1, PutAll },
        { "CPY",  0x080000C, 0xc0, 1, PutAll },
        { "DEA",  0x0000001, 0x00, 3, PutAll },   /* == DEC */
        { "DEC",  0x000006F, 0x00, 3, PutAll },
        { "DEX",  0x0000001, 0xca, 0, PutAll },
        { "DEY",  0x0000001, 0x88, 0, PutAll },
        { "EOR",  0x080A66C, 0x40, 0, PutAll },
        { "INA",  0x0000001, 0x00, 4, PutAll },   /* == INC */
        { "INC",  0x000006f, 0x00, 4, PutAll },
        { "INX",  0x0000001, 0xe8, 0, PutAll },
        { "INY",  0x0000001, 0xc8, 0, PutAll },
        { "JMP",  0x0010808, 0x4c, 6, PutAll },
        { "JSR",  0x0000008, 0x20, 7, PutAll },
        { "LDA",  0x080A66C, 0xa0, 0, PutAll },
        { "LDX",  0x080030C, 0xa2, 1, PutAll },
        { "LDY",  0x080006C, 0xa0, 1, PutAll },
        { "LSR",  0x000006F, 0x42, 1, PutAll },
        { "NOP",  0x0000001, 0xea, 0, PutAll },
        { "ORA",  0x080A66C, 0x00, 0, PutAll },
        { "PHA",  0x0000001, 0x48, 0, PutAll },
        { "PHP",  0x0000001, 0x08, 0, PutAll },
        { "PHX",  0x0000001, 0xda, 0, PutAll },
        { "PHY",  0x0000001, 0x5a, 0, PutAll },
        { "PLA",  0x0000001, 0x68, 0, PutAll },
        { "PLP",  0x0000001, 0x28, 0, PutAll },
        { "PLX",  0x0000001, 0xfa, 0, PutAll },
        { "PLY",  0x0000001, 0x7a, 0, PutAll },
        { "RMB0", 0x0000004, 0x07, 1, PutAll },
        { "RMB1", 0x0000004, 0x17, 1, PutAll },
        { "RMB2", 0x0000004, 0x27, 1, PutAll },
        { "RMB3", 0x0000004, 0x37, 1, PutAll },
        { "RMB4", 0x0000004, 0x47, 1, PutAll },
        { "RMB5", 0x0000004, 0x57, 1, PutAll },
        { "RMB6", 0x0000004, 0x67, 1, PutAll },
        { "RMB7", 0x0000004, 0x77, 1, PutAll },
        { "ROL",  0x000006F, 0x22, 1, PutAll },
        { "ROR",  0x000006F, 0x62, 1, PutAll },
        { "RTI",  0x0000001, 0x40, 0, PutAll },
        { "RTS",  0x0000001, 0x60, 0, PutAll },
        { "SBC",  0x080A66C, 0xe0, 0, PutAll },
        { "SEC",  0x0000001, 0x38, 0, PutAll },
        { "SED",  0x0000001, 0xf8, 0, PutAll },
        { "SEI",  0x0000001, 0x78, 0, PutAll },
        { "SMB0", 0x0000004, 0x87, 1, PutAll },
        { "SMB1", 0x0000004, 0x97, 1, PutAll },
        { "SMB2", 0x0000004, 0xA7, 1, PutAll },
        { "SMB3", 0x0000004, 0xB7, 1, PutAll },
        { "SMB4", 0x0000004, 0xC7, 1, PutAll },
        { "SMB5", 0x0000004, 0xD7, 1, PutAll },
        { "SMB6", 0x0000004, 0xE7, 1, PutAll },
        { "SMB7", 0x0000004, 0xF7, 1, PutAll },
        { "STA",  0x000A66C, 0x80, 0, PutAll },
        { "STP",  0x0000001, 0xdb, 0, PutAll },
        { "STX",  0x000010c, 0x82, 1, PutAll },
        { "STY",  0x000002c, 0x80, 1, PutAll },
        { "STZ",  0x000006c, 0x04, 5, PutAll },
        { "TAX",  0x0000001, 0xaa, 0, PutAll },
        { "TAY",  0x0000001, 0xa8, 0, PutAll },
        { "TRB",  0x000000c, 0x10, 1, PutAll },
        { "TSB",  0x000000c, 0x00, 1, PutAll },
        { "TSX",  0x0000001, 0xba, 0, PutAll },
        { "TXA",  0x0000001, 0x8a, 0, PutAll },
        { "TXS",  0x0000001, 0x9a, 0, PutAll },
        { "TYA",  0x0000001, 0x98, 0, PutAll },
        { "WAI",  0x0000001, 0xcb, 0, PutAll }
    }
};

/* Instruction table for the 4510 */
static const struct {
    unsigned Count;
    InsDesc  Ins[133];
} InsTab4510 = {
    sizeof (InsTab4510.Ins) / sizeof (InsTab4510.Ins[0]),
    {
        { "ADC",  0x080A66C, 0x60, 0, PutAll },
        { "AND",  0x080A66C, 0x20, 0, PutAll },
        { "ASL",  0x000006e, 0x02, 1, PutAll },
        { "ASR",  0x0000026, 0x43, 0, Put4510 },
        { "ASW",  0x0000008, 0xcb, 6, PutAll },
        { "BBR0", 0x0000000, 0x0F, 0, PutBitBranch },
        { "BBR1", 0x0000000, 0x1F, 0, PutBitBranch },
        { "BBR2", 0x0000000, 0x2F, 0, PutBitBranch },
        { "BBR3", 0x0000000, 0x3F, 0, PutBitBranch },
        { "BBR4", 0x0000000, 0x4F, 0, PutBitBranch },
        { "BBR5", 0x0000000, 0x5F, 0, PutBitBranch },
        { "BBR6", 0x0000000, 0x6F, 0, PutBitBranch },
        { "BBR7", 0x0000000, 0x7F, 0, PutBitBranch },
        { "BBS0", 0x0000000, 0x8F, 0, PutBitBranch },
        { "BBS1", 0x0000000, 0x9F, 0, PutBitBranch },
        { "BBS2", 0x0000000, 0xAF, 0, PutBitBranch },
        { "BBS3", 0x0000000, 0xBF, 0, PutBitBranch },
        { "BBS4", 0x0000000, 0xCF, 0, PutBitBranch },
        { "BBS5", 0x0000000, 0xDF, 0, PutBitBranch },
        { "BBS6", 0x0000000, 0xEF, 0, PutBitBranch },
        { "BBS7", 0x0000000, 0xFF, 0, PutBitBranch },
        { "BCC",  0x0020000, 0x90, 0, PutPCRel8 },
        { "BCS",  0x0020000, 0xb0, 0, PutPCRel8 },
        { "BEQ",  0x0020000, 0xf0, 0, PutPCRel8 },
        { "BIT",  0x0A0006C, 0x00, 2, PutAll },
        { "BMI",  0x0020000, 0x30, 0, PutPCRel8 },
        { "BNE",  0x0020000, 0xd0, 0, PutPCRel8 },
        { "BPL",  0x0020000, 0x10, 0, PutPCRel8 },
        { "BRA",  0x0020000, 0x80, 0, PutPCRel8 },
        { "BRK",  0x0800005, 0x00, 6, PutAll },
        { "BSR",  0x0040000, 0x63, 0, PutPCRel4510 },
        { "BVC",  0x0020000, 0x50, 0, PutPCRel8 },
        { "BVS",  0x0020000, 0x70, 0, PutPCRel8 },
        { "CLC",  0x0000001, 0x18, 0, PutAll },
        { "CLD",  0x0000001, 0xd8, 0, PutAll },
        { "CLE",  0x0000001, 0x02, 0, PutAll },
        { "CLI",  0x0000001, 0x58, 0, PutAll },
        { "CLV",  0x0000001, 0xb8, 0, PutAll },
        { "CMP",  0x080A66C, 0xc0, 0, PutAll },
        { "CPX",  0x080000C, 0xe0, 1, PutAll },
        { "CPY",  0x080000C, 0xc0, 1, PutAll },
        { "CPZ",  0x080000C, 0xd0, 1, Put4510 },
        { "DEA",  0x0000001, 0x00, 3, PutAll },   /* == DEC */
        { "DEC",  0x000006F, 0x00, 3, PutAll },
        { "DEW",  0x0000004, 0xc3, 9, PutAll },
        { "DEX",  0x0000001, 0xca, 0, PutAll },
        { "DEY",  0x0000001, 0x88, 0, PutAll },
        { "DEZ",  0x0000001, 0x3B, 0, PutAll },
        { "EOM",  0x0000001, 0xea, 0, PutAll },
        { "EOR",  0x080A66C, 0x40, 0, PutAll },
        { "INA",  0x0000001, 0x00, 4, PutAll },   /* == INC */
        { "INC",  0x000006f, 0x00, 4, PutAll },
        { "INW",  0x0000004, 0xe3, 9, PutAll },
        { "INX",  0x0000001, 0xe8, 0, PutAll },
        { "INY",  0x0000001, 0xc8, 0, PutAll },
        { "INZ",  0x0000001, 0x1B, 0, PutAll },
        { "JMP",  0x0010808, 0x4c, 6, PutAll },
        { "JSR",  0x0010808, 0x20, 7, Put4510 },
        { "LBCC", 0x0040000, 0x93, 0, PutPCRel4510 },
        { "LBCS", 0x0040000, 0xb3, 0, PutPCRel4510 },
        { "LBEQ", 0x0040000, 0xf3, 0, PutPCRel4510 },
        { "LBMI", 0x0040000, 0x33, 0, PutPCRel4510 },
        { "LBNE", 0x0040000, 0xd3, 0, PutPCRel4510 },
        { "LBPL", 0x0040000, 0x13, 0, PutPCRel4510 },
        { "LBRA", 0x0040000, 0x83, 0, PutPCRel4510 },
        { "LBVC", 0x0040000, 0x53, 0, PutPCRel4510 },
        { "LBVS", 0x0040000, 0x73, 0, PutPCRel4510 },
        { "LDA",  0x090A66C, 0xa0, 0, Put4510 },
        { "LDX",  0x080030C, 0xa2, 1, PutAll },
        { "LDY",  0x080006C, 0xa0, 1, PutAll },
        { "LDZ",  0x0800048, 0xa3, 1, Put4510 },
        { "LSR",  0x000006F, 0x42, 1, PutAll },
        { "MAP",  0x0000001, 0x5C, 0, PutAll },
        { "NEG",  0x0000001, 0x42, 0, PutAll },
        { "NOP",  0x0000001, 0xea, 0, PutAll }, /* == EOM */
        { "ORA",  0x080A66C, 0x00, 0, PutAll },
        { "PHA",  0x0000001, 0x48, 0, PutAll },
        { "PHD",  0x8000008, 0xf4, 1, PutAll }, /* == PHW */
        { "PHP",  0x0000001, 0x08, 0, PutAll },
        { "PHW",  0x8000008, 0xf4, 1, PutAll },
        { "PHX",  0x0000001, 0xda, 0, PutAll },
        { "PHY",  0x0000001, 0x5a, 0, PutAll },
        { "PHZ",  0x0000001, 0xdb, 0, PutAll },
        { "PLA",  0x0000001, 0x68, 0, PutAll },
        { "PLP",  0x0000001, 0x28, 0, PutAll },
        { "PLX",  0x0000001, 0xfa, 0, PutAll },
        { "PLY",  0x0000001, 0x7a, 0, PutAll },
        { "PLZ",  0x0000001, 0xfb, 0, PutAll },
        { "RMB0", 0x0000004, 0x07, 1, PutAll },
        { "RMB1", 0x0000004, 0x17, 1, PutAll },
        { "RMB2", 0x0000004, 0x27, 1, PutAll },
        { "RMB3", 0x0000004, 0x37, 1, PutAll },
        { "RMB4", 0x0000004, 0x47, 1, PutAll },
        { "RMB5", 0x0000004, 0x57, 1, PutAll },
        { "RMB6", 0x0000004, 0x67, 1, PutAll },
        { "RMB7", 0x0000004, 0x77, 1, PutAll },
        { "ROL",  0x000006F, 0x22, 1, PutAll },
        { "ROR",  0x000006F, 0x62, 1, PutAll },
        { "ROW",  0x0000008, 0xeb, 6, PutAll },
        { "RTI",  0x0000001, 0x40, 0, PutAll },
        { "RTN",  0x0800000, 0x62, 1, PutAll },
        { "RTS",  0x0000001, 0x60, 0, PutAll },
        { "SBC",  0x080A66C, 0xe0, 0, PutAll },
        { "SEC",  0x0000001, 0x38, 0, PutAll },
        { "SED",  0x0000001, 0xf8, 0, PutAll },
        { "SEE",  0x0000001, 0x03, 0, PutAll },
        { "SEI",  0x0000001, 0x78, 0, PutAll },
        { "SMB0", 0x0000004, 0x87, 1, PutAll },
        { "SMB1", 0x0000004, 0x97, 1, PutAll },
        { "SMB2", 0x0000004, 0xA7, 1, PutAll },
        { "SMB3", 0x0000004, 0xB7, 1, PutAll },
        { "SMB4", 0x0000004, 0xC7, 1, PutAll },
        { "SMB5", 0x0000004, 0xD7, 1, PutAll },
        { "SMB6", 0x0000004, 0xE7, 1, PutAll },
        { "SMB7", 0x0000004, 0xF7, 1, PutAll },
        { "STA",  0x010A66C, 0x80, 0, Put4510 },
        { "STX",  0x000030c, 0x82, 1, Put4510 },
        { "STY",  0x000006c, 0x80, 1, Put4510 },
        { "STZ",  0x000006c, 0x04, 5, PutAll },
        { "TAB",  0x0000001, 0x5b, 0, PutAll },
        { "TAX",  0x0000001, 0xaa, 0, PutAll },
        { "TAY",  0x0000001, 0xa8, 0, PutAll },
        { "TAZ",  0x0000001, 0x4b, 0, PutAll },
        { "TBA",  0x0000001, 0x7b, 0, PutAll },
        { "TRB",  0x000000c, 0x10, 1, PutAll },
        { "TSB",  0x000000c, 0x00, 1, PutAll },
        { "TSX",  0x0000001, 0xba, 0, PutAll },
        { "TSY",  0x0000001, 0x0b, 0, PutAll },
        { "TXA",  0x0000001, 0x8a, 0, PutAll },
        { "TXS",  0x0000001, 0x9a, 0, PutAll },
        { "TYA",  0x0000001, 0x98, 0, PutAll },
        { "TYS",  0x0000001, 0x2b, 0, PutAll },
        { "TZA",  0x0000001, 0x6b, 0, PutAll },
    }
};

/* Instruction table for the 65816 */
static const struct {
    unsigned Count;
    InsDesc  Ins[100];
} InsTab65816 = {
    sizeof (InsTab65816.Ins) / sizeof (InsTab65816.Ins[0]),
    {
        { "ADC",  0x0b8f6fc, 0x60, 0, PutAll },
        { "AND",  0x0b8f6fc, 0x20, 0, PutAll },
        { "ASL",  0x000006e, 0x02, 1, PutAll },
        { "BCC",  0x0020000, 0x90, 0, PutPCRel8 },
        { "BCS",  0x0020000, 0xb0, 0, PutPCRel8 },
        { "BEQ",  0x0020000, 0xf0, 0, PutPCRel8 },
        { "BIT",  0x0a0006c, 0x00, 2, PutAll },
        { "BMI",  0x0020000, 0x30, 0, PutPCRel8 },
        { "BNE",  0x0020000, 0xd0, 0, PutPCRel8 },
        { "BPL",  0x0020000, 0x10, 0, PutPCRel8 },
        { "BRA",  0x0020000, 0x80, 0, PutPCRel8 },
        { "BRK",  0x0800005, 0x00, 6, PutAll },
        { "BRL",  0x0040000, 0x82, 0, PutPCRel16 },
        { "BVC",  0x0020000, 0x50, 0, PutPCRel8 },
        { "BVS",  0x0020000, 0x70, 0, PutPCRel8 },
        { "CLC",  0x0000001, 0x18, 0, PutAll },
        { "CLD",  0x0000001, 0xd8, 0, PutAll },
        { "CLI",  0x0000001, 0x58, 0, PutAll },
        { "CLV",  0x0000001, 0xb8, 0, PutAll },
        { "CMP",  0x0b8f6fc, 0xc0, 0, PutAll },
        { "COP",  0x0800005, 0x02, 6, PutAll },
        { "CPA",  0x0b8f6fc, 0xc0, 0, PutAll },   /* == CMP */
        { "CPX",  0x0c0000c, 0xe0, 1, PutAll },
        { "CPY",  0x0c0000c, 0xc0, 1, PutAll },
        { "DEA",  0x0000001, 0x00, 3, PutAll },   /* == DEC */
        { "DEC",  0x000006F, 0x00, 3, PutAll },
        { "DEX",  0x0000001, 0xca, 0, PutAll },
        { "DEY",  0x0000001, 0x88, 0, PutAll },
        { "EOR",  0x0b8f6fc, 0x40, 0, PutAll },
        { "INA",  0x0000001, 0x00, 4, PutAll },   /* == INC */
        { "INC",  0x000006F, 0x00, 4, PutAll },
        { "INX",  0x0000001, 0xe8, 0, PutAll },
        { "INY",  0x0000001, 0xc8, 0, PutAll },
        { "JML",  0x4000010, 0x5c, 1, PutAll },
        { "JMP",  0x4010818, 0x4c, 6, PutJMP816 },
        { "JSL",  0x0000010, 0x20, 7, PutAll },
        { "JSR",  0x0010018, 0x20, 7, PutJSR816 },
        { "LDA",  0x0b8f6fc, 0xa0, 0, PutAll },
        { "LDX",  0x0c0030c, 0xa2, 1, PutAll },
        { "LDY",  0x0c0006c, 0xa0, 1, PutAll },
        { "LSR",  0x000006F, 0x42, 1, PutAll },
        { "MVN",  0x1000000, 0x54, 0, PutBlockMove },
        { "MVP",  0x1000000, 0x44, 0, PutBlockMove },
        { "NOP",  0x0000001, 0xea, 0, PutAll },
        { "ORA",  0x0b8f6fc, 0x00, 0, PutAll },
        { "PEA",  0x0000008, 0xf4, 6, PutAll },
        { "PEI",  0x0000400, 0xd4, 1, PutAll },
        { "PER",  0x0040000, 0x62, 0, PutPCRel16 },
        { "PHA",  0x0000001, 0x48, 0, PutAll },
        { "PHB",  0x0000001, 0x8b, 0, PutAll },
        { "PHD",  0x0000001, 0x0b, 0, PutAll },
        { "PHK",  0x0000001, 0x4b, 0, PutAll },
        { "PHP",  0x0000001, 0x08, 0, PutAll },
        { "PHX",  0x0000001, 0xda, 0, PutAll },
        { "PHY",  0x0000001, 0x5a, 0, PutAll },
        { "PLA",  0x0000001, 0x68, 0, PutAll },
        { "PLB",  0x0000001, 0xab, 0, PutAll },
        { "PLD",  0x0000001, 0x2b, 0, PutAll },
        { "PLP",  0x0000001, 0x28, 0, PutAll },
        { "PLX",  0x0000001, 0xfa, 0, PutAll },
        { "PLY",  0x0000001, 0x7a, 0, PutAll },
        { "REP",  0x0800000, 0xc2, 1, PutREP },
        { "ROL",  0x000006F, 0x22, 1, PutAll },
        { "ROR",  0x000006F, 0x62, 1, PutAll },
        { "RTI",  0x0000001, 0x40, 0, PutAll },
        { "RTL",  0x0000001, 0x6b, 0, PutAll },
        { "RTS",  0x0000001, 0x60, 0, PutRTS },
        { "SBC",  0x0b8f6fc, 0xe0, 0, PutAll },
        { "SEC",  0x0000001, 0x38, 0, PutAll },
        { "SED",  0x0000001, 0xf8, 0, PutAll },
        { "SEI",  0x0000001, 0x78, 0, PutAll },
        { "SEP",  0x0800000, 0xe2, 1, PutSEP },
        { "STA",  0x018f6fc, 0x80, 0, PutAll },
        { "STP",  0x0000001, 0xdb, 0, PutAll },
        { "STX",  0x000010c, 0x82, 1, PutAll },
        { "STY",  0x000002c, 0x80, 1, PutAll },
        { "STZ",  0x000006c, 0x04, 5, PutAll },
        { "SWA",  0x0000001, 0xeb, 0, PutAll },   /* == XBA */
        { "TAD",  0x0000001, 0x5b, 0, PutAll },   /* == TCD */
        { "TAS",  0x0000001, 0x1b, 0, PutAll },   /* == TCS */
        { "TAX",  0x0000001, 0xaa, 0, PutAll },
        { "TAY",  0x0000001, 0xa8, 0, PutAll },
        { "TCD",  0x0000001, 0x5b, 0, PutAll },
        { "TCS",  0x0000001, 0x1b, 0, PutAll },
        { "TDA",  0x0000001, 0x7b, 0, PutAll },   /* == TDC */
        { "TDC",  0x0000001, 0x7b, 0, PutAll },
        { "TRB",  0x000000c, 0x10, 1, PutAll },
        { "TSA",  0x0000001, 0x3b, 0, PutAll },   /* == TSC */
        { "TSB",  0x000000c, 0x00, 1, PutAll },
        { "TSC",  0x0000001, 0x3b, 0, PutAll },
        { "TSX",  0x0000001, 0xba, 0, PutAll },
        { "TXA",  0x0000001, 0x8a, 0, PutAll },
        { "TXS",  0x0000001, 0x9a, 0, PutAll },
        { "TXY",  0x0000001, 0x9b, 0, PutAll },
        { "TYA",  0x0000001, 0x98, 0, PutAll },
        { "TYX",  0x0000001, 0xbb, 0, PutAll },
        { "WAI",  0x0000001, 0xcb, 0, PutAll },
        { "WDM",  0x0800004, 0x42, 6, PutAll },
        { "XBA",  0x0000001, 0xeb, 0, PutAll },
        { "XCE",  0x0000001, 0xfb, 0, PutAll }
    }
};

/* Instruction table for the SWEET16 pseudo CPU */
static const struct {
    unsigned Count;
    InsDesc  Ins[26];
} InsTabSweet16 = {
    sizeof (InsTabSweet16.Ins) / sizeof (InsTabSweet16.Ins[0]),
    {
        { "ADD",  AMSW16_REG,              0xA0, 0, PutSweet16 },
        { "BC",   AMSW16_BRA,              0x03, 0, PutSweet16Branch },
        { "BK",   AMSW16_IMP,              0x0A, 0, PutSweet16 },
        { "BM",   AMSW16_BRA,              0x05, 0, PutSweet16Branch },
        { "BM1",  AMSW16_BRA,              0x08, 0, PutSweet16Branch },
        { "BNC",  AMSW16_BRA,              0x02, 0, PutSweet16Branch },
        { "BNM1", AMSW16_BRA,              0x09, 0, PutSweet16Branch },
        { "BNZ",  AMSW16_BRA,              0x07, 0, PutSweet16Branch },
        { "BP",   AMSW16_BRA,              0x04, 0, PutSweet16Branch },
        { "BR",   AMSW16_BRA,              0x01, 0, PutSweet16Branch },
        { "BS",   AMSW16_BRA,              0x0C, 0, PutSweet16Branch },
        { "BZ",   AMSW16_BRA,              0x06, 0, PutSweet16Branch },
        { "CPR",  AMSW16_REG,              0xD0, 0, PutSweet16 },
        { "DCR",  AMSW16_REG,              0xF0, 0, PutSweet16 },
        { "INR",  AMSW16_REG,              0xE0, 0, PutSweet16 },
        { "LD",   AMSW16_REG | AMSW16_IND, 0x00, 1, PutSweet16 },
        { "LDD",  AMSW16_IND,              0x60, 0, PutSweet16 },
        { "POP",  AMSW16_IND,              0x80, 0, PutSweet16 },
        { "POPD", AMSW16_IND,              0xC0, 0, PutSweet16 },
        { "RS",   AMSW16_IMP,              0x0B, 0, PutSweet16 },
        { "RTN",  AMSW16_IMP,              0x00, 0, PutSweet16 },
        { "SET",  AMSW16_IMM,              0x10, 0, PutSweet16 },
        { "ST",   AMSW16_REG | AMSW16_IND, 0x10, 1, PutSweet16 },
        { "STD",  AMSW16_IND,              0x70, 0, PutSweet16 },
        { "STP",  AMSW16_IND,              0x90, 0, PutSweet16 },
        { "SUB",  AMSW16_REG,              0xB0, 0, PutSweet16 },
    }
};

/* Instruction table for the HuC6280 (the CPU used in the PC engine) */
static const struct {
    unsigned Count;
    InsDesc  Ins[135];
} InsTabHuC6280 = {
    sizeof (InsTabHuC6280.Ins) / sizeof (InsTabHuC6280.Ins[0]),
    {
        { "ADC",  0x080A66C, 0x60, 0, PutAll },
        { "AND",  0x080A66C, 0x20, 0, PutAll },
        { "ASL",  0x000006e, 0x02, 1, PutAll },
        { "BBR0", 0x0000000, 0x0F, 0, PutBitBranch },
        { "BBR1", 0x0000000, 0x1F, 0, PutBitBranch },
        { "BBR2", 0x0000000, 0x2F, 0, PutBitBranch },
        { "BBR3", 0x0000000, 0x3F, 0, PutBitBranch },
        { "BBR4", 0x0000000, 0x4F, 0, PutBitBranch },
        { "BBR5", 0x0000000, 0x5F, 0, PutBitBranch },
        { "BBR6", 0x0000000, 0x6F, 0, PutBitBranch },
        { "BBR7", 0x0000000, 0x7F, 0, PutBitBranch },
        { "BBS0", 0x0000000, 0x8F, 0, PutBitBranch },
        { "BBS1", 0x0000000, 0x9F, 0, PutBitBranch },
        { "BBS2", 0x0000000, 0xAF, 0, PutBitBranch },
        { "BBS3", 0x0000000, 0xBF, 0, PutBitBranch },
        { "BBS4", 0x0000000, 0xCF, 0, PutBitBranch },
        { "BBS5", 0x0000000, 0xDF, 0, PutBitBranch },
        { "BBS6", 0x0000000, 0xEF, 0, PutBitBranch },
        { "BBS7", 0x0000000, 0xFF, 0, PutBitBranch },
        { "BCC",  0x0020000, 0x90, 0, PutPCRel8 },
        { "BCS",  0x0020000, 0xb0, 0, PutPCRel8 },
        { "BEQ",  0x0020000, 0xf0, 0, PutPCRel8 },
        { "BIT",  0x0A0006C, 0x00, 2, PutAll },
        { "BMI",  0x0020000, 0x30, 0, PutPCRel8 },
        { "BNE",  0x0020000, 0xd0, 0, PutPCRel8 },
        { "BPL",  0x0020000, 0x10, 0, PutPCRel8 },
        { "BRA",  0x0020000, 0x80, 0, PutPCRel8 },
        { "BRK",  0x0800005, 0x00, 6, PutAll },
        { "BSR",  0x0020000, 0x44, 0, PutPCRel8 },
        { "BVC",  0x0020000, 0x50, 0, PutPCRel8 },
        { "BVS",  0x0020000, 0x70, 0, PutPCRel8 },
        { "CLA",  0x0000001, 0x62, 0, PutAll },
        { "CLC",  0x0000001, 0x18, 0, PutAll },
        { "CLD",  0x0000001, 0xd8, 0, PutAll },
        { "CLI",  0x0000001, 0x58, 0, PutAll },
        { "CLV",  0x0000001, 0xb8, 0, PutAll },
        { "CLX",  0x0000001, 0x82, 0, PutAll },
        { "CLY",  0x0000001, 0xc2, 0, PutAll },
        { "CMP",  0x080A66C, 0xc0, 0, PutAll },
        { "CPX",  0x080000C, 0xe0, 1, PutAll },
        { "CPY",  0x080000C, 0xc0, 1, PutAll },
        { "CSH",  0x0000001, 0xd4, 0, PutAll },
        { "CSL",  0x0000001, 0x54, 0, PutAll },
        { "DEA",  0x0000001, 0x00, 3, PutAll },   /* == DEC */
        { "DEC",  0x000006F, 0x00, 3, PutAll },
        { "DEX",  0x0000001, 0xca, 0, PutAll },
        { "DEY",  0x0000001, 0x88, 0, PutAll },
        { "EOR",  0x080A66C, 0x40, 0, PutAll },
        { "INA",  0x0000001, 0x00, 4, PutAll },   /* == INC */
        { "INC",  0x000006f, 0x00, 4, PutAll },
        { "INX",  0x0000001, 0xe8, 0, PutAll },
        { "INY",  0x0000001, 0xc8, 0, PutAll },
        { "JMP",  0x0010808, 0x4c, 6, PutAll },
        { "JSR",  0x0000008, 0x20, 7, PutAll },
        { "LDA",  0x080A66C, 0xa0, 0, PutAll },
        { "LDX",  0x080030C, 0xa2, 1, PutAll },
        { "LDY",  0x080006C, 0xa0, 1, PutAll },
        { "LSR",  0x000006F, 0x42, 1, PutAll },
        { "NOP",  0x0000001, 0xea, 0, PutAll },
        { "ORA",  0x080A66C, 0x00, 0, PutAll },
        { "PHA",  0x0000001, 0x48, 0, PutAll },
        { "PHP",  0x0000001, 0x08, 0, PutAll },
        { "PHX",  0x0000001, 0xda, 0, PutAll },
        { "PHY",  0x0000001, 0x5a, 0, PutAll },
        { "PLA",  0x0000001, 0x68, 0, PutAll },
        { "PLP",  0x0000001, 0x28, 0, PutAll },
        { "PLX",  0x0000001, 0xfa, 0, PutAll },
        { "PLY",  0x0000001, 0x7a, 0, PutAll },
        { "RMB0", 0x0000004, 0x07, 1, PutAll },
        { "RMB1", 0x0000004, 0x17, 1, PutAll },
        { "RMB2", 0x0000004, 0x27, 1, PutAll },
        { "RMB3", 0x0000004, 0x37, 1, PutAll },
        { "RMB4", 0x0000004, 0x47, 1, PutAll },
        { "RMB5", 0x0000004, 0x57, 1, PutAll },
        { "RMB6", 0x0000004, 0x67, 1, PutAll },
        { "RMB7", 0x0000004, 0x77, 1, PutAll },
        { "ROL",  0x000006F, 0x22, 1, PutAll },
        { "ROR",  0x000006F, 0x62, 1, PutAll },
        { "RTI",  0x0000001, 0x40, 0, PutAll },
        { "RTS",  0x0000001, 0x60, 0, PutAll },
        { "SAX",  0x0000001, 0x22, 0, PutAll },
        { "SAY",  0x0000001, 0x42, 0, PutAll },
        { "SBC",  0x080A66C, 0xe0, 0, PutAll },
        { "SEC",  0x0000001, 0x38, 0, PutAll },
        { "SED",  0x0000001, 0xf8, 0, PutAll },
        { "SEI",  0x0000001, 0x78, 0, PutAll },
        { "SET",  0x0000001, 0xf4, 0, PutAll },
        { "SMB0", 0x0000004, 0x87, 1, PutAll },
        { "SMB1", 0x0000004, 0x97, 1, PutAll },
        { "SMB2", 0x0000004, 0xA7, 1, PutAll },
        { "SMB3", 0x0000004, 0xB7, 1, PutAll },
        { "SMB4", 0x0000004, 0xC7, 1, PutAll },
        { "SMB5", 0x0000004, 0xD7, 1, PutAll },
        { "SMB6", 0x0000004, 0xE7, 1, PutAll },
        { "SMB7", 0x0000004, 0xF7, 1, PutAll },
        { "ST0",  0x0800000, 0x03, 1, PutAll },
        { "ST1",  0x0800000, 0x13, 1, PutAll },
        { "ST2",  0x0800000, 0x23, 1, PutAll },
        { "STA",  0x000A66C, 0x80, 0, PutAll },
        { "STX",  0x000010c, 0x82, 1, PutAll },
        { "STY",  0x000002c, 0x80, 1, PutAll },
        { "STZ",  0x000006c, 0x04, 5, PutAll },
        { "SXY",  0x0000001, 0x02, 0, PutAll },
        { "TAI",  0x2000000, 0xf3, 0, PutBlockTransfer },
        { "TAM",  0x0800000, 0x53, 1, PutAll },
        { "TAM0", 0x0000001, 0x01, 0, PutTAMn},
        { "TAM1", 0x0000001, 0x02, 0, PutTAMn},
        { "TAM2", 0x0000001, 0x04, 0, PutTAMn},
        { "TAM3", 0x0000001, 0x08, 0, PutTAMn},
        { "TAM4", 0x0000001, 0x10, 0, PutTAMn},
        { "TAM5", 0x0000001, 0x20, 0, PutTAMn},
        { "TAM6", 0x0000001, 0x40, 0, PutTAMn},
        { "TAM7", 0x0000001, 0x80, 0, PutTAMn},
        { "TAX",  0x0000001, 0xaa, 0, PutAll },
        { "TAY",  0x0000001, 0xa8, 0, PutAll },
        { "TDD",  0x2000000, 0xc3, 0, PutBlockTransfer },
        { "TIA",  0x2000000, 0xe3, 0, PutBlockTransfer },
        { "TII",  0x2000000, 0x73, 0, PutBlockTransfer },
        { "TIN",  0x2000000, 0xD3, 0, PutBlockTransfer },
        { "TMA",  0x0800000, 0x43, 1, PutTMA },
        { "TMA0", 0x0000001, 0x01, 0, PutTMAn},
        { "TMA1", 0x0000001, 0x02, 0, PutTMAn},
        { "TMA2", 0x0000001, 0x04, 0, PutTMAn},
        { "TMA3", 0x0000001, 0x08, 0, PutTMAn},
        { "TMA4", 0x0000001, 0x10, 0, PutTMAn},
        { "TMA5", 0x0000001, 0x20, 0, PutTMAn},
        { "TMA6", 0x0000001, 0x40, 0, PutTMAn},
        { "TMA7", 0x0000001, 0x80, 0, PutTMAn},
        { "TRB",  0x000000c, 0x10, 1, PutAll },
        { "TSB",  0x000000c, 0x00, 1, PutAll },
        { "TST",  0x000006c, 0x83, 9, PutTST },
        { "TSX",  0x0000001, 0xba, 0, PutAll },
        { "TXA",  0x0000001, 0x8a, 0, PutAll },
        { "TXS",  0x0000001, 0x9a, 0, PutAll },
        { "TYA",  0x0000001, 0x98, 0, PutAll }
    }
};



/* An array with instruction tables */
static const InsTable* InsTabs[CPU_COUNT] = {
    (const InsTable*) &InsTabNone,
    (const InsTable*) &InsTab6502,
    (const InsTable*) &InsTab6502X,
    (const InsTable*) &InsTab6502DTV,
    (const InsTable*) &InsTab65SC02,
    (const InsTable*) &InsTab65C02,
    (const InsTable*) &InsTab65816,
    (const InsTable*) &InsTabSweet16,
    (const InsTable*) &InsTabHuC6280,
    0,                                  /* Mitsubishi 740 */
    (const InsTable*) &InsTab4510,
};
const InsTable* InsTab = (const InsTable*) &InsTab6502;

/* Table to build the effective 65xx opcode from a base opcode and an
** addressing mode. (The value in the table is ORed with the base opcode)
*/
static unsigned char EATab[12][AM65I_COUNT] = {
    {   /* Table 0 */
        0x00, 0x00, 0x05, 0x0D, 0x0F, 0x15, 0x1D, 0x1F,
        0x00, 0x19, 0x12, 0x00, 0x07, 0x11, 0x17, 0x01,
        0x00, 0x00, 0x00, 0x03, 0x13, 0x09, 0x00, 0x09,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 1 */
        0x08, 0x08, 0x04, 0x0C, 0x00, 0x14, 0x1C, 0x00,
        0x14, 0x1C, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x00
    },
    {   /* Table 2 */
        0x00, 0x00, 0x24, 0x2C, 0x0F, 0x34, 0x3C, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 3 */
        0x3A, 0x3A, 0xC6, 0xCE, 0x00, 0xD6, 0xDE, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 4 */
        0x1A, 0x1A, 0xE6, 0xEE, 0x00, 0xF6, 0xFE, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 5 */
        0x00, 0x00, 0x60, 0x98, 0x00, 0x70, 0x9E, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 6 */
        0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
        0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x90, 0x00
    },
    {   /* Table 7 (Subroutine opcodes) */
        0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
        0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 8 */
        0x00, 0x40, 0x01, 0x41, 0x00, 0x09, 0x49, 0x00,
        0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 9 */
        0x00, 0x00, 0x00, 0x10, 0x00, 0x20, 0x30, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 10 (NOPs) */
        0xea, 0x00, 0x04, 0x0c, 0x00, 0x14, 0x1c, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
        0x00, 0x00, 0x00, 0x00
    },
    {   /* Table 11 (LAX) */
        0x08, 0x08, 0x04, 0x0C, 0x00, 0x14, 0x1C, 0x00,
        0x14, 0x1C, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
        0x00, 0x00, 0x80, 0x00
    },
};

/* Table to build the effective SWEET16 opcode from a base opcode and an
** addressing mode.
*/
static unsigned char Sweet16EATab[2][AMSW16I_COUNT] = {
    {   /* Table 0 */
        0x00, 0x00, 0x00, 0x00, 0x00,
    },
    {   /* Table 1 */
        0x00, 0x00, 0x00, 0x40, 0x20,
    },
};

/* Table that encodes the additional bytes for each 65xx instruction */
unsigned char ExtBytes[AM65I_COUNT] = {
    0,          /* Implicit */
    0,          /* Accu */
    1,          /* Direct */
    2,          /* Absolute */
    3,          /* Absolute long */
    1,          /* Direct,X */
    2,          /* Absolute,X */
    3,          /* Absolute long,X */
    1,          /* Direct,Y */
    2,          /* Absolute,Y */
    1,          /* (Direct) */
    2,          /* (Absolute) */
    1,          /* [Direct] */
    1,          /* (Direct),Y */
    1,          /* [Direct],Y */
    1,          /* (Direct,X) */
    2,          /* (Absolute,X) */
    1,          /* Relative short */
    2,          /* Relative long */
    1,          /* r,s */
    1,          /* (r,s),y */
    1,          /* Immidiate accu */
    1,          /* Immidiate index */
    1,          /* Immidiate byte */
    2,          /* Blockmove (65816) */
    7,          /* Block transfer (HuC6280) */
    2,          /* Absolute Indirect long */
    2,          /* Immidiate word */
};

/* Table that encodes the additional bytes for each SWEET16 instruction */
static unsigned char Sweet16ExtBytes[AMSW16I_COUNT] = {
    0,          /* AMSW16_IMP */
    1,          /* AMSW16_BRA */
    2,          /* AMSW16_IMM */
    0,          /* AMSW16_IND */
    0,          /* AMSW16_REG */
};



/*****************************************************************************/
/*                   Handler functions for 6502 derivates                    */
/*****************************************************************************/



static int EvalEA (const InsDesc* Ins, EffAddr* A)
/* Evaluate the effective address. All fields in A will be valid after calling
** this function. The function returns true on success and false on errors.
*/
{
    /* Get the set of possible addressing modes */
    GetEA (A);

    /* From the possible addressing modes, remove the ones that are invalid
    ** for this instruction or CPU.
    */
    A->AddrModeSet &= Ins->AddrMode;

    /* If we have an expression, check it and remove any addressing modes that
    ** are too small for the expression size. Because we have to study the
    ** expression anyway, do also replace it by a simpler one if possible.
    */
    if (A->Expr) {
        ExprDesc ED;
        ED_Init (&ED);

        /* Study the expression */
        StudyExpr (A->Expr, &ED);

        /* Simplify it if possible */
        A->Expr = SimplifyExpr (A->Expr, &ED);

        if (ED.AddrSize == ADDR_SIZE_DEFAULT) {
            /* We don't know how big the expression is. If the instruction
            ** allows just one addressing mode, assume this as address size
            ** for the expression. Otherwise assume the default address size
            ** for data.
            */
            if ((A->AddrModeSet & ~AM65_ALL_ZP) == 0) {
                ED.AddrSize = ADDR_SIZE_ZP;
            } else if ((A->AddrModeSet & ~AM65_ALL_ABS) == 0) {
                ED.AddrSize = ADDR_SIZE_ABS;
            } else if ((A->AddrModeSet & ~AM65_ALL_FAR) == 0) {
                ED.AddrSize = ADDR_SIZE_FAR;
            } else {
                ED.AddrSize = DataAddrSize;
                /* If the default address size of the data segment is unequal
                ** to zero page addressing, but zero page addressing is
                ** allowed by the instruction, mark all symbols in the
                ** expression tree. This mark will be checked at end of
                ** assembly, and a warning is issued, if a zero page symbol
                ** was guessed wrong here.
                */
                if (ED.AddrSize > ADDR_SIZE_ZP && (A->AddrModeSet & AM65_SET_ZP)) {
                    ExprGuessedAddrSize (A->Expr, ADDR_SIZE_ZP);
                }
            }
        }

        /* Check the size */
        switch (ED.AddrSize) {

            case ADDR_SIZE_ABS:
                A->AddrModeSet &= ~AM65_SET_ZP;
                break;

            case ADDR_SIZE_FAR:
                A->AddrModeSet &= ~(AM65_SET_ZP | AM65_SET_ABS);
                break;
        }

        /* Free any resource associated with the expression desc */
        ED_Done (&ED);
    }

    /* Check if we have any adressing modes left */
    if (A->AddrModeSet == 0) {
        Error ("Illegal addressing mode");
        return 0;
    }
    A->AddrMode    = BitFind (A->AddrModeSet);
    A->AddrModeBit = (0x01UL << A->AddrMode);

    /* If the instruction has a one byte operand and immediate addressing is
    ** allowed but not used, check for an operand expression in the form
    ** <label or >label, where label is a far or absolute label. If found,
    ** emit a warning. This warning protects against a typo, where the '#'
    ** for the immediate operand is omitted.
    */
    if (A->Expr && (Ins->AddrMode & AM65_ALL_IMM)                &&
        (A->AddrModeSet & (AM65_DIR | AM65_ABS | AM65_ABS_LONG)) &&
        ExtBytes[A->AddrMode] == 1) {

        /* Found, check the expression */
        ExprNode* Left = A->Expr->Left;
        if ((A->Expr->Op == EXPR_BYTE0 || A->Expr->Op == EXPR_BYTE1) &&
            Left->Op == EXPR_SYMBOL                                  &&
            GetSymAddrSize (Left->V.Sym) != ADDR_SIZE_ZP             &&
            !(A->Flags & EFFADDR_OVERRIDE_ZP)) {

            /* Output a warning */
            Warning (1, "Suspicious address expression");
        }
    }

    /* Build the opcode */
    A->Opcode = Ins->BaseCode | EATab[Ins->ExtCode][A->AddrMode];

    /* If feature force_range is active, and we have immediate addressing mode,
    ** limit the expression to the maximum possible value.
    */
    if (A->AddrMode == AM65I_IMM_ACCU || A->AddrMode == AM65I_IMM_INDEX ||
        A->AddrMode == AM65I_IMM_IMPLICIT || A->AddrMode == AM65I_IMM_IMPLICIT_WORD) {
        if (ForceRange && A->Expr) {
            A->Expr = MakeBoundedExpr (A->Expr, ExtBytes[A->AddrMode]);
        }
    }

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
            if (CPU == CPU_65816 && (A->AddrModeBit & (AM65_ABS | AM65_ABS_X | AM65_ABS_Y | AM65_ABS_X_IND))) {
                /* This is a 16 bit mode that uses an address. If in 65816,
                ** mode, force this address into 16 bit range to allow
                ** addressing inside a 64K segment.
                */
                Emit2 (A->Opcode, GenNearAddrExpr (A->Expr));
            } else {
                Emit2 (A->Opcode, A->Expr);
            }
            break;

        case 3:
            /* Far argument */
            Emit3 (A->Opcode, A->Expr);
            break;

        default:
            Internal ("Invalid operand byte count: %u", ExtBytes[A->AddrMode]);

    }
}



static long PutImmed8 (const InsDesc* Ins)
/* Parse and emit an immediate 8 bit instruction. Return the value of the
** operand if it's available and const.
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
    if (A.Expr) {
        (void) IsConstExpr (A.Expr, &Val);
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
    EmitPCRel (Ins->BaseCode, GenBranchExpr (2), 1);
}



static void PutPCRel16 (const InsDesc* Ins)
/* Handle branches with an 16 bit distance and PER */
{
    EmitPCRel (Ins->BaseCode, GenBranchExpr (3), 2);
}



static void PutPCRel4510 (const InsDesc* Ins)
/* Handle branches with a 16 bit distance */
{
    /* 16 bit branch opcode is 8 bit branch opcode or'ed with 0x03 */
    EmitPCRel (Ins->BaseCode, GenBranchExpr (2), 2);
}



static void PutBlockMove (const InsDesc* Ins)
/* Handle the blockmove instructions (65816) */
{
    ExprNode* Arg1;
    ExprNode* Arg2;

    Emit0 (Ins->BaseCode);

    if (CurTok.Tok == TOK_HASH) {
        /* The operand is a bank-byte expression. */
        NextTok ();
        Arg1 = Expression ();
    } else {
        /* The operand is a far-address expression.
        ** Use only its bank part.
        */
        Arg1 = FuncBankByte ();
    }
    ConsumeComma ();

    if (CurTok.Tok == TOK_HASH) {
        NextTok ();
        Arg2 = Expression ();
    } else {
        Arg2 = FuncBankByte ();
    }

    /* The operands are written in Assembly code as source, destination;
    ** but, they're assembled as <destination> <source>.
    */
    EmitByte (Arg2);
    EmitByte (Arg1);
}



static void PutBlockTransfer (const InsDesc* Ins)
/* Handle the block transfer instructions (HuC6280) */
{
    Emit0 (Ins->BaseCode);
    EmitWord (Expression ());
    ConsumeComma ();
    EmitWord (Expression ());
    ConsumeComma ();
    EmitWord (Expression ());
}



static void PutBitBranch (const InsDesc* Ins)
/* Handle 65C02 branch on bit condition */
{
    Emit0 (Ins->BaseCode);
    EmitByte (Expression ());
    ConsumeComma ();
    EmitSigned (GenBranchExpr (1), 1);
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
            Warning (1, "Cannot track processor status byte");
        } else {
            if (Val & 0x10) {
                /* Index registers to 16 bit */
                ExtBytes[AM65I_IMM_INDEX] = 2;
            }
            if (Val & 0x20) {
                /* Accu to 16 bit */
                ExtBytes[AM65I_IMM_ACCU] = 2;
            }
        }
    }
}



static void PutSEP (const InsDesc* Ins)
/* Emit a SEP instruction (65816), track register sizes */
{
    /* Use the generic handler */
    long Val = PutImmed8 (Ins);

    /* We track the status only for the 816 CPU and in smart mode */
    if (CPU == CPU_65816 && SmartMode) {

        /* Check the range for Val. */
        if (Val < 0) {
            /* We had an error */
            Warning (1, "Cannot track processor status byte");
        } else {
            if (Val & 0x10) {
                /* Index registers to 8 bit */
                ExtBytes[AM65I_IMM_INDEX] = 1;
            }
            if (Val & 0x20) {
                /* Accu to 8 bit */
                ExtBytes[AM65I_IMM_ACCU] = 1;
            }
        }
    }
}



static void PutTAMn (const InsDesc* Ins)
/* Emit a TAMn instruction (HuC6280). Because this is a two-byte instruction with
** implicit addressing mode, the opcode byte in the table is actually the
** second operand byte. The TAM instruction is the more generic form, it takes
** an immediate argument.
*/
{
    /* Emit the TAM opcode itself */
    Emit0 (0x53);

    /* Emit the argument, which is the opcode from the table */
    Emit0 (Ins->BaseCode);
}



static void PutTMA (const InsDesc* Ins)
/* Emit a TMA instruction (HuC6280) with an immediate argument. Only one bit
** in the argument byte may be set.
*/
{
    /* Use the generic handler */
    long Val = PutImmed8 (Ins);

    /* Check the range for Val. */
    if (Val < 0) {
        /* We had an error */
        Warning (1, "Cannot check argument of TMA instruction");
    } else {
        /* Make sure just one bit is set */
        if ((Val & (Val - 1)) != 0) {
            Error ("Argument of TMA must be a power of two");
        }
    }
}



static void PutTMAn (const InsDesc* Ins)
/* Emit a TMAn instruction (HuC6280). Because this is a two-byte instruction with
** implicit addressing mode, the opcode byte in the table is actually the
** second operand byte. The TMA instruction is the more generic form, it takes
** an immediate argument.
*/
{
    /* Emit the TMA opcode itself */
    Emit0 (0x43);

    /* Emit the argument, which is the opcode from the table */
    Emit0 (Ins->BaseCode);
}



static void PutTST (const InsDesc* Ins)
/* Emit a TST instruction (HuC6280). */
{
    ExprNode* Arg1;
    EffAddr   A;

    /* The first argument is always an immediate byte */
    if (CurTok.Tok != TOK_HASH) {
        ErrorSkip ("Invalid addressing mode");
        return;
    }
    NextTok ();
    Arg1 = Expression ();

    /* Second argument follows */
    ConsumeComma ();

    /* For the second argument, we use the standard function */
    if (EvalEA (Ins, &A)) {

        /* No error, output code */
        Emit1 (A.Opcode, Arg1);

        /* Check how many extension bytes are needed and output the instruction */
        switch (ExtBytes[A.AddrMode]) {

            case 1:
                EmitByte (A.Expr);
                break;

            case 2:
                EmitWord (A.Expr);
                break;
        }
    }
}



static void PutJMP (const InsDesc* Ins)
/* Handle the jump instruction for the 6502. Problem is that these chips have
** a bug: If the address crosses a page, the upper byte gets not corrected and
** the instruction will fail. The PutJmp function will add a linker assertion
** to check for this case and is otherwise identical to PutAll.
*/
{
    EffAddr A;

    /* Evaluate the addressing mode used */
    if (EvalEA (Ins, &A)) {

        /* Check for indirect addressing */
        if ((A.AddrModeBit & AM65_ABS_IND) && (CPU < CPU_65SC02) && (RelaxChecks == 0)) {

            /* Compare the low byte of the expression to 0xFF to check for
            ** a page cross. Be sure to use a copy of the expression otherwise
            ** things will go weird later. This only affects the 6502 CPU,
            ** and was corrected in 65C02 and later CPUs in this family.
            */
            ExprNode* E = GenNE (GenByteExpr (CloneExpr (A.Expr)), 0xFF);

            /* Generate the message */
            unsigned Msg = GetStringId ("\"jmp (abs)\" across page border");

            /* Generate the assertion */
            AddAssertion (E, ASSERT_ACT_ERROR, Msg);
        }

        /* No error, output code */
        EmitCode (&A);
    }
}



static void PutJMP816 (const InsDesc* Ins)
/* Handle the JMP instruction for the 816.
** Allowing the long_jsr_jmp_rts feature to permit a long JMP.
** Note that JMP [abs] and JML [abs] are always both permitted for instruction $DC,
** because the [] notation for long indirection makes the generated instruction unambiguous.
*/
{
    if (LongJsrJmpRts) {
        PutJMP (Ins);
    } else {
        InsDesc InsAbs = *Ins;
        InsAbs.AddrMode &= ~(AM65_ABS_LONG);
        PutJMP (&InsAbs);
    }
}



static void PutJSR816 (const InsDesc* Ins)
/* Handle the JSR instruction for the 816.
** Allowing the long_jsr_jmp_rts feature to permit a long JSR.
*/
{
    if (LongJsrJmpRts) {
        PutAll (Ins);
    } else {
        InsDesc InsAbs = *Ins;
        InsAbs.AddrMode &= ~(AM65_ABS_LONG);
        PutJMP (&InsAbs);
    }
}



static void PutRTS (const InsDesc* Ins attribute ((unused)))
/* Handle the RTS instruction for the 816. In smart mode emit a RTL opcode if
** the enclosing scope is FAR, but only if the long_jsr_jmp_rts feature applies.
*/
{
    if (LongJsrJmpRts && SmartMode && CurrentScope->AddrSize == ADDR_SIZE_FAR) {
        Emit0 (0x6B);       /* RTL */
    } else {
        Emit0 (0x60);       /* RTS */
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



static void Put4510 (const InsDesc* Ins)
/* Handle all other instructions, with modifications for 4510 */
{
    /* The 4510 uses all 256 possible opcodes, so the last ones were crammed
    ** in where an opcode was still undefined. As a result, some of those
    ** don't follow any rules for encoding the addressmodes. So the EATab
    ** approach does not work always. In this function, the wrongly calculated
    ** opcode is replaced by the correct one "on the fly". Suggestions for a
    ** better approach are welcome.
    **
    ** These are:
    ** $47 -> $44 : ASR $12
    ** $57 -> $54 : ASR $12,X
    ** $93 -> $82 : STA ($12,SP),Y
    ** $9c -> $8b : STY $1234,X
    ** $9e -> $9b : STX $1234,Y
    ** $af -> $ab : LDZ $1234
    ** $bf -> $bb : LDZ $1234,X
    ** $b3 -> $e2 : LDA ($12,SP),Y
    ** $d0 -> $c2 : CPZ #$00
    ** $fc -> $23 : JSR ($1234,X)
    */
    EffAddr A;

    /* Evaluate the addressing mode used */
    if (EvalEA (Ins, &A)) {
        switch (A.Opcode) {
            case 0x47: A.Opcode = 0x44; break;
            case 0x57: A.Opcode = 0x54; break;
            case 0x93: A.Opcode = 0x82; break;
            case 0x9C: A.Opcode = 0x8B; break;
            case 0x9E: A.Opcode = 0x9B; break;
            case 0xAF: A.Opcode = 0xAB; break;
            case 0xBF: A.Opcode = 0xBB; break;
            case 0xB3: A.Opcode = 0xE2; break;
            case 0xD0: A.Opcode = 0xC2; break;
            case 0xFC: A.Opcode = 0x23; break;
            default: /* Keep opcode as it is */ break;
        }

        /* No error, output code */
        EmitCode (&A);
    }
}



/*****************************************************************************/
/*                       Handler functions for SWEET16                       */
/*****************************************************************************/



static void PutSweet16 (const InsDesc* Ins)
/* Handle a generic sweet16 instruction */
{
    EffAddr A;

    /* Evaluate the addressing mode used */
    GetSweet16EA (&A);

    /* From the possible addressing modes, remove the ones that are invalid
    ** for this instruction or CPU.
    */
    A.AddrModeSet &= Ins->AddrMode;

    /* Check if we have any adressing modes left */
    if (A.AddrModeSet == 0) {
        Error ("Illegal addressing mode");
        return;
    }
    A.AddrMode    = BitFind (A.AddrModeSet);
    A.AddrModeBit = (0x01UL << A.AddrMode);

    /* Build the opcode */
    A.Opcode = Ins->BaseCode | Sweet16EATab[Ins->ExtCode][A.AddrMode] | A.Reg;

    /* Check how many extension bytes are needed and output the instruction */
    switch (Sweet16ExtBytes[A.AddrMode]) {

        case 0:
            Emit0 (A.Opcode);
            break;

        case 1:
            Emit1 (A.Opcode, A.Expr);
            break;

        case 2:
            Emit2 (A.Opcode, A.Expr);
            break;

        default:
            Internal ("Invalid operand byte count: %u", Sweet16ExtBytes[A.AddrMode]);

    }
}



static void PutSweet16Branch (const InsDesc* Ins)
/* Handle a sweet16 branch instruction */
{
    EmitPCRel (Ins->BaseCode, GenBranchExpr (2), 1);
}



/*****************************************************************************/
/*                                   Code                                    */
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
    if (NewCPU != CPU_UNKNOWN && InsTabs[NewCPU]) {
        CPU = NewCPU;
        InsTab = InsTabs[CPU];
    } else {
        Error ("CPU not supported");
    }
}



cpu_t GetCPU (void)
/* Return the current CPU */
{
    return CPU;
}



int FindInstruction (const StrBuf* Ident)
/* Check if Ident is a valid mnemonic. If so, return the index in the
** instruction table. If not, return -1.
*/
{
    unsigned I;
    const InsDesc* ID;
    char Key[sizeof (ID->Mnemonic)];

    /* Shortcut for the "none" CPU: If there are no instructions to search
    ** for, bail out early.
    */
    if (InsTab->Count == 0) {
        /* Not found */
        return -1;
    }

    /* Make a copy, and uppercase that copy */
    I = 0;
    while (I < SB_GetLen (Ident)) {
        /* If the identifier is longer than the longest mnemonic, it cannot
        ** be one.
        */
        if (I >= sizeof (Key) - 1) {
            /* Not found, no need for further action */
            return -1;
        }
        Key[I] = toupper ((unsigned char)SB_AtUnchecked (Ident, I));
        ++I;
    }
    Key[I] = '\0';

    /* Search for the key */
    ID = bsearch (Key, InsTab->Ins, InsTab->Count, sizeof (InsDesc), CmpName);
    if (ID == 0) {
        /* Not found */
        return -1;
    } else {
        /* Found, return the entry */
        return ID - InsTab->Ins;
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
