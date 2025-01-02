/*****************************************************************************/
/*                                                                           */
/*                                 trace.c                                   */
/*                                                                           */
/*             Instruction tracing functionality sim65 6502 simulator        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2025, Sidney Cadot                                                    */
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include "6502.h"
#include "memory.h"
#include "trace.h"
#include "peripherals.h"

/* Current Trace Mode. Tracing is off by default, and needs to be explicitly enabled. */
uint8_t TraceMode = TRACE_DISABLED;

/* CC65 stack pointer */
uint8_t StackPointerZPageAddress;

/* 6502, 65C02 addressing modes. */
typedef enum {
    ILLEGAL,
    IMPLIED,
    ACCUMULATOR,
    IMMEDIATE,
    REL,
    ZP,
    ZP_X,
    ZP_Y,
    ZP_IND,
    ZP_X_IND,
    ZP_IND_Y,
    ZP_REL,
    ABS,
    ABS_X,
    ABS_Y,
    ABS_IND,
    ABS_X_IND
} AddressingMode;

/* Info for a specific opcode and addressing mode, for a specific CPU type. */
typedef struct {
    const char * mnemonic;
    AddressingMode adrmode;
} InstructionInfo;

/* Information for standard 6502 opcodes. */
static InstructionInfo II_6502[256] = {
    { "brk"  , IMPLIED     }, /* 0x00 to 0x0f */
    { "ora"  , ZP_X_IND    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "ora"  , ZP          },
    { "asl"  , ZP          },
    { "???"  , ILLEGAL     },
    { "php"  , IMPLIED     },
    { "ora"  , IMMEDIATE   },
    { "asl"  , ACCUMULATOR },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "ora"  , ABS         },
    { "asl"  , ABS         },
    { "???"  , ILLEGAL     },

    { "bpl"  , REL         }, /* 0x10 to 0x1f */
    { "ora"  , ZP_IND_Y    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "ora"  , ZP_X        },
    { "asl"  , ZP_X        },
    { "???"  , ILLEGAL     },
    { "clc"  , IMPLIED     },
    { "ora"  , ABS_Y       },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "ora"  , ABS_X       },
    { "asl"  , ABS_X       },
    { "???"  , ILLEGAL     },

    { "jsr"  , ABS         }, /* 0x20 to 0x2f */
    { "and"  , ZP_X_IND    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "bit"  , ZP          },
    { "and"  , ZP          },
    { "rol"  , ZP          },
    { "???"  , ILLEGAL     },
    { "plp"  , IMPLIED     },
    { "and"  , IMMEDIATE   },
    { "rol"  , ACCUMULATOR },
    { "???"  , ILLEGAL     },
    { "bit"  , ABS         },
    { "and"  , ABS         },
    { "rol"  , ABS         },
    { "???"  , ILLEGAL     },

    { "bmi"  , REL         }, /* 0x30 to 0x3f */
    { "and"  , ZP_IND_Y    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "and"  , ZP_X        },
    { "rol"  , ZP_X        },
    { "???"  , ILLEGAL     },
    { "sec"  , IMPLIED     },
    { "and"  , ABS_Y       },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "and"  , ABS_X       },
    { "rol"  , ABS_X       },
    { "???"  , ILLEGAL     },

    { "rti"  , IMPLIED     }, /* 0x40 to 0x4f */
    { "eor"  , ZP_X_IND    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "eor"  , ZP          },
    { "lsr"  , ZP          },
    { "???"  , ILLEGAL     },
    { "pha"  , IMPLIED     },
    { "eor"  , IMMEDIATE   },
    { "lsr"  , ACCUMULATOR },
    { "???"  , ILLEGAL     },
    { "jmp"  , ABS         },
    { "eor"  , ABS         },
    { "lsr"  , ABS         },
    { "???"  , ILLEGAL     },

    { "bvc"  , REL         }, /* 0x50 to 0x5f */
    { "eor"  , ZP_IND_Y    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "eor"  , ZP_X        },
    { "lsr"  , ZP_X        },
    { "???"  , ILLEGAL     },
    { "cli"  , IMPLIED     },
    { "eor"  , ABS_Y       },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "eor"  , ABS_X       },
    { "lsr"  , ABS_X       },
    { "???"  , ILLEGAL     },

    { "rts"  , IMPLIED     }, /* 0x60 to 0x6f */
    { "adc"  , ZP_X_IND    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "adc"  , ZP          },
    { "ror"  , ZP          },
    { "???"  , ILLEGAL     },
    { "pla"  , IMPLIED     },
    { "adc"  , IMMEDIATE   },
    { "ror"  , ACCUMULATOR },
    { "???"  , ILLEGAL     },
    { "jmp"  , ABS_IND     },
    { "adc"  , ABS         },
    { "ror"  , ABS         },
    { "???"  , ILLEGAL     },

    { "bvs"  , REL         }, /* 0x70 to 0x7f */
    { "adc"  , ZP_IND_Y    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "adc"  , ZP_X        },
    { "ror"  , ZP_X        },
    { "???"  , ILLEGAL     },
    { "sei"  , IMPLIED     },
    { "adc"  , ABS_Y       },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "adc"  , ABS_X       },
    { "ror"  , ABS_X       },
    { "???"  , ILLEGAL     },

    { "???"  , ILLEGAL     }, /* 0x80 to 0x8f */
    { "sta"  , ZP_X_IND    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "sty"  , ZP          },
    { "sta"  , ZP          },
    { "stx"  , ZP          },
    { "???"  , ILLEGAL     },
    { "dey"  , IMPLIED     },
    { "???"  , ILLEGAL     },
    { "txa"  , IMPLIED     },
    { "???"  , ILLEGAL     },
    { "sty"  , ABS         },
    { "sta"  , ABS         },
    { "stx"  , ABS         },
    { "???"  , ILLEGAL     },

    { "bcc"  , REL         }, /* 0x90 to 0x9f */
    { "sta"  , ZP_IND_Y    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "sty"  , ZP_X        },
    { "sta"  , ZP_X        },
    { "stx"  , ZP_Y        },
    { "???"  , ILLEGAL     },
    { "tya"  , IMPLIED     },
    { "sta"  , ABS_Y       },
    { "txs"  , IMPLIED     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "sta"  , ABS_X       },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },

    { "ldy"  , IMMEDIATE   }, /* 0xa0 to 0xaf */
    { "lda"  , ZP_X_IND    },
    { "ldx"  , IMMEDIATE   },
    { "???"  , ILLEGAL     },
    { "ldy"  , ZP          },
    { "lda"  , ZP          },
    { "ldx"  , ZP          },
    { "???"  , ILLEGAL     },
    { "tay"  , IMPLIED     },
    { "lda"  , IMMEDIATE   },
    { "tax"  , IMPLIED     },
    { "???"  , ILLEGAL     },
    { "ldy"  , ABS         },
    { "lda"  , ABS         },
    { "ldx"  , ABS         },
    { "???"  , ILLEGAL     },

    { "bcs"  , REL         }, /* 0xb0 to 0xbf */
    { "lda"  , ZP_IND_Y    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "ldy"  , ZP_X        },
    { "lda"  , ZP_X        },
    { "ldx"  , ZP_Y        },
    { "???"  , ILLEGAL     },
    { "clv"  , IMPLIED     },
    { "lda"  , ABS_Y       },
    { "tsx"  , IMPLIED     },
    { "???"  , ILLEGAL     },
    { "ldy"  , ABS_X       },
    { "lda"  , ABS_X       },
    { "ldx"  , ABS_Y       },
    { "???"  , ILLEGAL     },

    { "cpy"  , IMMEDIATE   }, /* 0xc0 to 0xcf */
    { "cmp"  , ZP_X_IND    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "cpy"  , ZP          },
    { "cmp"  , ZP          },
    { "dec"  , ZP          },
    { "???"  , ILLEGAL     },
    { "iny"  , IMPLIED     },
    { "cmp"  , IMMEDIATE   },
    { "dex"  , IMPLIED     },
    { "???"  , ILLEGAL     },
    { "cpy"  , ABS         },
    { "cmp"  , ABS         },
    { "dec"  , ABS         },
    { "???"  , ILLEGAL     },

    { "bne"  , REL         }, /* 0xd0 to 0xdf */
    { "cmp"  , ZP_IND_Y    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "cmp"  , ZP_X        },
    { "dec"  , ZP_X        },
    { "???"  , ILLEGAL     },
    { "cld"  , IMPLIED     },
    { "cmp"  , ABS_Y       },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "cmp"  , ABS_X       },
    { "dec"  , ABS_X       },
    { "???"  , ILLEGAL     },

    { "cpx"  , IMMEDIATE   }, /* 0xe0 to 0xef */
    { "sbc"  , ZP_X_IND    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "cpx"  , ZP          },
    { "sbc"  , ZP          },
    { "inc"  , ZP          },
    { "???"  , ILLEGAL     },
    { "inx"  , IMPLIED     },
    { "sbc"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "???"  , ILLEGAL     },
    { "cpx"  , ABS         },
    { "sbc"  , ABS         },
    { "inc"  , ABS         },
    { "???"  , ILLEGAL     },

    { "beq"  , REL         }, /* 0xf0 to 0xff */
    { "sbc"  , ZP_IND_Y    },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "sbc"  , ZP_X        },
    { "inc"  , ZP_X        },
    { "???"  , ILLEGAL     },
    { "sed"  , IMPLIED     },
    { "sbc"  , ABS_Y       },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "???"  , ILLEGAL     },
    { "sbc"  , ABS_X       },
    { "inc"  , ABS_X       },
    { "???"  , ILLEGAL     }
};

