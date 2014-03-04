/*****************************************************************************/
/*                                                                           */
/*                                opc65sc02.c                                */
/*                                                                           */
/*                      65SC02 opcode description table                      */
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
#include "opc65sc02.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Descriptions for all opcodes */
const OpcDesc OpcTable_65SC02[256] = {
    {   "brk",  1,  flNone,                   OH_Implicit              }, /* $00 */
    {   "ora",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $01 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $02 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $03 */
    {   "tsb",  2,  flUseLabel,               OH_Direct                }, /* $04 */
    {   "ora",  2,  flUseLabel,               OH_Direct                }, /* $05 */
    {   "asl",  2,  flUseLabel,               OH_Direct                }, /* $06 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $07 */
    {   "php",  1,  flNone,                   OH_Implicit              }, /* $08 */
    {   "ora",  2,  flNone,                   OH_Immediate             }, /* $09 */
    {   "asl",  1,  flNone,                   OH_Accumulator           }, /* $0a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $0b */
    {   "tsb",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0c */
    {   "ora",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0d */
    {   "asl",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $0f */
    {   "bpl",  2,  flLabel,                  OH_Relative              }, /* $10 */
    {   "ora",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $11 */
    {   "ora",  2,  flUseLabel,               OH_DirectIndirect        }, /* $12 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $13 */
    {   "trb",  2,  flUseLabel,               OH_Direct                }, /* $14 */
    {   "ora",  2,  flUseLabel,               OH_DirectX               }, /* $15 */
    {   "asl",  2,  flUseLabel,               OH_DirectX               }, /* $16 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $17 */
    {   "clc",  1,  flNone,                   OH_Implicit              }, /* $18 */
    {   "ora",  3,  flUseLabel,               OH_AbsoluteY             }, /* $19 */
    {   "inc",  1,  flNone,                   OH_Accumulator           }, /* $1a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $1b */
    {   "trb",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $1c */
    {   "ora",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $1d */
    {   "asl",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $1e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $1f */
    {   "jsr",  3,  flLabel,                  OH_Absolute              }, /* $20 */
    {   "and",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $21 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $22 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $23 */
    {   "bit",  2,  flUseLabel,               OH_Direct                }, /* $24 */
    {   "and",  2,  flUseLabel,               OH_Direct                }, /* $25 */
    {   "rol",  2,  flUseLabel,               OH_Direct                }, /* $26 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $27 */
    {   "plp",  1,  flNone,                   OH_Implicit              }, /* $28 */
    {   "and",  2,  flNone,                   OH_Immediate             }, /* $29 */
    {   "rol",  1,  flNone,                   OH_Accumulator           }, /* $2a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $2b */
    {   "bit",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2c */
    {   "and",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2d */
    {   "rol",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $2f */
    {   "bmi",  2,  flLabel,                  OH_Relative              }, /* $30 */
    {   "and",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $31 */
    {   "and",  2,  flUseLabel,               OH_DirectIndirect,       }, /* $32 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $33 */
    {   "bit",  2,  flUseLabel,               OH_DirectX               }, /* $34 */
    {   "and",  2,  flUseLabel,               OH_DirectX               }, /* $35 */
    {   "rol",  2,  flUseLabel,               OH_DirectX               }, /* $36 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $37 */
    {   "sec",  1,  flNone,                   OH_Implicit              }, /* $38 */
    {   "and",  3,  flUseLabel,               OH_AbsoluteY             }, /* $39 */
    {   "dec",  1,  flNone,                   OH_Accumulator           }, /* $3a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $3b */
    {   "bit",  3,  flUseLabel,               OH_AbsoluteX             }, /* $3c */
    {   "and",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $3d */
    {   "rol",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $3e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $3f */
    {   "rti",  1,  flNone,                   OH_Rts                   }, /* $40 */
    {   "eor",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $41 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $42 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $43 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $44 */
    {   "eor",  2,  flUseLabel,               OH_Direct                }, /* $45 */
    {   "lsr",  2,  flUseLabel,               OH_Direct                }, /* $46 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $47 */
    {   "pha",  1,  flNone,                   OH_Implicit              }, /* $48 */
    {   "eor",  2,  flNone,                   OH_Immediate             }, /* $49 */
    {   "lsr",  1,  flNone,                   OH_Accumulator           }, /* $4a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $4b */
    {   "jmp",  3,  flLabel,                  OH_JmpAbsolute           }, /* $4c */
    {   "eor",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $4d */
    {   "lsr",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $4e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $4f */
    {   "bvc",  2,  flLabel,                  OH_Relative              }, /* $50 */
    {   "eor",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $51 */
    {   "eor",  2,  flUseLabel,               OH_DirectIndirect        }, /* $52 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $53 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $54 */
    {   "eor",  2,  flUseLabel,               OH_DirectX               }, /* $55 */
    {   "lsr",  2,  flUseLabel,               OH_DirectX               }, /* $56 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $57 */
    {   "cli",  1,  flNone,                   OH_Implicit              }, /* $58 */
    {   "eor",  3,  flUseLabel,               OH_AbsoluteY             }, /* $59 */
    {   "phy",  1,  flNone,                   OH_Implicit              }, /* $5a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $5b */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $5c */
    {   "eor",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $5d */
    {   "lsr",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $5e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $5f */
    {   "rts",  1,  flNone,                   OH_Rts                   }, /* $60 */
    {   "adc",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $61 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $62 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $63 */
    {   "stz",  2,  flUseLabel,               OH_Direct                }, /* $64 */
    {   "adc",  2,  flUseLabel,               OH_Direct                }, /* $65 */
    {   "ror",  2,  flUseLabel,               OH_Direct                }, /* $66 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $67 */
    {   "pla",  1,  flNone,                   OH_Implicit              }, /* $68 */
    {   "adc",  2,  flNone,                   OH_Immediate             }, /* $69 */
    {   "ror",  1,  flNone,                   OH_Accumulator           }, /* $6a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $6b */
    {   "jmp",  3,  flLabel,                  OH_JmpAbsoluteIndirect   }, /* $6c */
    {   "adc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $6d */
    {   "ror",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $6e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $6f */
    {   "bvs",  2,  flLabel,                  OH_Relative              }, /* $70 */
    {   "adc",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $71 */
    {   "adc",  2,  flUseLabel,               OH_DirectIndirect,       }, /* $72 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $73 */
    {   "stz",  2,  flUseLabel,               OH_DirectX               }, /* $74 */
    {   "adc",  2,  flUseLabel,               OH_DirectX               }, /* $75 */
    {   "ror",  2,  flUseLabel,               OH_DirectX               }, /* $76 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $77 */
    {   "sei",  1,  flNone,                   OH_Implicit              }, /* $78 */
    {   "adc",  3,  flUseLabel,               OH_AbsoluteY             }, /* $79 */
    {   "ply",  1,  flNone,                   OH_Implicit              }, /* $7a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $7b */
    {   "jmp",  3,  flLabel,                  OH_AbsoluteXIndirect     }, /* $7c */
    {   "adc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $7d */
    {   "ror",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $7e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $7f */
    {   "bra",  2,  flLabel,                  OH_Relative              }, /* $80 */
    {   "sta",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $81 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $82 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $83 */
    {   "sty",  2,  flUseLabel,               OH_Direct                }, /* $84 */
    {   "sta",  2,  flUseLabel,               OH_Direct                }, /* $85 */
    {   "stx",  2,  flUseLabel,               OH_Direct                }, /* $86 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $87 */
    {   "dey",  1,  flNone,                   OH_Implicit              }, /* $88 */
    {   "bit",  2,  flNone,                   OH_Immediate             }, /* $89 */
    {   "txa",  1,  flNone,                   OH_Implicit              }, /* $8a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $8b */
    {   "sty",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8c */
    {   "sta",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8d */
    {   "stx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $8f */
    {   "bcc",  2,  flLabel,                  OH_Relative              }, /* $90 */
    {   "sta",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $91 */
    {   "sta",  2,  flUseLabel,               OH_DirectIndirect        }, /* $92 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $93 */
    {   "sty",  2,  flUseLabel,               OH_DirectX               }, /* $94 */
    {   "sta",  2,  flUseLabel,               OH_DirectX               }, /* $95 */
    {   "stx",  2,  flUseLabel,               OH_DirectY               }, /* $96 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $97 */
    {   "tya",  1,  flNone,                   OH_Implicit              }, /* $98 */
    {   "sta",  3,  flUseLabel,               OH_AbsoluteY             }, /* $99 */
    {   "txs",  1,  flNone,                   OH_Implicit              }, /* $9a */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $9b */
    {   "stz",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $9c */
    {   "sta",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $9d */
    {   "stz",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $9e */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $9f */
    {   "ldy",  2,  flNone,                   OH_Immediate             }, /* $a0 */
    {   "lda",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $a1 */
    {   "ldx",  2,  flNone,                   OH_Immediate             }, /* $a2 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $a3 */
    {   "ldy",  2,  flUseLabel,               OH_Direct                }, /* $a4 */
    {   "lda",  2,  flUseLabel,               OH_Direct                }, /* $a5 */
    {   "ldx",  2,  flUseLabel,               OH_Direct                }, /* $a6 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $a7 */
    {   "tay",  1,  flNone,                   OH_Implicit              }, /* $a8 */
    {   "lda",  2,  flNone,                   OH_Immediate             }, /* $a9 */
    {   "tax",  1,  flNone,                   OH_Implicit              }, /* $aa */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $ab */
    {   "ldy",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ac */
    {   "lda",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ad */
    {   "ldx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ae */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $af */
    {   "bcs",  2,  flLabel,                  OH_Relative              }, /* $b0 */
    {   "lda",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $b1 */
    {   "lda",  2,  flUseLabel,               OH_DirectIndirect        }, /* $b2 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $b3 */
    {   "ldy",  2,  flUseLabel,               OH_DirectX               }, /* $b4 */
    {   "lda",  2,  flUseLabel,               OH_DirectX               }, /* $b5 */
    {   "ldx",  2,  flUseLabel,               OH_DirectY               }, /* $b6 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $b7 */
    {   "clv",  1,  flNone,                   OH_Implicit              }, /* $b8 */
    {   "lda",  3,  flUseLabel,               OH_AbsoluteY             }, /* $b9 */
    {   "tsx",  1,  flNone,                   OH_Implicit              }, /* $ba */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $bb */
    {   "ldy",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $bc */
    {   "lda",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $bd */
    {   "ldx",  3,  flUseLabel|flAbsOverride, OH_AbsoluteY             }, /* $be */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $bf */
    {   "cpy",  2,  flNone,                   OH_Immediate             }, /* $c0 */
    {   "cmp",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $c1 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $c2 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $c3 */
    {   "cpy",  2,  flUseLabel,               OH_Direct                }, /* $c4 */
    {   "cmp",  2,  flUseLabel,               OH_Direct                }, /* $c5 */
    {   "dec",  2,  flUseLabel,               OH_Direct                }, /* $c6 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $c7 */
    {   "iny",  1,  flNone,                   OH_Implicit              }, /* $c8 */
    {   "cmp",  2,  flNone,                   OH_Immediate             }, /* $c9 */
    {   "dex",  1,  flNone,                   OH_Implicit              }, /* $ca */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $cb */
    {   "cpy",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $cc */
    {   "cmp",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $cd */
    {   "dec",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ce */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $cf */
    {   "bne",  2,  flLabel,                  OH_Relative              }, /* $d0 */
    {   "cmp",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $d1 */
    {   "cmp",  2,  flUseLabel,               OH_DirectIndirect        }, /* $d2 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $d3 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $d4 */
    {   "cmp",  2,  flUseLabel,               OH_DirectX               }, /* $d5 */
    {   "dec",  2,  flUseLabel,               OH_DirectX               }, /* $d6 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $d7 */
    {   "cld",  1,  flNone,                   OH_Implicit              }, /* $d8 */
    {   "cmp",  3,  flUseLabel,               OH_AbsoluteY             }, /* $d9 */
    {   "phx",  1,  flNone,                   OH_Implicit              }, /* $da */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $db */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $dc */
    {   "cmp",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $dd */
    {   "dec",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $de */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $df */
    {   "cpx",  2,  flNone,                   OH_Immediate             }, /* $e0 */
    {   "sbc",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $e1 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $e2 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $e3 */
    {   "cpx",  2,  flUseLabel,               OH_Direct                }, /* $e4 */
    {   "sbc",  2,  flUseLabel,               OH_Direct                }, /* $e5 */
    {   "inc",  2,  flUseLabel,               OH_Direct                }, /* $e6 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $e7 */
    {   "inx",  1,  flNone,                   OH_Implicit              }, /* $e8 */
    {   "sbc",  2,  flNone,                   OH_Immediate             }, /* $e9 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $ea */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $eb */
    {   "cpx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ec */
    {   "sbc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ed */
    {   "inc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ee */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $ef */
    {   "beq",  2,  flLabel,                  OH_Relative              }, /* $f0 */
    {   "sbc",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $f1 */
    {   "sbc",  2,  flUseLabel,               OH_DirectIndirect        }, /* $f2 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $f3 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $f4 */
    {   "sbc",  2,  flUseLabel,               OH_DirectX               }, /* $f5 */
    {   "inc",  2,  flUseLabel,               OH_DirectX               }, /* $f6 */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $f7 */
    {   "sed",  1,  flNone,                   OH_Implicit              }, /* $f8 */
    {   "sbc",  3,  flUseLabel,               OH_AbsoluteY             }, /* $f9 */
    {   "plx",  1,  flNone,                   OH_Implicit              }, /* $fa */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $fb */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $fc */
    {   "sbc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $fd */
    {   "inc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $fe */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $ff */
};
