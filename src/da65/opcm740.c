/*****************************************************************************/
/*                                                                           */
/*                                 opcm740.c                                 */
/*                                                                           */
/*               Mitsubishi 740 series opcode description table              */
/*                                                                           */
/* A contribution from Chris Baird                                           */
/* EMail:         cjb@brushtail.apana.org.au                                 */
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
#include "opcm740.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Descriptions for all opcodes */
const OpcDesc OpcTable_M740[256] = {
    {   "brk",  1,  flNone,                   OH_Implicit              }, /* $00 */
    {   "ora",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $01 */
    {   "jsr",  2,  flLabel,                  OH_JmpDirectIndirect     }, /* $02 */
    {   "bbs",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $03 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $04 */
    {   "ora",  2,  flUseLabel,               OH_Direct                }, /* $05 */
    {   "asl",  2,  flUseLabel,               OH_Direct                }, /* $06 */
    {   "bbs",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $07 */
    {   "php",  1,  flNone,                   OH_Implicit              }, /* $08 */
    {   "ora",  2,  flNone,                   OH_Immediate             }, /* $09 */
    {   "asl",  1,  flNone,                   OH_Accumulator           }, /* $0a */
    {   "seb",  1,  flNone,                   OH_AccumulatorBit        }, /* $0b */
    {   "",     1,  flIllegal,                OH_Illegal,              }, /* $0c */
    {   "ora",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0d */
    {   "asl",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $0e */
    {   "seb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $0f */
    {   "bpl",  2,  flLabel,                  OH_Relative              }, /* $10 */
    {   "ora",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $11 */
    {   "clt",  1,  flNone,                   OH_Implicit              }, /* $12 */
    {   "bbc",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $13 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $14 */
    {   "ora",  2,  flUseLabel,               OH_DirectX               }, /* $15 */
    {   "asl",  2,  flUseLabel,               OH_DirectX               }, /* $16 */
    {   "bbc",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $17 */
    {   "clc",  1,  flNone,                   OH_Implicit              }, /* $18 */
    {   "ora",  3,  flUseLabel,               OH_AbsoluteY             }, /* $19 */
    {   "dec",  1,  flNone,                   OH_Accumulator           }, /* $1a */
    {   "clb",  1,  flNone,                   OH_AccumulatorBit        }, /* $1b */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $1c */
    {   "ora",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $1d */
    {   "asl",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $1e */
    {   "clb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $1f */
    {   "jsr",  3,  flLabel,                  OH_Absolute              }, /* $20 */
    {   "and",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $21 */
    {   "jsr",  2,  flLabel,                  OH_SpecialPage           }, /* $22 */
    {   "bbs",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $23 */
    {   "bit",  2,  flUseLabel,               OH_Direct                }, /* $24 */
    {   "and",  2,  flUseLabel,               OH_Direct                }, /* $25 */
    {   "rol",  2,  flUseLabel,               OH_Direct                }, /* $26 */
    {   "bbs",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $27 */
    {   "plp",  1,  flNone,                   OH_Implicit              }, /* $28 */
    {   "and",  2,  flNone,                   OH_Immediate             }, /* $29 */
    {   "rol",  1,  flNone,                   OH_Accumulator           }, /* $2a */
    {   "seb",  1,  flNone,                   OH_AccumulatorBit        }, /* $2b */
    {   "bit",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2c */
    {   "and",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2d */
    {   "rol",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $2e */
    {   "seb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $2f */
    {   "bmi",  2,  flLabel,                  OH_Relative              }, /* $30 */
    {   "and",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $31 */
    {   "set",  1,  flNone,                   OH_Implicit              }, /* $32 */
    {   "bbc",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $33 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $34 */
    {   "and",  2,  flUseLabel,               OH_DirectX               }, /* $35 */
    {   "rol",  2,  flUseLabel,               OH_DirectX               }, /* $36 */
    {   "bbc",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $37 */
    {   "sec",  1,  flNone,                   OH_Implicit              }, /* $38 */
    {   "and",  3,  flUseLabel,               OH_AbsoluteY             }, /* $39 */
    {   "inc",  1,  flNone,                   OH_Accumulator           }, /* $3a */
    {   "clb",  1,  flNone,                   OH_AccumulatorBit        }, /* $3b */
    {   "ldm",  3,  flLabel,                  OH_DirectImmediate       }, /* $3c */
    {   "and",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $3d */
    {   "rol",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $3e */
    {   "clb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $3f */
    {   "rti",  1,  flNone,                   OH_Rts                   }, /* $40 */
    {   "eor",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $41 */
    {   "stp",  1,  flNone,                   OH_Implicit              }, /* $42 */
    {   "bbs",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $43 */
    {   "com",  2,  flUseLabel,               OH_Direct                }, /* $44 */
    {   "eor",  2,  flUseLabel,               OH_Direct                }, /* $45 */
    {   "lsr",  2,  flUseLabel,               OH_Direct                }, /* $46 */
    {   "bbs",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $47 */
    {   "pha",  1,  flNone,                   OH_Implicit              }, /* $48 */
    {   "eor",  2,  flNone,                   OH_Immediate             }, /* $49 */
    {   "lsr",  1,  flNone,                   OH_Accumulator           }, /* $4a */
    {   "seb",  1,  flNone,                   OH_AccumulatorBit        }, /* $4b */
    {   "jmp",  3,  flLabel,                  OH_JmpAbsolute           }, /* $4c */
    {   "eor",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $4d */
    {   "lsr",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $4e */
    {   "seb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $4f */
    {   "bvc",  2,  flLabel,                  OH_Relative              }, /* $50 */
    {   "eor",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $51 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $52 */
    {   "bbc",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $53 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $54 */
    {   "eor",  2,  flUseLabel,               OH_DirectX               }, /* $55 */
    {   "lsr",  2,  flUseLabel,               OH_DirectX               }, /* $56 */
    {   "bbc",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $57 */
    {   "cli",  1,  flNone,                   OH_Implicit              }, /* $58 */
    {   "eor",  3,  flUseLabel,               OH_AbsoluteY             }, /* $59 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $5a */
    {   "clb",  1,  flNone,                   OH_AccumulatorBit        }, /* $5b */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $5c */
    {   "eor",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $5d */
    {   "lsr",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $5e */
    {   "clb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $5f */
    {   "rts",  1,  flNone,                   OH_Rts                   }, /* $60 */
    {   "adc",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $61 */
    {   "mul",  2,  flUseLabel,               OH_DirectX               }, /* $62 */
    {   "bbs",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $63 */
    {   "tst",  2,  flUseLabel,               OH_Direct                }, /* $64 */
    {   "adc",  2,  flUseLabel,               OH_Direct                }, /* $65 */
    {   "ror",  2,  flUseLabel,               OH_Direct                }, /* $66 */
    {   "bbs",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $67 */
    {   "pla",  1,  flNone,                   OH_Implicit              }, /* $68 */
    {   "adc",  2,  flNone,                   OH_Immediate             }, /* $69 */
    {   "ror",  1,  flNone,                   OH_Accumulator           }, /* $6a */
    {   "seb",  1,  flNone,                   OH_AccumulatorBit        }, /* $6b */
    {   "jmp",  3,  flLabel,                  OH_JmpAbsoluteIndirect   }, /* $6c */
    {   "adc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $6d */
    {   "ror",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $6e */
    {   "seb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $6f */
    {   "bvs",  2,  flLabel,                  OH_Relative              }, /* $70 */
    {   "adc",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $71 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $72 */
    {   "bbc",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $73 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $74 */
    {   "adc",  2,  flUseLabel,               OH_DirectX               }, /* $75 */
    {   "ror",  2,  flUseLabel,               OH_DirectX               }, /* $76 */
    {   "bbc",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $77 */
    {   "sei",  1,  flNone,                   OH_Implicit              }, /* $78 */
    {   "adc",  3,  flUseLabel,               OH_AbsoluteY             }, /* $79 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $7a */
    {   "clb",  1,  flNone,                   OH_AccumulatorBit        }, /* $7b */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $7c */
    {   "adc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $7d */
    {   "ror",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $7e */
    {   "clb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $7f */
    {   "bra",  2,  flLabel,                  OH_Relative              }, /* $80 */
    {   "sta",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $81 */
    {   "rrf",  2,  flLabel,                  OH_Direct                }, /* $82 */
    {   "bbs",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $83 */
    {   "sty",  2,  flUseLabel,               OH_Direct                }, /* $84 */
    {   "sta",  2,  flUseLabel,               OH_Direct                }, /* $85 */
    {   "stx",  2,  flUseLabel,               OH_Direct                }, /* $86 */
    {   "bbs",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $87 */
    {   "dey",  1,  flNone,                   OH_Implicit              }, /* $88 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $89 */
    {   "txa",  1,  flNone,                   OH_Implicit              }, /* $8a */
    {   "seb",  1,  flNone,                   OH_AccumulatorBit        }, /* $8b */
    {   "sty",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8c */
    {   "sta",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8d */
    {   "stx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $8e */
    {   "seb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $8f */
    {   "bcc",  2,  flLabel,                  OH_Relative              }, /* $90 */
    {   "sta",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $91 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $92 */
    {   "bbc",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $93 */
    {   "sty",  2,  flUseLabel,               OH_DirectX               }, /* $94 */
    {   "sta",  2,  flUseLabel,               OH_DirectX               }, /* $95 */
    {   "stx",  2,  flUseLabel,               OH_DirectY               }, /* $96 */
    {   "bbc",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $97 */
    {   "tya",  1,  flNone,                   OH_Implicit              }, /* $98 */
    {   "sta",  3,  flUseLabel,               OH_AbsoluteY             }, /* $99 */
    {   "txs",  1,  flNone,                   OH_Implicit              }, /* $9a */
    {   "clb",  1,  flNone,                   OH_AccumulatorBit        }, /* $9b */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $9c */
    {   "sta",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $9d */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $9e */
    {   "clb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $9f */
    {   "ldy",  2,  flNone,                   OH_Immediate             }, /* $a0 */
    {   "lda",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $a1 */
    {   "ldx",  2,  flNone,                   OH_Immediate             }, /* $a2 */
    {   "bbs",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $a3 */
    {   "ldy",  2,  flUseLabel,               OH_Direct                }, /* $a4 */
    {   "lda",  2,  flUseLabel,               OH_Direct                }, /* $a5 */
    {   "ldx",  2,  flUseLabel,               OH_Direct                }, /* $a6 */
    {   "bbs",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $a7 */
    {   "tay",  1,  flNone,                   OH_Implicit              }, /* $a8 */
    {   "lda",  2,  flNone,                   OH_Immediate             }, /* $a9 */
    {   "tax",  1,  flNone,                   OH_Implicit              }, /* $aa */
    {   "seb",  1,  flNone,                   OH_AccumulatorBit        }, /* $ab */
    {   "ldy",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ac */
    {   "lda",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ad */
    {   "ldx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ae */
    {   "seb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $af */
    {   "bcs",  2,  flLabel,                  OH_Relative              }, /* $b0 */
    {   "lda",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $b1 */
    {   "jmp",  2,  flLabel,                  OH_JmpDirectIndirect     }, /* $b2 */
    {   "bbc",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $b3 */
    {   "ldy",  2,  flUseLabel,               OH_DirectX               }, /* $b4 */
    {   "lda",  2,  flUseLabel,               OH_DirectX               }, /* $b5 */
    {   "ldx",  2,  flUseLabel,               OH_DirectY               }, /* $b6 */
    {   "bbc",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $b7 */
    {   "clv",  1,  flNone,                   OH_Implicit              }, /* $b8 */
    {   "lda",  3,  flUseLabel,               OH_AbsoluteY             }, /* $b9 */
    {   "tsx",  1,  flNone,                   OH_Implicit              }, /* $ba */
    {   "clb",  1,  flNone,                   OH_AccumulatorBit        }, /* $bb */
    {   "ldy",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $bc */
    {   "lda",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $bd */
    {   "ldx",  3,  flUseLabel|flAbsOverride, OH_AbsoluteY             }, /* $be */
    {   "clb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $bf */
    {   "cpy",  2,  flNone,                   OH_Immediate             }, /* $c0 */
    {   "cmp",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $c1 */
    {   "wit",  1,  flNone,                   OH_Implicit,             }, /* $c2 */
    {   "bbs",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $c3 */
    {   "cpy",  2,  flUseLabel,               OH_Direct                }, /* $c4 */
    {   "cmp",  2,  flUseLabel,               OH_Direct                }, /* $c5 */
    {   "dec",  2,  flUseLabel,               OH_Direct                }, /* $c6 */
    {   "bbs",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $c7 */
    {   "iny",  1,  flNone,                   OH_Implicit              }, /* $c8 */
    {   "cmp",  2,  flNone,                   OH_Immediate             }, /* $c9 */
    {   "dex",  1,  flNone,                   OH_Implicit              }, /* $ca */
    {   "seb",  1,  flNone,                   OH_AccumulatorBit        }, /* $cb */
    {   "cpy",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $cc */
    {   "cmp",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $cd */
    {   "dec",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ce */
    {   "seb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $cf */
    {   "bne",  2,  flLabel,                  OH_Relative              }, /* $d0 */
    {   "cmp",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $d1 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $d2 */
    {   "bbc",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $d3 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $d4 */
    {   "cmp",  2,  flUseLabel,               OH_DirectX               }, /* $d5 */
    {   "dec",  2,  flUseLabel,               OH_DirectX               }, /* $d6 */
    {   "bbc",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $d7 */
    {   "cld",  1,  flNone,                   OH_Implicit              }, /* $d8 */
    {   "cmp",  3,  flUseLabel,               OH_AbsoluteY             }, /* $d9 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $da */
    {   "clb",  1,  flNone,                   OH_AccumulatorBit        }, /* $db */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $dc */
    {   "cmp",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $dd */
    {   "dec",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $de */
    {   "clb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $df */
    {   "cpx",  2,  flNone,                   OH_Immediate             }, /* $e0 */
    {   "sbc",  2,  flUseLabel,               OH_DirectXIndirect       }, /* $e1 */
    {   "div",  2,  flUseLabel,               OH_DirectX               }, /* $e2 */
    {   "bbs",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $e3 */
    {   "cpx",  2,  flUseLabel,               OH_Direct                }, /* $e4 */
    {   "sbc",  2,  flUseLabel,               OH_Direct                }, /* $e5 */
    {   "inc",  2,  flUseLabel,               OH_Direct                }, /* $e6 */
    {   "bbs",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $e7 */
    {   "inx",  1,  flNone,                   OH_Implicit              }, /* $e8 */
    {   "sbc",  2,  flNone,                   OH_Immediate             }, /* $e9 */
    {   "nop",  1,  flNone,                   OH_Implicit              }, /* $ea */
    {   "seb",  1,  flNone,                   OH_AccumulatorBit        }, /* $eb */
    {   "cpx",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ec */
    {   "sbc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ed */
    {   "inc",  3,  flUseLabel|flAbsOverride, OH_Absolute              }, /* $ee */
    {   "seb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $ef */
    {   "beq",  2,  flLabel,                  OH_Relative              }, /* $f0 */
    {   "sbc",  2,  flUseLabel,               OH_DirectIndirectY       }, /* $f1 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $f2 */
    {   "bbc",  2,  flUseLabel,               OH_AccumulatorBitBranch  }, /* $f3 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $f4 */
    {   "sbc",  2,  flUseLabel,               OH_DirectX               }, /* $f5 */
    {   "inc",  2,  flUseLabel,               OH_DirectX               }, /* $f6 */
    {   "bbc",  3,  flUseLabel,               OH_ZeroPageBit           }, /* $f7 */
    {   "sed",  1,  flNone,                   OH_Implicit              }, /* $f8 */
    {   "sbc",  3,  flUseLabel,               OH_AbsoluteY             }, /* $f9 */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $fa */
    {   "clb",  1,  flNone,                   OH_AccumulatorBit        }, /* $fb */
    {   "",     1,  flIllegal,                OH_Illegal               }, /* $fc */
    {   "sbc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $fd */
    {   "inc",  3,  flUseLabel|flAbsOverride, OH_AbsoluteX             }, /* $fe */
    {   "clb",  2,  flUseLabel,               OH_ZeroPageBit           }, /* $ff */
};