/* Information for 65C02 opcodes. */
static InstructionInfo II_65C02[256] = {
    { "brk"  , IMPLIED     }, /* 0x00 to 0x0f */
    { "ora"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "tsb"  , ZP          },
    { "ora"  , ZP          },
    { "asl"  , ZP          },
    { "rmb0" , ZP          },
    { "php"  , IMPLIED     },
    { "ora"  , IMMEDIATE   },
    { "asl"  , ACCUMULATOR },
    { "nop"  , IMPLIED     },
    { "tsb"  , ABS         },
    { "ora"  , ABS         },
    { "asl"  , ABS         },
    { "bbr0" , ZP_REL      },

    { "bpl"  , REL         }, /* 0x10 to 0x1f */
    { "ora"  , ZP_IND_Y    },
    { "ora"  , ZP_IND      },
    { "nop"  , IMPLIED     },
    { "trb"  , ZP          },
    { "ora"  , ZP_X        },
    { "asl"  , ZP_X        },
    { "rmb1" , ZP          },
    { "clc"  , IMPLIED     },
    { "ora"  , ABS_Y       },
    { "inc"  , ACCUMULATOR },
    { "nop"  , IMPLIED     },
    { "trb"  , ABS         },
    { "ora"  , ABS_X       },
    { "asl"  , ABS_X       },
    { "bbr1" , ZP_REL      },

    { "jsr"  , ABS         }, /* 0x20 to 0x2f */
    { "and"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "bit"  , ZP          },
    { "and"  , ZP          },
    { "rol"  , ZP          },
    { "rmb2" , ZP          },
    { "plp"  , IMPLIED     },
    { "and"  , IMMEDIATE   },
    { "rol"  , ACCUMULATOR },
    { "nop"  , IMPLIED     },
    { "bit"  , ABS         },
    { "and"  , ABS         },
    { "rol"  , ABS         },
    { "bbr2" , ZP_REL      },

    { "bmi"  , REL         }, /* 0x30 to 0x3f */
    { "and"  , ZP_IND_Y    },
    { "and"  , ZP_IND      },
    { "nop"  , IMPLIED     },
    { "bit"  , ZP_X        },
    { "and"  , ZP_X        },
    { "rol"  , ZP_X        },
    { "rmb3" , ZP          },
    { "sec"  , IMPLIED     },
    { "and"  , ABS_Y       },
    { "dec"  , ACCUMULATOR },
    { "nop"  , IMPLIED     },
    { "bit"  , ABS_X       },
    { "and"  , ABS_X       },
    { "rol"  , ABS_X       },
    { "bbr3" , ZP_REL      },

    { "rti"  , IMPLIED     }, /* 0x40 to 0x4f */
    { "eor"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "nop"  , ILLEGAL     },
    { "nop"  , ZP          },
    { "eor"  , ZP          },
    { "lsr"  , ZP          },
    { "rmb4" , ZP          },
    { "pha"  , IMPLIED     },
    { "eor"  , IMMEDIATE   },
    { "lsr"  , ACCUMULATOR },
    { "nop"  , IMPLIED     },
    { "jmp"  , ABS         },
    { "eor"  , ABS         },
    { "lsr"  , ABS         },
    { "bbr4" , ZP_REL      },

    { "bvc"  , REL         }, /* 0x50 to 0x5f */
    { "eor"  , ZP_IND_Y    },
    { "eor"  , ZP_IND      },
    { "nop"  , IMPLIED     },
    { "nop"  , ZP_X        },
    { "eor"  , ZP_X        },
    { "lsr"  , ZP_X        },
    { "rmb5" , ZP          },
    { "cli"  , IMPLIED     },
    { "eor"  , ABS_Y       },
    { "phy"  , IMPLIED     },
    { "nop"  , IMPLIED     },
    { "nop"  , ABS         },
    { "eor"  , ABS_X       },
    { "lsr"  , ABS_X       },
    { "bbr5" , ZP_REL      },

    { "rts"  , IMPLIED     }, /* 0x60 to 0x6f */
    { "adc"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "stz"  , ZP          },
    { "adc"  , ZP          },
    { "ror"  , ZP          },
    { "rmb6" , ZP          },
    { "pla"  , IMPLIED     },
    { "adc"  , IMMEDIATE   },
    { "ror"  , ACCUMULATOR },
    { "nop"  , IMPLIED     },
    { "jmp"  , ABS_IND     },
    { "adc"  , ABS         },
    { "ror"  , ABS         },
    { "bbr6" , ZP_REL      },

    { "bvs"  , REL         }, /* 0x70 to 0x7f */
    { "adc"  , ZP_IND_Y    },
    { "adc"  , ZP_IND      },
    { "nop"  , IMPLIED     },
    { "stz"  , ZP_X        },
    { "adc"  , ZP_X        },
    { "ror"  , ZP_X        },
    { "rmb7" , ZP          },
    { "sei"  , IMPLIED     },
    { "adc"  , ABS_Y       },
    { "ply"  , IMPLIED     },
    { "nop"  , IMPLIED     },
    { "jmp"  , ABS_X_IND   },
    { "adc"  , ABS_X       },
    { "ror"  , ABS_X       },
    { "bbr7" , ZP_REL      },

    { "bra"  , REL         }, /* 0x80 to 0x8f */
    { "sta"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "sty"  , ZP          },
    { "sta"  , ZP          },
    { "stx"  , ZP          },
    { "smb0" , ZP          },
    { "dey"  , IMPLIED     },
    { "bit"  , IMMEDIATE   },
    { "txa"  , IMPLIED     },
    { "nop"  , IMPLIED     },
    { "sty"  , ABS         },
    { "sta"  , ABS         },
    { "stx"  , ABS         },
    { "bbs0" , ZP_REL      },

    { "bcc"  , REL         }, /* 0x90 to 0x9f */
    { "sta"  , ZP_IND_Y    },
    { "sta"  , ZP_IND      },
    { "nop"  , IMPLIED     },
    { "sty"  , ZP_X        },
    { "sta"  , ZP_X        },
    { "stx"  , ZP_Y        },
    { "smb1" , ZP          },
    { "tya"  , IMPLIED     },
    { "sta"  , ABS_Y       },
    { "txs"  , IMPLIED     },
    { "nop"  , IMPLIED     },
    { "stz"  , ABS         },
    { "sta"  , ABS_X       },
    { "stz"  , ABS_X       },
    { "bbs1" , ZP_REL      },

    { "ldy"  , IMMEDIATE   }, /* 0xa0 to 0xaf */
    { "lda"  , ZP_X_IND    },
    { "ldx"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "ldy"  , ZP          },
    { "lda"  , ZP          },
    { "ldx"  , ZP          },
    { "smb2" , ZP          },
    { "tay"  , IMPLIED     },
    { "lda"  , IMMEDIATE   },
    { "tax"  , IMPLIED     },
    { "nop"  , IMPLIED     },
    { "ldy"  , ABS         },
    { "lda"  , ABS         },
    { "ldx"  , ABS         },
    { "bbs2" , ZP_REL      },

    { "bcs"  , REL         }, /* 0xb0 to 0xbf */
    { "lda"  , ZP_IND_Y    },
    { "lda"  , ZP_IND      },
    { "nop"  , IMPLIED     },
    { "ldy"  , ZP_X        },
    { "lda"  , ZP_X        },
    { "ldx"  , ZP_Y        },
    { "smb3" , ZP          },
    { "clv"  , IMPLIED     },
    { "lda"  , ABS_Y       },
    { "tsx"  , IMPLIED     },
    { "nop"  , IMPLIED     },
    { "ldy"  , ABS_X       },
    { "lda"  , ABS_X       },
    { "ldx"  , ABS_Y       },
    { "bbs3" , ZP_REL      },

    { "cpy"  , IMMEDIATE   }, /* 0xc0 to 0xcf */
    { "cmp"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "cpy"  , ZP          },
    { "cmp"  , ZP          },
    { "dec"  , ZP          },
    { "smb4" , ZP          },
    { "iny"  , IMPLIED     },
    { "cmp"  , IMMEDIATE   },
    { "dex"  , IMPLIED     },
    { "wai"  , IMPLIED     },
    { "cpy"  , ABS         },
    { "cmp"  , ABS         },
    { "dec"  , ABS         },
    { "bbs4" , ZP_REL      },

    { "bne"  , REL         }, /* 0xd0 to 0xdf */
    { "cmp"  , ZP_IND_Y    },
    { "cmp"  , ZP_IND      },
    { "nop"  , IMPLIED     },
    { "nop"  , ZP_X        },
    { "cmp"  , ZP_X        },
    { "dec"  , ZP_X        },
    { "smb5" , ZP          },
    { "cld"  , IMPLIED     },
    { "cmp"  , ABS_Y       },
    { "phx"  , IMPLIED     },
    { "stp"  , IMPLIED     },
    { "nop"  , ABS         },
    { "cmp"  , ABS_X       },
    { "dec"  , ABS_X       },
    { "bbs5" , ZP_REL      },

    { "cpx"  , IMMEDIATE   }, /* 0xe0 to 0xef */
    { "sbc"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "cpx"  , ZP          },
    { "sbc"  , ZP          },
    { "inc"  , ZP          },
    { "smb6" , ZP          },
    { "inx"  , IMPLIED     },
    { "sbc"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "nop"  , IMPLIED     },
    { "cpx"  , ABS         },
    { "sbc"  , ABS         },
    { "inc"  , ABS         },
    { "bbs6" , ZP_REL      },

    { "beq"  , REL         }, /* 0xf0 to 0xff */
    { "sbc"  , ZP_IND_Y    },
    { "sbc"  , ZP_IND      },
    { "nop"  , IMPLIED     },
    { "nop"  , ZP_X        },
    { "sbc"  , ZP_X        },
    { "inc"  , ZP_X        },
    { "smb7" , ZP          },
    { "sed"  , IMPLIED     },
    { "sbc"  , ABS_Y       },
    { "plx"  , IMPLIED     },
    { "nop"  , IMPLIED     },
    { "nop"  , ABS         },
    { "sbc"  , ABS_X       },
    { "inc"  , ABS_X       },
    { "bbs7" , ZP_REL      }
};

