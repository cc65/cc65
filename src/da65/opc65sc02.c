/*****************************************************************************/
/*                                                                           */
/*				  opc65sc02.h				     */
/*                                                                           */
/*                      65SC02 opcode description table                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
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



/* da65 */
#include "handler.h"
#include "opc65sc02.h"



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Descriptions for all opcodes */
const OpcDesc OpcTable_65SC02[256] = {
    {   "brk",  1,  0,          CPU_6502,   OH_Implicit              }, /* $00 */
    {   "ora",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $01 */
    {   "cop",  2,  0,          CPU_65816,  OH_Implicit              }, /* $02 */
    {   "ora",  2,  0,          CPU_65816,  OH_StackRelative         }, /* $03 */
    {   "tsb",  2,  lfUseLabel, CPU_65816,  OH_Direct                }, /* $04 */
    {   "ora",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $05 */
    {   "asl",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $06 */
    {   "ora",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLong    }, /* $07 */
    {   "php",  1,  0,          CPU_6502,   OH_Implicit              }, /* $08 */
    {   "ora",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $09 */
    {   "asl",  1,  0,          CPU_6502,   OH_Accumulator           }, /* $0a */
    {   "phd",  1,  0,          CPU_65816,  OH_Implicit              }, /* $0b */
    {   "tsb",  3,  lfUseLabel, CPU_65816,  OH_Absolute              }, /* $0c */
    {   "ora",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $0d */
    {   "asl",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $0e */
    {   "ora",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLong          }, /* $0f */
    {   "bpl",  2,  lfLabel,    CPU_6502,   OH_Relative              }, /* $10 */
    {   "ora",  2,  lfUseLabel, CPU_6502,   OH_DirectIndirectY       }, /* $11 */
    {   "ora",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirect        }, /* $12 */
    {   "ora",  2,  0,          CPU_65816,  OH_StackRelativeIndirectY}, /* $13 */
    {   "trb",  2,  lfUseLabel, CPU_65816,  OH_Direct                }, /* $14 */
    {   "ora",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $15 */
    {   "asl",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $16 */
    {   "ora",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLongY   }, /* $17 */
    {   "clc",  1,  0,          CPU_6502,   OH_Implicit              }, /* $18 */
    {   "ora",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $19 */
    {   "inc",  1,  0,          CPU_65816,  OH_Accumulator           }, /* $1a */
    {   "tcs",  1,  0,          CPU_65816,  OH_Implicit              }, /* $1b */
    {   "trb",  3,  lfUseLabel, CPU_65816,  OH_Absolute              }, /* $1c */
    {   "ora",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $1d */
    {   "asl",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $1e */
    {   "ora",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLongX         }, /* $1f */
    {   "jsr",  3,  lfLabel,    CPU_6502,   OH_Absolute              }, /* $20 */
    {   "and",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $21 */
    {   "jsl",  3,  lfLabel,    CPU_65816,  OH_AbsoluteLong          }, /* $22 */
    {   "and",  2,  0,          CPU_65816,  OH_StackRelative         }, /* $23 */
    {   "bit",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $24 */
    {   "and",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $25 */
    {   "rol",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $26 */
    {   "and",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLong    }, /* $27 */
    {   "plp",  1,  0,          CPU_6502,   OH_Implicit              }, /* $28 */
    {   "and",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $29 */
    {   "rol",  1,  0,          CPU_6502,   OH_Accumulator           }, /* $2a */
    {   "pld",  1,  0,          CPU_65816,  OH_Implicit              }, /* $2b */
    {   "bit",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $2c */
    {   "and",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $2d */
    {   "rol",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $2e */
    {   "and",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLong          }, /* $2f */
    {   "bmi",  2,  lfLabel,    CPU_6502,   OH_Relative              }, /* $30 */
    {   "and",  2,  lfUseLabel, CPU_6502,   OH_DirectIndirectY       }, /* $31 */
    {   "and",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirect        }, /* $32 */
    {   "and",  2,  0,          CPU_65816,  OH_StackRelativeIndirectY}, /* $33 */
    {   "bit",  2,  lfUseLabel, CPU_65816,  OH_DirectX               }, /* $34 */
    {   "and",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $35 */
    {   "rol",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $36 */
    {   "and",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLongY   }, /* $37 */
    {   "sec",  1,  0,          CPU_6502,   OH_Implicit              }, /* $38 */
    {   "and",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $39 */
    {   "dec",  1,  0,          CPU_65816,  OH_Accumulator           }, /* $3a */
    {   "tsc",  1,  0,          CPU_65816,  OH_Implicit              }, /* $3b */
    {   "bit",  3,  lfUseLabel, CPU_65816,  OH_AbsoluteX             }, /* $3c */
    {   "and",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $3d */
    {   "rol",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $3e */
    {   "and",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLongX         }, /* $3f */
    {   "rti",  1,  0,          CPU_6502,   OH_Rts                   }, /* $40 */
    {   "eor",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $41 */
    {   "wdm",  2,  0,          CPU_65816,  OH_Implicit              }, /* $42 */
    {   "eor",  2,  0,          CPU_65816,  OH_StackRelative         }, /* $43 */
    {   "mvp",  3,  0,          CPU_65816,  OH_BlockMove             }, /* $44 */
    {   "eor",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $45 */
    {   "lsr",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $46 */
    {   "eor",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLong    }, /* $47 */
    {   "pha",  1,  0,          CPU_6502,   OH_Implicit              }, /* $48 */
    {   "eor",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $49 */
    {   "lsr",  1,  0,          CPU_6502,   OH_Accumulator           }, /* $4a */
    {   "phk",  1,  0,          CPU_65816,  OH_Implicit              }, /* $4b */
    {   "jmp",  3,  lfLabel,    CPU_6502,   OH_JmpAbsolute           }, /* $4c */
    {   "eor",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $4d */
    {   "lsr",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $4e */
    {   "eor",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLong          }, /* $4f */
    {   "bvc",  2,  lfLabel,    CPU_6502,   OH_Relative              }, /* $50 */
    {   "eor",  2,  lfUseLabel, CPU_6502,   OH_DirectIndirectY       }, /* $51 */
    {   "eor",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirect        }, /* $52 */
    {   "eor",  2,  0,          CPU_65816,  OH_StackRelativeIndirectY}, /* $53 */
    {   "mvn",  3,  0,          CPU_65816,  OH_BlockMove             }, /* $54 */
    {   "eor",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $55 */
    {   "lsr",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $56 */
    {   "eor",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLongY   }, /* $57 */
    {   "cli",  1,  0,          CPU_6502,   OH_Implicit              }, /* $58 */
    {   "eor",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $59 */
    {   "phy",  1,  0,          CPU_65816,  OH_Implicit              }, /* $5a */
    {   "tcd",  1,  0,          CPU_65816,  OH_Implicit              }, /* $5b */
    {   "jml",  4,  lfLabel,    CPU_65816,  OH_AbsoluteLong          }, /* $5c */
    {   "eor",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $5d */
    {   "lsr",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $5e */
    {   "eor",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLongX         }, /* $5f */
    {   "rts",  1,  0,          CPU_6502,   OH_Rts                   }, /* $60 */
    {   "adc",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $61 */
    {   "per",  3,  lfLabel,    CPU_65816,  OH_RelativeLong          }, /* $62 */
    {   "adc",  2,  0,          CPU_65816,  OH_StackRelative         }, /* $63 */
    {   "stz",  2,  lfUseLabel, CPU_65816,  OH_Direct                }, /* $64 */
    {   "adc",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $65 */
    {   "ror",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $66 */
    {   "adc",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLong    }, /* $67 */
    {   "pla",  1,  0,          CPU_6502,   OH_Implicit              }, /* $68 */
    {   "adc",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $69 */
    {   "ror",  1,  0,          CPU_6502,   OH_Accumulator           }, /* $6a */
    {   "rtl",  1,  0,          CPU_65816,  OH_Implicit              }, /* $6b */
    {   "jmp",  3,  lfLabel,    CPU_6502,   OH_JmpAbsoluteIndirect   }, /* $6c */
    {   "adc",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $6d */
    {   "ror",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $6e */
    {   "adc",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLong          }, /* $6f */
    {   "bvs",  2,  lfLabel,    CPU_6502,   OH_Relative              }, /* $70 */
    {   "adc",  2,  lfUseLabel, CPU_6502,   OH_DirectIndirectY       }, /* $71 */
    {   "adc",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirect        }, /* $72 */
    {   "adc",  2,  0,          CPU_65816,  OH_StackRelativeIndirectY}, /* $73 */
    {   "stz",  2,  lfUseLabel, CPU_65816,  OH_DirectX               }, /* $74 */
    {   "adc",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $75 */
    {   "ror",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $76 */
    {   "adc",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLongY   }, /* $77 */
    {   "sei",  1,  0,          CPU_6502,   OH_Implicit              }, /* $78 */
    {   "adc",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $79 */
    {   "ply",  1,  0,          CPU_65816,  OH_Implicit              }, /* $7a */
    {   "tdc",  1,  0,          CPU_65816,  OH_Implicit              }, /* $7b */
    {   "jmp",  3,  lfLabel,    CPU_65816,  OH_AbsoluteXIndirect     }, /* $7c */
    {   "adc",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $7d */
    {   "ror",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $7e */
    {   "adc",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLongX         }, /* $7f */
    {   "bra",  2,  lfLabel,    CPU_65816,  OH_Relative              }, /* $80 */
    {   "sta",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $81 */
    {   "brl",  3,  lfLabel,    CPU_65816,  OH_RelativeLong          }, /* $82 */
    {   "sta",  2,  0,          CPU_65816,  OH_StackRelative         }, /* $83 */
    {   "sty",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $84 */
    {   "sta",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $85 */
    {   "stx",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $86 */
    {   "sta",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLong    }, /* $87 */
    {   "dey",  1,  0,          CPU_6502,   OH_Implicit              }, /* $88 */
    {   "bit",  2,  0,          CPU_65816,  OH_Immidiate             }, /* $89 */
    {   "txa",  1,  0,          CPU_6502,   OH_Implicit              }, /* $8a */
    {   "phb",  1,  0,          CPU_65816,  OH_Implicit              }, /* $8b */
    {   "sty",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $8c */
    {   "sta",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $8d */
    {   "stx",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $8e */
    {   "sta",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLong          }, /* $8f */
    {   "bcc",  2,  lfLabel,    CPU_6502,   OH_Relative              }, /* $90 */
    {   "sta",  2,  lfUseLabel, CPU_6502,   OH_DirectIndirectY       }, /* $91 */
    {   "sta",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirect        }, /* $92 */
    {   "sta",  2,  0,          CPU_65816,  OH_StackRelativeIndirectY}, /* $93 */
    {   "sty",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $94 */
    {   "sta",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $95 */
    {   "stx",  2,  lfUseLabel, CPU_6502,   OH_DirectY               }, /* $96 */
    {   "sta",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLongY   }, /* $97 */
    {   "tya",  1,  0,          CPU_6502,   OH_Implicit              }, /* $98 */
    {   "sta",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $99 */
    {   "txs",  1,  0,          CPU_6502,   OH_Implicit              }, /* $9a */
    {   "txy",  1,  0,          CPU_65816,  OH_Implicit              }, /* $9b */
    {   "stz",  3,  lfUseLabel, CPU_65816,  OH_Absolute              }, /* $9c */
    {   "sta",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $9d */
    {   "stz",  3,  lfUseLabel, CPU_65816,  OH_AbsoluteX             }, /* $9e */
    {   "sta",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLongX         }, /* $9f */
    {   "ldy",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $a0 */
    {   "lda",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $a1 */
    {   "ldx",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $a2 */
    {   "lda",  2,  0,          CPU_65816,  OH_StackRelative         }, /* $a3 */
    {   "ldy",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $a4 */
    {   "lda",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $a5 */
    {   "ldx",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $a6 */
    {   "lda",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLong    }, /* $a7 */
    {   "tay",  1,  0,          CPU_6502,   OH_Implicit              }, /* $a8 */
    {   "lda",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $a9 */
    {   "tax",  1,  0,          CPU_6502,   OH_Implicit              }, /* $aa */
    {   "plb",  1,  0,          CPU_65816,  OH_Implicit              }, /* $ab */
    {   "ldy",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $ac */
    {   "lda",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $ad */
    {   "ldx",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $ae */
    {   "lda",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLong          }, /* $af */
    {   "bcs",  2,  lfLabel,    CPU_6502,   OH_Relative              }, /* $b0 */
    {   "lda",  2,  lfUseLabel, CPU_6502,   OH_DirectIndirectY       }, /* $b1 */
    {   "lda",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirect        }, /* $b2 */
    {   "lda",  2,  0,          CPU_65816,  OH_StackRelativeIndirectY}, /* $b3 */
    {   "ldy",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $b4 */
    {   "lda",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $b5 */
    {   "ldx",  2,  lfUseLabel, CPU_6502,   OH_DirectY               }, /* $b6 */
    {   "lda",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLongY   }, /* $b7 */
    {   "clv",  1,  0,          CPU_6502,   OH_Implicit              }, /* $b8 */
    {   "lda",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $b9 */
    {   "tsx",  1,  0,          CPU_6502,   OH_Implicit              }, /* $ba */
    {   "tyx",  1,  0,          CPU_65816,  OH_Implicit              }, /* $bb */
    {   "ldy",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $bc */
    {   "lda",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $bd */
    {   "ldx",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $be */
    {   "lda",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLongX         }, /* $bf */
    {   "cpy",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $c0 */
    {   "cmp",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $c1 */
    {   "rep",  2,  0,          CPU_65816,  OH_Immidiate             }, /* $c2 */
    {   "cmp",  2,  0,          CPU_65816,  OH_StackRelative         }, /* $c3 */
    {   "cpy",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $c4 */
    {   "cmp",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $c5 */
    {   "dec",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $c6 */
    {   "cmp",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLong    }, /* $c7 */
    {   "iny",  1,  0,          CPU_6502,   OH_Implicit              }, /* $c8 */
    {   "cmp",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $c9 */
    {   "dex",  1,  0,          CPU_6502,   OH_Implicit              }, /* $ca */
    {   "wai",  1,  0,          CPU_65816,  OH_Implicit              }, /* $cb */
    {   "cpy",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $cc */
    {   "cmp",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $cd */
    {   "dec",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $ce */
    {   "cmp",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLong          }, /* $cf */
    {   "bne",  2,  lfLabel,    CPU_6502,   OH_Relative              }, /* $d0 */
    {   "cmp",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $d1 */
    {   "cmp",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirect        }, /* $d2 */
    {   "cmp",  2,  0,          CPU_65816,  OH_StackRelativeIndirectY}, /* $d3 */
    {   "pei",  2,  lfUseLabel, CPU_65816,  OH_Direct                }, /* $d4 */
    {   "cmp",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $d5 */
    {   "dec",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $d6 */
    {   "cmp",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLongY   }, /* $d7 */
    {   "cld",  1,  0,          CPU_6502,   OH_Implicit              }, /* $d8 */
    {   "cmp",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $d9 */
    {   "phx",  1,  0,          CPU_65816,  OH_Implicit              }, /* $da */
    {   "stp",  1,  0,          CPU_65816,  OH_Implicit              }, /* $db */
    {   "jml",  3,  lfLabel,    CPU_65816,  OH_AbsoluteIndirect      }, /* $dc */
    {   "cmp",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $dd */
    {   "dec",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $de */
    {   "cmp",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLongX         }, /* $df */
    {   "cpx",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $e0 */
    {   "sbc",  2,  lfUseLabel, CPU_6502,   OH_DirectXIndirect       }, /* $e1 */
    {   "sep",  2,  0,          CPU_65816,  OH_Immidiate             }, /* $e2 */
    {   "sbc",  2,  0,          CPU_65816,  OH_StackRelative         }, /* $e3 */
    {   "cpx",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $e4 */
    {   "sbc",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $e5 */
    {   "inc",  2,  lfUseLabel, CPU_6502,   OH_Direct                }, /* $e6 */
    {   "sbc",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLong    }, /* $e7 */
    {   "inx",  1,  0,          CPU_6502,   OH_Implicit              }, /* $e8 */
    {   "sbc",  2,  0,          CPU_6502,   OH_Immidiate             }, /* $e9 */
    {   "nop",  1,  0,          CPU_6502,   OH_Implicit              }, /* $ea */
    {   "xba",  1,  0,          CPU_65816,  OH_Implicit              }, /* $eb */
    {   "cpx",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $ec */
    {   "sbc",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $ed */
    {   "inc",  3,  lfUseLabel, CPU_6502,   OH_Absolute              }, /* $ee */
    {   "sbc",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLong          }, /* $ef */
    {   "beq",  2,  lfLabel,    CPU_6502,   OH_Relative              }, /* $f0 */
    {   "sbc",  2,  lfUseLabel, CPU_6502,   OH_DirectIndirectY       }, /* $f1 */
    {   "sbc",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirect        }, /* $f2 */
    {   "sbc",  2,  0,          CPU_65816,  OH_StackRelativeIndirectY}, /* $f3 */
    {   "pea",  3,  lfUseLabel, CPU_65816,  OH_Absolute              }, /* $f4 */
    {   "sbc",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $f5 */
    {   "inc",  2,  lfUseLabel, CPU_6502,   OH_DirectX               }, /* $f6 */
    {   "sbc",  2,  lfUseLabel, CPU_65816,  OH_DirectIndirectLongY   }, /* $f7 */
    {   "sed",  1,  0,          CPU_6502,   OH_Implicit              }, /* $f8 */
    {   "sbc",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteY             }, /* $f9 */
    {   "plx",  1,  0,          CPU_65816,  OH_Implicit              }, /* $fa */
    {   "xce",  1,  0,          CPU_65816,  OH_Implicit              }, /* $fb */
    {   "jsr",  3,  lfLabel,    CPU_65816,  OH_AbsoluteXIndirect     }, /* $fc */
    {   "sbc",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $fd */
    {   "inc",  3,  lfUseLabel, CPU_6502,   OH_AbsoluteX             }, /* $fe */
    {   "sbc",  4,  lfUseLabel, CPU_65816,  OH_AbsoluteLongX         }, /* $ff */
};



