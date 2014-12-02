/*****************************************************************************/
/*                                                                           */
/*                                 opc6502.c                                 */
/*                                                                           */
/*             6502 opcode description table with NMOS illegals              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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



/* da65 */
#include "handler.h"
#include "opc6502x.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Descriptions for all opcodes. Base table from opc6502.c with illegal
** opcodes from http://www.oxyron.de/html/opcodes02.html
*/
const OpcDesc OpcTable_6502X[256] = {
    {   "brk",  1,  flNone,                   OH_Implicit              }, /* $00 */
    {   "ora",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $01 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $02 */
    {   "slo",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $03 */
    {   "nop",  2,  flUseLabel,               OH_Direct                }, /* $04 */
    {   "ora",  2,  flUseLabel,               OH_Direct                }, /* $05 */
    {   "asl",  2,  flUseLabel,               OH_Direct                }, /* $06 */
    {   "slo",  2,  flUseLabel,               OH_Direct                }, /* $07 */
    {   "php",  1,  flNone,                   OH_Implicit              }, /* $08 */
    {   "ora",  2,  flNone,                   OH_Immediate             }, /* $09 */
    {   "asl",  1,  flNone,                   OH_Accumulator           }, /* $0a */
    {   "anc",  2,  flNone,                   OH_Immediate             }, /* $0b */
    {   "nop",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0c */
    {   "ora",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0d */
    {   "asl",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0e */
    {   "slo",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0f */
    {   "bpl",  2,  flLabel,                  OH_Relative              }, /* $10 */
    {   "ora",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $11 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $12 */
    {   "slo",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $13 */
    {   "nop",  2,  flUseLabel,               OH_DirectX               }, /* $14 */
    {   "ora",  2,  flUseLabel,               OH_DirectX               }, /* $15 */
    {   "asl",  2,  flUseLabel,               OH_DirectX               }, /* $16 */
    {   "slo",  2,  flUseLabel,               OH_DirectX               }, /* $17 */
    {   "clc",  1,  flNone,                   OH_Implicit              }, /* $18 */
    {   "ora",  3,  flUseLabel,               OH_AbsoluteY             }, /* $19 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $1a */
    {   "slo",  3,  flUseLabel,               OH_AbsoluteY             }, /* $1b */
    {   "nop",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $1c */
    {   "ora",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $1d */
    {   "asl",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $1e */
    {   "slo",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $1f */
    {   "jsr",  3,  flLabel,                  OH_Absolute              }, /* $20 */
    {   "and",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $21 */
    {   "jam",  1,  flNone,                   OH_Implicit,             }, /* $22 */
    {   "rla",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $23 */
    {   "bit",  2,  flUseLabel,               OH_Direct                }, /* $24 */
    {   "and",  2,  flUseLabel,               OH_Direct                }, /* $25 */
    {   "rol",  2,  flUseLabel,               OH_Direct                }, /* $26 */
    {   "rla",  2,  flUseLabel,               OH_Direct                }, /* $27 */
    {   "plp",  1,  flNone,                   OH_Implicit              }, /* $28 */
    {   "and",  2,  flNone,                   OH_Immediate             }, /* $29 */
    {   "rol",  1,  flNone,                   OH_Accumulator           }, /* $2a */
    {   "anc",  2,  flNone,                   OH_Immediate             }, /* $2b */
    {   "bit",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2c */
    {   "and",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2d */
    {   "rol",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2e */
    {   "rla",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2f */
    {   "bmi",  2,  flLabel,                  OH_Relative              }, /* $30 */
    {   "and",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $31 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $32 */
    {   "rla",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $33 */
    {   "nop",  2,  flUseLabel,               OH_DirectX               }, /* $34 */
    {   "and",  2,  flUseLabel,               OH_DirectX               }, /* $35 */
    {   "rol",  2,  flUseLabel,               OH_DirectX               }, /* $36 */
    {   "rla",  2,  flUseLabel,               OH_DirectX               }, /* $37 */
    {   "sec",  1,  flNone,                   OH_Implicit              }, /* $38 */
    {   "and",  3,  flUseLabel,               OH_AbsoluteY             }, /* $39 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $3a */
    {   "rla",  3,  flUseLabel,               OH_AbsoluteY             }, /* $3b */
    {   "nop",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $3c */
    {   "and",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $3d */
    {   "rol",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $3e */
    {   "rla",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $3f */
    {   "rti",  1,  flNone,                   OH_Rts                   }, /* $40 */
    {   "eor",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $41 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $42 */
    {   "sre",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $43 */
    {   "nop",  2,  flUseLabel,               OH_Direct                }, /* $44 */
    {   "eor",  2,  flUseLabel,               OH_Direct                }, /* $45 */
    {   "lsr",  2,  flUseLabel,               OH_Direct                }, /* $46 */
    {   "sre",  2,  flUseLabel,               OH_Direct                }, /* $47 */
    {   "pha",  1,  flNone,                   OH_Implicit              }, /* $48 */
    {   "eor",  2,  flNone,                   OH_Immediate             }, /* $49 */
    {   "lsr",  1,  flNone,                   OH_Accumulator           }, /* $4a */
    {   "alr",  2,  flNone,                   OH_Immediate             }, /* $4b */
    {   "jmp",  3,  flLabel,                  OH_JmpAbsolute           }, /* $4c */
    {   "eor",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $4d */
    {   "lsr",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $4e */
    {   "sre",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $4f */
    {   "bvc",  2,  flLabel,                  OH_Relative              }, /* $50 */
    {   "eor",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $51 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $52 */
    {   "sre",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $53 */
    {   "nop",  2,  flUseLabel,               OH_DirectX               }, /* $54 */
    {   "eor",  2,  flUseLabel,               OH_DirectX               }, /* $55 */
    {   "lsr",  2,  flUseLabel,               OH_DirectX               }, /* $56 */
    {   "sre",  2,  flUseLabel,               OH_DirectX               }, /* $57 */
    {   "cli",  1,  flNone,                   OH_Implicit              }, /* $58 */
    {   "eor",  3,  flUseLabel,               OH_AbsoluteY             }, /* $59 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $5a */
    {   "sre",  3,  flUseLabel,               OH_AbsoluteY             }, /* $5b */
    {   "nop",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $5c */
    {   "eor",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $5d */
    {   "lsr",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $5e */
    {   "sre",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $5f */
    {   "rts",  1,  flNone,                   OH_Rts                   }, /* $60 */
    {   "adc",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $61 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $62 */
    {   "rra",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $63 */
    {   "nop",  2,  flUseLabel,               OH_Direct                }, /* $64 */
    {   "adc",  2,  flUseLabel,               OH_Direct                }, /* $65 */
    {   "ror",  2,  flUseLabel,               OH_Direct                }, /* $66 */
    {   "rra",  2,  flUseLabel,               OH_Direct                }, /* $67 */
    {   "pla",  1,  flNone,                   OH_Implicit              }, /* $68 */
    {   "adc",  2,  flNone,                   OH_Immediate             }, /* $69 */
    {   "ror",  1,  flNone,                   OH_Accumulator           }, /* $6a */
    {   "arr",  2,  flNone,                   OH_Immediate             }, /* $6b */
    {   "jmp",  3,  flLabel,                  OH_JmpAbsoluteIndirect   }, /* $6c */
    {   "adc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $6d */
    {   "ror",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $6e */
    {   "rra",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $6f */
    {   "bvs",  2,  flLabel,                  OH_Relative              }, /* $70 */
    {   "adc",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $71 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $72 */
    {   "rra",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $73 */
    {   "nop",  2,  flUseLabel,               OH_DirectX               }, /* $74 */
    {   "adc",  2,  flUseLabel,               OH_DirectX               }, /* $75 */
    {   "ror",  2,  flUseLabel,               OH_DirectX               }, /* $76 */
    {   "rra",  2,  flUseLabel,               OH_DirectX               }, /* $77 */
    {   "sei",  1,  flNone,                   OH_Implicit              }, /* $78 */
    {   "adc",  3,  flUseLabel,               OH_AbsoluteY             }, /* $79 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $7a */
    {   "rra",  3,  flUseLabel,               OH_AbsoluteY             }, /* $7b */
    {   "nop",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $7c */
    {   "adc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $7d */
    {   "ror",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $7e */
    {   "rra",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $7f */
    {   "nop",  2,  flNone,                   OH_Immediate             }, /* $80 */
    {   "sta",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $81 */
    {   "nop",  2,  flNone,                   OH_Immediate             }, /* $82 */
    {   "sax",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $83 */
    {   "sty",  2,  flUseLabel,               OH_Direct                }, /* $84 */
    {   "sta",  2,  flUseLabel,               OH_Direct                }, /* $85 */
    {   "stx",  2,  flUseLabel,               OH_Direct                }, /* $86 */
    {   "sax",  2,  flUseLabel,               OH_Direct                }, /* $87 */
    {   "dey",  1,  flNone,                   OH_Implicit              }, /* $88 */
    {   "nop",  2,  flNone,                   OH_Immediate             }, /* $89 */
    {   "txa",  1,  flNone,                   OH_Implicit              }, /* $8a */
    {   "xaa",  2,  flNone,                   OH_Immediate             }, /* $8b */
    {   "sty",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8c */
    {   "sta",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8d */
    {   "stx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8e */
    {   "sax",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8f */
    {   "bcc",  2,  flLabel,                  OH_Relative              }, /* $90 */
    {   "sta",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $91 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $92 */
    {   "ahx",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $93 */
    {   "sty",  2,  flUseLabel,               OH_DirectX               }, /* $94 */
    {   "sta",  2,  flUseLabel,               OH_DirectX               }, /* $95 */
    {   "stx",  2,  flUseLabel,               OH_DirectY               }, /* $96 */
    {   "sax",  2,  flUseLabel,               OH_DirectY               }, /* $97 */
    {   "tya",  1,  flNone,                   OH_Implicit              }, /* $98 */
    {   "sta",  3,  flUseLabel,               OH_AbsoluteY             }, /* $99 */
    {   "txs",  1,  flNone,                   OH_Implicit              }, /* $9a */
    {   "tas",  3,  flUseLabel,               OH_AbsoluteY             }, /* $9b */
    {   "shy",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $9c */
    {   "sta",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $9d */
    {   "shx",  3,  flUseLabel,               OH_AbsoluteY             }, /* $9e */
    {   "ahx",  3,  flUseLabel,               OH_AbsoluteY             }, /* $9f */
    {   "ldy",  2,  flNone,                   OH_Immediate             }, /* $a0 */
    {   "lda",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $a1 */
    {   "ldx",  2,  flNone,                   OH_Immediate             }, /* $a2 */
    {   "lax",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $a3 */
    {   "ldy",  2,  flUseLabel,               OH_Direct                }, /* $a4 */
    {   "lda",  2,  flUseLabel,               OH_Direct                }, /* $a5 */
    {   "ldx",  2,  flUseLabel,               OH_Direct                }, /* $a6 */
    {   "lax",  2,  flUseLabel,               OH_Direct                }, /* $a7 */
    {   "tay",  1,  flNone,                   OH_Implicit              }, /* $a8 */
    {   "lda",  2,  flNone,                   OH_Immediate             }, /* $a9 */
    {   "tax",  1,  flNone,                   OH_Implicit              }, /* $aa */
    {   "lax",  2,  flNone,                   OH_Immediate             }, /* $ab */
    {   "ldy",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ac */
    {   "lda",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ad */
    {   "ldx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ae */
    {   "lax",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $af */
    {   "bcs",  2,  flLabel,                  OH_Relative              }, /* $b0 */
    {   "lda",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $b1 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $b2 */
    {   "lax",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $b3 */
    {   "ldy",  2,  flUseLabel,               OH_DirectX               }, /* $b4 */
    {   "lda",  2,  flUseLabel,               OH_DirectX               }, /* $b5 */
    {   "ldx",  2,  flUseLabel,               OH_DirectY               }, /* $b6 */
    {   "lax",  2,  flUseLabel,               OH_DirectY               }, /* $b7 */
    {   "clv",  1,  flNone,                   OH_Implicit              }, /* $b8 */
    {   "lda",  3,  flUseLabel,               OH_AbsoluteY             }, /* $b9 */
    {   "tsx",  1,  flNone,                   OH_Implicit              }, /* $ba */
    {   "las",  3,  flUseLabel,               OH_AbsoluteY             }, /* $bb */
    {   "ldy",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $bc */
    {   "lda",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $bd */
    {   "ldx",  3,  flUseLabel|flAbsOverride, OH_AbsoluteY             }, /* $be */
    {   "lax",  3,  flUseLabel|flAbsOverride, OH_AbsoluteY             }, /* $bf */
    {   "cpy",  2,  flNone,                   OH_Immediate             }, /* $c0 */
    {   "cmp",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $c1 */
    {   "nop",  2,  flNone,                   OH_Immediate             }, /* $c2 */
    {   "dcp",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $c3 */
    {   "cpy",  2,  flUseLabel,               OH_Direct                }, /* $c4 */
    {   "cmp",  2,  flUseLabel,               OH_Direct                }, /* $c5 */
    {   "dec",  2,  flUseLabel,               OH_Direct                }, /* $c6 */
    {   "dcp",  2,  flUseLabel,               OH_Direct                }, /* $c7 */
    {   "iny",  1,  flNone,                   OH_Implicit              }, /* $c8 */
    {   "cmp",  2,  flNone,                   OH_Immediate             }, /* $c9 */
    {   "dex",  1,  flNone,                   OH_Implicit              }, /* $ca */
    {   "axs",  2,  flNone,                   OH_Immediate             }, /* $cb */
    {   "cpy",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $cc */
    {   "cmp",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $cd */
    {   "dec",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ce */
    {   "dcp",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $cf */
    {   "bne",  2,  flLabel,                  OH_Relative              }, /* $d0 */
    {   "cmp",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $d1 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $d2 */
    {   "dcp",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $d3 */
    {   "nop",  2,  flUseLabel,               OH_DirectX               }, /* $d4 */
    {   "cmp",  2,  flUseLabel,               OH_DirectX               }, /* $d5 */
    {   "dec",  2,  flUseLabel,               OH_DirectX               }, /* $d6 */
    {   "dcp",  2,  flUseLabel,               OH_DirectX               }, /* $d7 */
    {   "cld",  1,  flNone,                   OH_Implicit              }, /* $d8 */
    {   "cmp",  3,  flUseLabel,               OH_AbsoluteY             }, /* $d9 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $da */
    {   "dcp",  3,  flUseLabel,               OH_AbsoluteY             }, /* $db */
    {   "nop",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $dc */
    {   "cmp",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $dd */
    {   "dec",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $de */
    {   "dcp",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $df */
    {   "cpx",  2,  flNone,                   OH_Immediate             }, /* $e0 */
    {   "sbc",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $e1 */
    {   "nop",  2,  flNone,                   OH_Immediate             }, /* $e2 */
    {   "isc",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $e3 */
    {   "cpx",  2,  flUseLabel,               OH_Direct                }, /* $e4 */
    {   "sbc",  2,  flUseLabel,               OH_Direct                }, /* $e5 */
    {   "inc",  2,  flUseLabel,               OH_Direct                }, /* $e6 */
    {   "isc",  2,  flUseLabel,               OH_Direct                }, /* $e7 */
    {   "inx",  1,  flNone,                   OH_Implicit              }, /* $e8 */
    {   "sbc",  2,  flNone,                   OH_Immediate             }, /* $e9 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $ea */
    {   "sbc",  2,  flNone,                   OH_Immediate             }, /* $eb */
    {   "cpx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ec */
    {   "sbc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ed */
    {   "inc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ee */
    {   "isc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ef */
    {   "beq",  2,  flLabel,                  OH_Relative              }, /* $f0 */
    {   "sbc",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $f1 */
    {   "jam",  1,  flNone,                   OH_Implicit              }, /* $f2 */
    {   "isc",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $f3 */
    {   "nop",  2,  flUseLabel,               OH_DirectX               }, /* $f4 */
    {   "sbc",  2,  flUseLabel,               OH_DirectX               }, /* $f5 */
    {   "inc",  2,  flUseLabel,               OH_DirectX               }, /* $f6 */
    {   "isc",  2,  flUseLabel,               OH_DirectX               }, /* $f7 */
    {   "sed",  1,  flNone,                   OH_Implicit              }, /* $f8 */
    {   "sbc",  3,  flUseLabel,               OH_AbsoluteY             }, /* $f9 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $fa */
    {   "isc",  3,  flUseLabel,               OH_AbsoluteY             }, /* $fb */
    {   "nop",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $fc */
    {   "sbc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $fd */
    {   "inc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $fe */
    {   "isc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $ff */
};