/* Information for 6502X (6502 with undocumented instructions) opcodes. */
static InstructionInfo II_6502X[256] = {
    { "brk"  , IMPLIED     }, /* 0x00 to 0x0f */
    { "ora"  , ZP_X_IND    },
    { "jam"  , IMPLIED     },
    { "slo"  , ZP_X_IND    },
    { "nop"  , ZP          },
    { "ora"  , ZP          },
    { "asl"  , ZP          },
    { "slo"  , ZP          },
    { "php"  , IMPLIED     },
    { "ora"  , IMMEDIATE   },
    { "asl"  , ACCUMULATOR },
    { "anc"  , IMMEDIATE   },
    { "nop"  , ABS         },
    { "ora"  , ABS         },
    { "asl"  , ABS         },
    { "slo"  , ABS         },

    { "bpl"  , REL         }, /* 0x10 to 0x1f */
    { "ora"  , ZP_IND_Y    },
    { "jam"  , IMPLIED     },
    { "slo"  , ZP_IND_Y    },
    { "nop"  , ZP_X        },
    { "ora"  , ZP_X        },
    { "asl"  , ZP_X        },
    { "slo"  , ZP          },
    { "clc"  , IMPLIED     },
    { "ora"  , ABS_Y       },
    { "nop"  , IMPLIED     },
    { "slo"  , ABS_Y       },
    { "*nop" , ABS_X       },
    { "ora"  , ABS_X       },
    { "asl"  , ABS_X       },
    { "slo"  , ABS_X       },

    { "jsr"  , ABS         }, /* 0x20 to 0x2f */
    { "and"  , ZP_X_IND    },
    { "jam"  , IMPLIED     },
    { "rla"  , ZP_X_IND    },
    { "bit"  , ZP          },
    { "and"  , ZP          },
    { "rol"  , ZP          },
    { "rla"  , ZP          },
    { "plp"  , IMPLIED     },
    { "and"  , IMMEDIATE   },
    { "rol"  , ACCUMULATOR },
    { "anc"  , IMMEDIATE   },
    { "bit"  , ABS         },
    { "and"  , ABS         },
    { "rol"  , ABS         },
    { "rla"  , ABS         },

    { "bmi"  , REL         }, /* 0x30 to 0x3f */
    { "and"  , ZP_IND_Y    },
    { "jam"  , IMPLIED     },
    { "rla"  , ZP_IND_Y    },
    { "nop"  , ZP_X        },
    { "and"  , ZP_X        },
    { "rol"  , ZP_X        },
    { "rla"  , ZP_X        },
    { "sec"  , IMPLIED     },
    { "and"  , ABS_Y       },
    { "nop"  , IMPLIED     },
    { "rla"  , ABS_Y       },
    { "nop"  , ABS_X       },
    { "and"  , ABS_X       },
    { "rol"  , ABS_X       },
    { "rla"  , ABS_X       },

    { "rti"  , IMPLIED     }, /* 0x40 to 0x4f */
    { "eor"  , ZP_X_IND    },
    { "jam"  , IMPLIED     },
    { "sre"  , ZP_X_IND    },
    { "nop"  , ZP          },
    { "eor"  , ZP          },
    { "lsr"  , ZP          },
    { "sre"  , ZP          },
    { "pha"  , IMPLIED     },
    { "eor"  , IMMEDIATE   },
    { "lsr"  , ACCUMULATOR },
    { "alr"  , IMMEDIATE   },
    { "jmp"  , ABS         },
    { "eor"  , ABS         },
    { "lsr"  , ABS         },
    { "sre"  , ABS         },

    { "bvc"  , REL         }, /* 0x50 to 0x5f */
    { "eor"  , ZP_IND_Y    },
    { "jam"  , IMPLIED     },
    { "sre"  , ZP_IND_Y    },
    { "nop"  , ZP_X        },
    { "eor"  , ZP_X        },
    { "lsr"  , ZP_X        },
    { "sre"  , ZP_X        },
    { "cli"  , IMPLIED     },
    { "eor"  , ABS_Y       },
    { "nop"  , IMPLIED     },
    { "sre"  , ABS_Y       },
    { "nop"  , ABS_X       },
    { "eor"  , ABS_X       },
    { "lsr"  , ABS_X       },
    { "sre"  , ABS_X       },

    { "rts"  , IMPLIED     }, /* 0x60 to 0x6f */
    { "adc"  , ZP_X_IND    },
    { "jam"  , IMPLIED     },
    { "rra"  , ZP_X_IND    },
    { "nop"  , ZP          },
    { "adc"  , ZP          },
    { "ror"  , ZP          },
    { "rra"  , ZP          },
    { "pla"  , IMPLIED     },
    { "adc"  , IMMEDIATE   },
    { "ror"  , ACCUMULATOR },
    { "arr"  , IMMEDIATE   },
    { "jmp"  , ABS_IND     },
    { "adc"  , ABS         },
    { "ror"  , ABS         },
    { "rra"  , ABS         },

    { "bvs"  , REL         }, /* 0x70 to 0x7f */
    { "adc"  , ZP_IND_Y    },
    { "jam"  , IMPLIED     },
    { "sre"  , ZP_IND_Y    },
    { "nop"  , ZP_X        },
    { "adc"  , ZP_X        },
    { "ror"  , ZP_X        },
    { "rra"  , ZP_X        },
    { "sei"  , IMPLIED     },
    { "adc"  , ABS_Y       },
    { "nop"  , IMPLIED     },
    { "rra"  , ABS_Y       },
    { "nop"  , ABS_X       },
    { "adc"  , ABS_X       },
    { "ror"  , ABS_X       },
    { "rra"  , ABS_X       },

    { "nop"  , IMMEDIATE   }, /* 0x80 to 0x8f */
    { "sta"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "sax"  , ZP_X_IND    },
    { "sty"  , ZP          },
    { "sta"  , ZP          },
    { "stx"  , ZP          },
    { "sax"  , ZP          },
    { "dey"  , IMPLIED     },
    { "nop"  , IMMEDIATE   },
    { "txa"  , IMPLIED     },
    { "ane"  , IMMEDIATE   },
    { "sty"  , ABS         },
    { "sta"  , ABS         },
    { "stx"  , ABS         },
    { "sax"  , ABS         },

    { "bcc"  , REL         }, /* 0x90 to 0x9f */
    { "sta"  , ZP_IND_Y    },
    { "jam"  , IMPLIED     },
    { "sha"  , ZP_IND_Y    },
    { "sty"  , ZP_X        },
    { "sta"  , ZP_X        },
    { "stx"  , ZP_Y        },
    { "sax"  , ZP_Y        },
    { "tya"  , IMPLIED     },
    { "sta"  , ABS_Y       },
    { "txs"  , IMPLIED     },
    { "tas"  , ABS_Y       },
    { "shy"  , ABS_X       },
    { "sta"  , ABS_X       },
    { "shx"  , ABS_Y       },
    { "sha"  , ABS_Y       },

    { "ldy"  , IMMEDIATE   }, /* 0xa0 to 0xaf */
    { "lda"  , ZP_X_IND    },
    { "ldx"  , IMMEDIATE   },
    { "lax"  , ZP_X_IND    },
    { "ldy"  , ZP          },
    { "lda"  , ZP          },
    { "ldx"  , ZP          },
    { "lax"  , ZP          },
    { "tay"  , IMPLIED     },
    { "lda"  , IMMEDIATE   },
    { "tax"  , IMPLIED     },
    { "lax"  , IMMEDIATE   },
    { "ldy"  , ABS         },
    { "lda"  , ABS         },
    { "ldx"  , ABS         },
    { "lax"  , ABS         },

    { "bcs"  , REL         }, /* 0xb0 to 0xbf */
    { "lda"  , ZP_IND_Y    },
    { "jam"  , IMPLIED     },
    { "lax"  , ZP_IND_Y    },
    { "ldy"  , ZP_X        },
    { "lda"  , ZP_X        },
    { "ldx"  , ZP_Y        },
    { "lax"  , ZP_Y        },
    { "clv"  , IMPLIED     },
    { "lda"  , ABS_Y       },
    { "tsx"  , IMPLIED     },
    { "las"  , ABS_Y       },
    { "ldy"  , ABS_X       },
    { "lda"  , ABS_X       },
    { "ldx"  , ABS_Y       },
    { "lax"  , ABS_Y       },

    { "cpy"  , IMMEDIATE   }, /* 0xc0 to 0xcf */
    { "cmp"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "dcp"  , ZP_X_IND    },
    { "cpy"  , ZP          },
    { "cmp"  , ZP          },
    { "dec"  , ZP          },
    { "dcp"  , ZP          },
    { "iny"  , IMPLIED     },
    { "cmp"  , IMMEDIATE   },
    { "dex"  , IMPLIED     },
    { "sbx"  , IMMEDIATE   },
    { "cpy"  , ABS         },
    { "cmp"  , ABS         },
    { "dec"  , ABS         },
    { "dcp"  , ABS         },

    { "bne"  , REL         }, /* 0xd0 to 0xdf */
    { "cmp"  , ZP_IND_Y    },
    { "jam"  , IMPLIED     },
    { "dcp"  , ZP_IND_Y    },
    { "nop"  , ZP_X        },
    { "cmp"  , ZP_X        },
    { "dec"  , ZP_X        },
    { "dcp"  , ZP_X        },
    { "cld"  , IMPLIED     },
    { "cmp"  , ABS_Y       },
    { "nop"  , IMPLIED     },
    { "dcp"  , ABS_Y       },
    { "nop"  , ABS_X       },
    { "cmp"  , ABS_X       },
    { "dec"  , ABS_X       },
    { "dcp"  , ABS_X       },

    { "cpx"  , IMMEDIATE   }, /* 0xe0 to 0xef */
    { "sbc"  , ZP_X_IND    },
    { "nop"  , IMMEDIATE   },
    { "isc"  , ZP_X_IND    },
    { "cpx"  , ZP          },
    { "sbc"  , ZP          },
    { "inc"  , ZP          },
    { "isc"  , ZP          },
    { "inx"  , IMPLIED     },
    { "sbc"  , IMMEDIATE   },
    { "nop"  , IMPLIED     },
    { "sbc"  , IMMEDIATE   },
    { "cpx"  , ABS         },
    { "sbc"  , ABS         },
    { "inc"  , ABS         },
    { "isc"  , ABS         },

    { "beq"  , REL         }, /* 0xf0 to 0xff */
    { "sbc"  , ZP_IND_Y    },
    { "jam"  , IMPLIED     },
    { "isc"  , ZP_IND_Y    },
    { "nop"  , ZP_X        },
    { "sbc"  , ZP_X        },
    { "inc"  , ZP_X        },
    { "isc"  , ZP_X        },
    { "sed"  , IMPLIED     },
    { "sbc"  , ABS_Y       },
    { "nop"  , IMPLIED     },
    { "isc"  , ABS_Y       },
    { "nop"  , ABS_X       },
    { "sbc"  , ABS_X       },
    { "inc"  , ABS_X       },
    { "isc"  , ABS_X       }
};

