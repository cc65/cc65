/*****************************************************************************/
/*                                                                           */
/*				  opctable.c				     */
/*                                                                           */
/*		     Disassembler opcode description table		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



/* da65 */
#include "cpu.h"
#include "handler.h"
#include "opctable.h"



/*****************************************************************************/
/*		    		     Data				     */
/*****************************************************************************/



const OpcDesc OpcTable[256] = {
    {   /* $00 */
    	"brk",
    	2,
    	0,
    	CPU_ALL,
	OH_Implicit
    },
    {  /* $01 */
	"ora",
	2,
	lfUseLabel,
	CPU_ALL,
       	OH_DirectXIndirect
    },
    {  /* $02 */
	"cop",
	2,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $03 */
	"ora",
	2,
	0,
	CPU_65816,
	OH_StackRelative
    },
    {  /* $04 */
	"tsb",
	2,
	lfUseLabel,
	CPU_65816,
	OH_Direct
    },
    {  /* $05 */
	"ora",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $06 */
	"asl",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $07 */
	"ora",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLong
    },
    {  /* $08 */
	"php",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $09 */
	"ora",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $0a */
	"asl",
	1,
	0,
	CPU_ALL,
	OH_Accumulator
    },
    {  /* $0b */
	"phd",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $0c */
	"tsb",
	3,
	lfUseLabel,
	CPU_65816,
	OH_Absolute
    },
    {  /* $0d */
	"ora",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $0e */
	"asl",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $0f */
	"ora",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $10 */
	"bpl",
	2,
	lfLabel,
	CPU_ALL,
	OH_Relative
    },
    {  /* $11 */
	"ora",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectIndirectY
    },
    {  /* $12 */
	"ora",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirect
    },
    {  /* $13 */
	"ora",
	2,
	0,
	CPU_65816,
	OH_StackRelativeIndirectY
    },
    {  /* $14 */
	"trb",
	2,
	lfUseLabel,
	CPU_65816,
	OH_Direct
    },
    {  /* $15 */
	"ora",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $16 */
	"asl",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $17 */
	"ora",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLongY
    },
    {  /* $18 */
	"clc",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $19 */
	"ora",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $1a */
	"inc",
	1,
	0,
	CPU_65816,
	OH_Accumulator
    },
    {  /* $1b */
	"tcs",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $1c */
	"trb",
	3,
	lfUseLabel,
	CPU_65816,
	OH_Absolute
    },
    {  /* $1d */
	"ora",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $1e */
	"asl",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $1f */
	"ora",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLongX
    },
    {  /* $20 */
	"jsr",
	3,
	lfLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $21 */
	"and",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectXIndirect
    },
    {  /* $22 */
	"jsl",
	3,
	lfLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $23 */
	"and",
	2,
	0,
	CPU_65816,
	OH_StackRelative
    },
    {  /* $24 */
	"bit",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $25 */
	"and",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $26 */
	"rol",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $27 */
	"and",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLong
    },
    {  /* $28 */
	"plp",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $29 */
	"and",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $2a */
	"rol",
	1,
	0,
	CPU_ALL,
	OH_Accumulator
    },
    {  /* $2b */
	"pld",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $2c */
	"bit",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $2d */
	"and",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $2e */
	"rol",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $2f */
	"and",
	4,
	lfUseLabel,
	CPU_65816,
       	OH_AbsoluteLong
    },
    {  /* $30 */
	"bmi",
	2,
	lfLabel,
	CPU_ALL,
	OH_Relative
    },
    {  /* $31 */
	"and",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectIndirectY
    },
    {  /* $32 */
	"and",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirect
    },
    {  /* $33 */
	"and",
	2,
	0,
	CPU_65816,
	OH_StackRelativeIndirectY
    },
    {  /* $34 */
	"bit",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectX
    },
    {  /* $35 */
	"and",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $36 */
	"rol",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $37 */
	"and",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLongY
    },
    {  /* $38 */
	"sec",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $39 */
	"and",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $3a */
	"dec",
	1,
	0,
	CPU_65816,
	OH_Accumulator
    },
    {  /* $3b */
	"tsc",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $3c */
	"bit",
	3,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteX
    },
    {  /* $3d */
	"and",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $3e */
	"rol",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $3f */
	"and",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLongX
    },
    {  /* $40 */
	"rti",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $41 */
	"eor",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectXIndirect
    },
    {  /* $42 */
	"wdm",
	2,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $43 */
	"eor",
	2,
	0,
	CPU_65816,
	OH_StackRelative
    },
    {  /* $44 */
	"mvp",
	3,
	0,
	CPU_65816,
	OH_BlockMove
    },
    {  /* $45 */
	"eor",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $46 */
	"lsr",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $47 */
	"eor",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLong
    },
    {  /* $48 */
	"pha",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $49 */
	"eor",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $4a */
	"lsr",
	1,
	0,
	CPU_ALL,
	OH_Accumulator
    },
    {  /* $4b */
	"phk",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $4c */
	"jmp",
	3,
	lfLabel,
	CPU_ALL,
       	OH_JmpAbsolute
    },
    {  /* $4d */
	"eor",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $4e */
	"lsr",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $4f */
	"eor",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $50 */
	"bvc",
	2,
	lfLabel,
	CPU_ALL,
	OH_Relative
    },
    {  /* $51 */
	"eor",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectIndirectY
    },
    {  /* $52 */
	"eor",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirect
    },
    {  /* $53 */
	"eor",
	2,
	0,
	CPU_65816,
	OH_StackRelativeIndirectY
    },
    {  /* $54 */
	"mvn",
	3,
	0,
	CPU_65816,
	OH_BlockMove
    },
    {  /* $55 */
	"eor",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $56 */
	"lsr",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $57 */
	"eor",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLongY
    },
    {  /* $58 */
	"cli",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $59 */
	"eor",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $5a */
	"phy",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $5b */
	"tcd",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $5c */
	"jml",
	4,
	lfLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $5d */
	"eor",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $5e */
	"lsr",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $5f */
	"eor",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLongX
    },
    {  /* $60 */
	"rts",
	1,
	0,
	CPU_ALL,
       	OH_Rts
    },
    {  /* $61 */
	"adc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectXIndirect
    },
    {  /* $62 */
	"per",
	3,
	lfLabel,
	CPU_65816,
	OH_RelativeLong
    },
    {  /* $63 */
	"adc",
	2,
	0,
	CPU_65816,
	OH_StackRelative
    },
    {  /* $64 */
	"stz",
	2,
	lfUseLabel,
	CPU_65816,
	OH_Direct
    },
    {  /* $65 */
	"adc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $66 */
	"ror",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $67 */
	"adc",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLong
    },
    {  /* $68 */
	"pla",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $69 */
	"adc",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $6a */
	"ror",
	1,
	0,
	CPU_ALL,
	OH_Accumulator
    },
    {  /* $6b */
	"rtl",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $6c */
	"jmp",
	3,
	lfLabel,
	CPU_ALL,
	OH_AbsoluteIndirect
    },
    {  /* $6d */
	"adc",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $6e */
	"ror",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $6f */
	"adc",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $70 */
	"bvs",
	2,
	lfLabel,
	CPU_ALL,
	OH_Relative
    },
    {  /* $71 */
	"adc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectIndirectY
    },
    {  /* $72 */
	"adc",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirect
    },
    {  /* $73 */
	"adc",
	2,
	0,
	CPU_65816,
	OH_StackRelativeIndirectY
    },
    {  /* $74 */
	"stz",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectX
    },
    {  /* $75 */
	"adc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $76 */
	"ror",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $77 */
	"adc",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLongY
    },
    {  /* $78 */
	"sei",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $79 */
	"adc",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $7a */
	"ply",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $7b */
	"tdc",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $7c */
	"jmp",
	3,
	lfLabel,
	CPU_65816,
	OH_AbsoluteXIndirect
    },
    {  /* $7d */
	"adc",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $7e */
	"ror",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $7f */
	"adc",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLongX
    },
    {  /* $80 */
	"bra",
	2,
	lfLabel,
	CPU_65816,
	OH_Relative
    },
    {  /* $81 */
	"sta",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectXIndirect
    },
    {  /* $82 */
	"brl",
	3,
	lfLabel,
	CPU_65816,
	OH_RelativeLong
    },
    {  /* $83 */
	"sta",
	2,
	0,
	CPU_65816,
	OH_StackRelative
    },
    {  /* $84 */
	"sty",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $85 */
	"sta",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $86 */
	"stx",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $87 */
	"sta",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLong
    },
    {  /* $88 */
	"dey",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $89 */
	"bit",
	2,
	0,
	CPU_65816,
	OH_Immidiate
    },
    {  /* $8a */
	"txa",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $8b */
	"phb",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $8c */
	"sty",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $8d */
	"sta",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $8e */
	"stx",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $8f */
	"sta",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $90 */
	"bcc",
	2,
	lfLabel,
	CPU_ALL,
	OH_Relative
    },
    {  /* $91 */
	"sta",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectIndirectY
    },
    {  /* $92 */
	"sta",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirect
    },
    {  /* $93 */
	"sta",
	2,
	0,
	CPU_65816,
	OH_StackRelativeIndirectY
    },
    {  /* $94 */
	"sty",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $95 */
	"sta",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $96 */
	"stx",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectY
    },
    {  /* $97 */
	"sta",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLongY
    },
    {  /* $98 */
	"tya",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $99 */
	"sta",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $9a */
	"txs",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $9b */
	"txy",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $9c */
	"stz",
	3,
	lfUseLabel,
	CPU_65816,
	OH_Absolute
    },
    {  /* $9d */
	"sta",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $9e */
	"stz",
	3,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteX
    },
    {  /* $9f */
	"sta",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLongX
    },
    {  /* $a0 */
	"ldy",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $a1 */
	"lda",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectXIndirect
    },
    {  /* $a2 */
	"ldx",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $a3 */
	"lda",
	2,
	0,
	CPU_65816,
	OH_StackRelative
    },
    {  /* $a4 */
	"ldy",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $a5 */
	"lda",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $a6 */
	"ldx",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $a7 */
	"lda",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLong
    },
    {  /* $a8 */
	"tay",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $a9 */
	"lda",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $aa */
	"tax",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $ab */
	"plb",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $ac */
	"ldy",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $ad */
	"lda",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $ae */
	"ldx",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $af */
	"lda",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $b0 */
	"bcs",
	2,
	lfLabel,
	CPU_ALL,
	OH_Relative
    },
    {  /* $b1 */
	"lda",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectIndirectY
    },
    {  /* $b2 */
	"lda",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirect
    },
    {  /* $b3 */
	"lda",
	2,
	0,
	CPU_65816,
	OH_StackRelativeIndirectY
    },
    {  /* $b4 */
	"ldy",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $b5 */
	"lda",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $b6 */
	"ldx",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectY
    },
    {  /* $b7 */
	"lda",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLongY
    },
    {  /* $b8 */
	"clv",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $b9 */
	"lda",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $ba */
	"tsx",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $bb */
	"tyx",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $bc */
	"ldy",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $bd */
	"lda",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $be */
	"ldx",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $bf */
	"lda",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLongX
    },
    {  /* $c0 */
	"cpy",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $c1 */
	"cmp",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectXIndirect
    },
    {  /* $c2 */
	"rep",
	2,
	0,
	CPU_65816,
	OH_Immidiate
    },
    {  /* $c3 */
	"cmp",
	2,
	0,
	CPU_65816,
	OH_StackRelative
    },
    {  /* $c4 */
	"cpy",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $c5 */
	"cmp",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $c6 */
	"dec",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $c7 */
	"cmp",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLong
    },
    {  /* $c8 */
	"iny",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $c9 */
	"cmp",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $ca */
	"dex",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $cb */
	"wai",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $cc */
	"cpy",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $cd */
	"cmp",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $ce */
	"dec",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $cf */
	"cmp",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $d0 */
	"bne",
	2,
	lfLabel,
	CPU_ALL,
	OH_Relative
    },
    {  /* $d1 */
	"cmp",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectXIndirect
    },
    {  /* $d2 */
	"cmp",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirect
    },
    {  /* $d3 */
	"cmp",
	2,
	0,
	CPU_65816,
      	OH_StackRelativeIndirectY
    },
    {  /* $d4 */
	"pei",
	2,
	lfUseLabel,
	CPU_65816,
	OH_Direct
    },
    {  /* $d5 */
	"cmp",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $d6 */
	"dec",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $d7 */
	"cmp",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLongY
    },
    {  /* $d8 */
	"cld",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $d9 */
	"cmp",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $da */
	"phx",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $db */
	"stp",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $dc */
	"jml",
	3,
	lfLabel,
	CPU_65816,
	OH_AbsoluteIndirect
    },
    {  /* $dd */
	"cmp",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $de */
	"dec",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $df */
	"cmp",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLongX
    },
    {  /* $e0 */
	"cpx",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $e1 */
	"sbc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectXIndirect
    },
    {  /* $e2 */
	"sep",
	2,
	0,
	CPU_65816,
	OH_Immidiate
    },
    {  /* $e3 */
	"sbc",
	2,
	0,
	CPU_65816,
	OH_StackRelative
    },
    {  /* $e4 */
	"cpx",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $e5 */
	"sbc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $e6 */
	"inc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_Direct
    },
    {  /* $e7 */
	"sbc",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLong
    },
    {  /* $e8 */
	"inx",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $e9 */
	"sbc",
	2,
	0,
	CPU_ALL,
	OH_Immidiate
    },
    {  /* $ea */
	"nop",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $eb */
	"xba",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $ec */
	"cpx",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $ed */
	"sbc",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $ee */
	"inc",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_Absolute
    },
    {  /* $ef */
	"sbc",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLong
    },
    {  /* $f0 */
	"beq",
	2,
	lfLabel,
	CPU_ALL,
	OH_Relative
    },
    {  /* $f1 */
	"sbc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectIndirectY
    },
    {  /* $f2 */
	"sbc",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirect
    },
    {  /* $f3 */
	"sbc",
	2,
	0,
	CPU_65816,
	OH_StackRelativeIndirectY
    },
    {  /* $f4 */
	"pea",
	3,
	lfUseLabel,
	CPU_65816,
	OH_Absolute
    },
    {  /* $f5 */
	"sbc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $f6 */
	"inc",
	2,
	lfUseLabel,
	CPU_ALL,
	OH_DirectX
    },
    {  /* $f7 */
	"sbc",
	2,
	lfUseLabel,
	CPU_65816,
	OH_DirectIndirectLongY
    },
    {  /* $f8 */
	"sed",
	1,
	0,
	CPU_ALL,
	OH_Implicit
    },
    {  /* $f9 */
	"sbc",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteY
    },
    {  /* $fa */
	"plx",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $fb */
	"xce",
	1,
	0,
	CPU_65816,
	OH_Implicit
    },
    {  /* $fc */
	"jsr",
	3,
	lfLabel,
	CPU_65816,
	OH_AbsoluteXIndirect
    },
    {  /* $fd */
	"sbc",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $fe */
	"inc",
	3,
	lfUseLabel,
	CPU_ALL,
	OH_AbsoluteX
    },
    {  /* $ff */
	"sbc",
	4,
	lfUseLabel,
	CPU_65816,
	OH_AbsoluteLongX
    },
};