static InstructionInfo * II[3] = { II_6502, II_65C02, II_6502X };

static unsigned GetInstructionLength (uint8_t opcode)
/* Get the number of bytes in the full instruction. Depends on the addressing mode. */
{
    switch (II[CPU][opcode].adrmode) {
        case ILLEGAL:
        case IMPLIED:
        case ACCUMULATOR:
            return 1;
        case IMMEDIATE:
        case REL:
        case ZP:
        case ZP_X:
        case ZP_Y:
        case ZP_IND:
        case ZP_X_IND:
        case ZP_IND_Y:
            return 2;
        case ZP_REL:
        case ABS:
        case ABS_X:
        case ABS_Y:
        case ABS_IND:
        case ABS_X_IND:
            return 3;
    }

    /* We should never get here. */
    assert(false);
}



static char * PrintAssemblyInstruction (char * ptr)
/* Print assembly instruction: mnemonic and addres-mode specific operand(s). */
{
    uint8_t opcode;

    /* Print the instruction starting at the current program counter. */

    opcode = MemReadByte (Regs.PC);

    ptr += sprintf (ptr, "%-4s ", II[CPU][opcode].mnemonic);

    switch (II[CPU][opcode].adrmode) {
        case IMPLIED:
        case ILLEGAL:
            break;
        case ACCUMULATOR:
            ptr += sprintf (ptr, "A");
            break;
        case IMMEDIATE:
            ptr += sprintf (ptr, "#$%02X", MemReadByte (Regs.PC + 1));
            break;
        case REL:
            ptr += sprintf (ptr, "$%04X", Regs.PC + 2 + (int8_t)MemReadByte (Regs.PC + 1));
            break;
        case ZP:
            ptr += sprintf (ptr, "$%02X", MemReadByte (Regs.PC + 1));
            break;
        case ZP_X:
            ptr += sprintf (ptr, "$%02X,X", MemReadByte (Regs.PC + 1));
            break;
        case ZP_Y:
            ptr += sprintf (ptr, "$%02X,Y", MemReadByte (Regs.PC + 1));
            break;
        case ZP_IND:
            ptr += sprintf (ptr, "($%02X)", MemReadByte (Regs.PC + 1));
            break;
        case ZP_X_IND:
            ptr += sprintf (ptr, "($%02X,X)", MemReadByte (Regs.PC + 1));
            break;
        case ZP_IND_Y:
            ptr += sprintf (ptr, "($%02X),Y", MemReadByte (Regs.PC + 1));
            break;
        case ZP_REL:
            ptr += sprintf (ptr, "$%02X,$%04X", MemReadByte (Regs.PC + 1), Regs.PC + 3 + (int8_t)MemReadByte (Regs.PC + 2));
            break;
        case ABS:
            ptr += sprintf (ptr, "$%04X", MemReadWord (Regs.PC + 1));
            break;
        case ABS_IND:
            ptr += sprintf (ptr, "($%04X)", MemReadWord (Regs.PC + 1));
            break;
        case ABS_X:
            ptr += sprintf (ptr, "$%04X,X", MemReadWord (Regs.PC + 1));
            break;
        case ABS_X_IND:
            ptr += sprintf (ptr, "($%04X,X)", MemReadWord (Regs.PC + 1));
            break;
        case ABS_Y:
            ptr += sprintf (ptr, "$%04X,Y", MemReadWord (Regs.PC + 1));
            break;
    }

    return ptr;
}



static void PrintTraceInstructionOrInterrupt (const char * InterruptType)
{
    char traceline[200];
    char * traceline_ptr = traceline;
    uint8_t opcode;
    unsigned k, num_bytes;

    if (TraceMode & TRACE_FIELD_INSTR_COUNTER) {

        if (traceline_ptr != traceline) {
            /* Print field separator. */
            traceline_ptr += sprintf (traceline_ptr, "  ");
        }

        traceline_ptr += sprintf (traceline_ptr, "%12" PRIu64, Peripherals.Counter.CpuInstructions);
    }

    if (TraceMode & TRACE_FIELD_CLOCK_COUNTER) {

        if (traceline_ptr != traceline) {
            /* Print field separator. */
            traceline_ptr += sprintf (traceline_ptr, "  ");
        }

        traceline_ptr += sprintf (traceline_ptr, "%12" PRIu64, Peripherals.Counter.ClockCycles);
    }

    if (TraceMode & TRACE_FIELD_PC) {

        if (traceline_ptr != traceline) {
            /* Print field separator. */
            traceline_ptr += sprintf (traceline_ptr, "  ");
        }

        traceline_ptr += sprintf (traceline_ptr, "%04X", Regs.PC);
    }

    if (TraceMode & TRACE_FIELD_INSTR_BYTES) {

        if (traceline_ptr != traceline) {
            /* Print field separator. */
            traceline_ptr += sprintf (traceline_ptr, "  ");
        }

        if (InterruptType == NULL)
        {
            /* Get the opcode */
            opcode = MemReadByte (Regs.PC);

            /* How many bytes are in the full instruction? 1, 2 or 3. */
            num_bytes = GetInstructionLength (opcode);
        } else {
            num_bytes = 0; /* Consider interrupts as instructions that are inserted into the instruction stream. */
        }

        /* Print 0 to 3 bytes for the interrupt/instruction. */
        for (k = 0; k < 3; ++k) {
            if (k != 0) {
                *traceline_ptr++ = ' ';
            }
            if (k < num_bytes) {
                traceline_ptr += sprintf (traceline_ptr, "%02X", MemReadByte (Regs.PC + k));
            } else {
                traceline_ptr += sprintf (traceline_ptr, "  ");
            }
        }
    }

    if (TraceMode & TRACE_FIELD_INSTR_ASSEMBLY) {

        if (traceline_ptr != traceline) {
            /* Print field separator. */
            traceline_ptr += sprintf (traceline_ptr, "  ");
        }

        char * save_ptr = traceline_ptr;

        if (InterruptType == NULL) {
            traceline_ptr = PrintAssemblyInstruction (traceline_ptr);
        } else {
            /* Print interrupt message. */
            traceline_ptr += sprintf (traceline_ptr, "*** %s ***", InterruptType);
        }

        /* Fill out the field to 16 characters */
        num_bytes = traceline_ptr - save_ptr;
        if (num_bytes < 16) {
            traceline_ptr += sprintf (traceline_ptr, "%*s", 16 - num_bytes, "");
        }
    }

    if (TraceMode & TRACE_FIELD_CPU_REGISTERS) {

        if (traceline_ptr != traceline) {
            /* Print field separator. */
            traceline_ptr += sprintf (traceline_ptr, "  ");
        }

        traceline_ptr += sprintf (traceline_ptr,
            "A=%02X X=%02X Y=%02X S=%02X Flags=%c%c%c%c%c%c",
            Regs.AC,
            Regs.XR,
            Regs.YR,
            Regs.SP,
            (Regs.SR & SF) ? 'N' : 'n',
            (Regs.SR & OF) ? 'V' : 'v',
            (Regs.SR & DF) ? 'D' : 'd',
            (Regs.SR & IF) ? 'I' : 'i',
            (Regs.SR & ZF) ? 'Z' : 'z',
            (Regs.SR & CF) ? 'C' : 'c'
        );
    }

    if (TraceMode & TRACE_FIELD_CC65_SP) {

        if (traceline_ptr != traceline) {
            /* Print field separator. */
            traceline_ptr += sprintf (traceline_ptr, "  ");
        }

        traceline_ptr += sprintf (traceline_ptr,
            "  SP=%04X",
            MemReadZPWord (StackPointerZPageAddress)
        );
    }

    if (traceline_ptr != traceline) {
        puts (traceline);
    }
}



void TraceInit (uint8_t SPAddr)
{
    StackPointerZPageAddress = SPAddr;
}



void PrintTraceNMI (void)
{
    PrintTraceInstructionOrInterrupt("NMI");
}



void PrintTraceIRQ (void)
{
    PrintTraceInstructionOrInterrupt("IRQ");
}



void PrintTraceInstruction (void)
{
    PrintTraceInstructionOrInterrupt(NULL);
}
