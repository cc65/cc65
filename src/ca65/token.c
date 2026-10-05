/*****************************************************************************/
/*                                                                           */
/*                                  token.c                                  */
/*                                                                           */
/*                  Token list for the ca65 macro assembler                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2007-2011, Ullrich von Bassewitz                                      */
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



#include <stdio.h>

/* ca65 */
#include "token.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Use a struct so we can extend it if required */
typedef struct TokDescEntry TokDescEntry;
struct TokDescEntry {
    const char*         Desc;
};

static const TokDescEntry TokDesc[] = {
    { "none",                           },
    { "end-of-file",                    },
    { "end-of-line",                    },
    { "{s}",                            },
    { "{s}",                            },

    { "{i}",                            },
    { "'{c}'",                          },
    { "\"{s}\"",                        },

    { "A",                              },
    { "Q",                              },
    { "S",                              },
    { "X",                              },
    { "Y",                              },
    { "Z",                              },
    { "R{i}",                           },

    { ":=",                             },
    { "unnamed label",                  },

    { "=",                              },
    { "<>",                             },
    { "<",                              },
    { ">",                              },
    { "<=",                             },
    { ">=",                             },

    { ".AND",                           },
    { ".OR",                            },
    { ".XOR",                           },
    { ".NOT",                           },

    { "+",                              },
    { "-",                              },
    { "*",                              },
    { "/",                              },
    { "!",                              },
    { "|",                              },
    { "^",                              },
    { "&",                              },
    { "<<",                             },
    { ">>",                             },
    { "~",                              },

    { "$",                              },
    { "::",                             },
    { ".",                              },
    { ",",                              },
    { "#",                              },
    { ":",                              },
    { "(",                              },
    { ")",                              },
    { "[",                              },
    { "]",                              },
    { "{",                              },
    { "}",                              },
    { "@",                              },

    { "z:",                             },
    { "a:",                             },
    { "f:",                             },

    { "macro parameter",                },
    { "repeat counter",                 },

    { ".A16"                            },
    { ".A8"                             },
    { ".ADDR"                           },
    { ".ADDRSIZE"                       },
    { ".ALIGN"                          },
    { ".ASCIIZ"                         },
    { ".ASIZE"                          },
    { ".ASSERT"                         },
    { ".AUTOIMPORT"                     },
    { ".BANK"                           },
    { ".BANKBYTE"                       },
    { ".BANKBYTES"                      },
    { ".BANKS"                          },
    { ".BLANK"                          },
    { ".BSS"                            },
    { ".BYTE"                           },
    { ".CAP"                            },
    { ".CASE"                           },
    { ".CHARMAP"                        },
    { ".CODE"                           },
    { ".CONCAT"                         },
    { ".CONDES"                         },
    { ".CONST"                          },
    { ".CONSTRUCTOR"                    },
    { ".CPU"                            },
    { ".DATA"                           },
    { ".DBG"                            },
    { ".DBYT"                           },
    { ".DEBUGINFO"                      },
    { ".DEFINE"                         },
    { ".DEFINED"                        },
    { ".DEFINEDMACRO"                   },
    { ".DELMAC"                         },
    { ".DESTRUCTOR"                     },
    { ".DWORD"                          },
    { ".ELSE"                           },
    { ".ELSEIF"                         },
    { ".END"                            },
    { ".ENDENUM"                        },
    { ".ENDIF"                          },
    { ".ENDMACRO"                       },
    { ".ENDPROC"                        },
    { ".ENDREP"                         },
    { ".ENDSCOPE"                       },
    { ".ENDSTRUCT"                      },
    { ".ENDUNION"                       },
    { ".ENUM"                           },
    { ".ERROR"                          },
    { ".EXITMACRO"                      },
    { ".EXPORT"                         },
    { ".EXPORTZP"                       },
    { ".FARADDR"                        },
    { ".FATAL"                          },
    { ".FEATURE"                        },
    { ".FILEOPT"                        },
    { ".FORCEIMPORT"                    },
    { ".FORCEWORD"                      },
    { ".GLOBAL"                         },
    { ".GLOBALZP"                       },
    { ".HIBYTE"                         },
    { ".HIBYTES"                        },
    { ".HIWORD"                         },
    { ".I16"                            },
    { ".I8"                             },
    { ".MAKEIDENT"                      },
    { ".IF"                             },
    { ".IFBLANK"                        },
    { ".IFCONST"                        },
    { ".IFDEF"                          },
    { ".IFNBLANK"                       },
    { ".IFNCONST"                       },
    { ".IFNDEF"                         },
    { ".IFNREF"                         },
    { ".IFP02"                          },
    { ".IFP02X"                         },
    { ".IFP4510"                        },
    { ".IFP45GS02"                      },
    { ".IFP6280"                        },
    { ".IFP816"                         },
    { ".IFPC02"                         },
    { ".IFPCE02"                        },
    { ".IFPDTV"                         },
    { ".IFPM740"                        },
    { ".IFPSC02"                        },
    { ".IFPSWEET16"                     },
    { ".IFPWC02"                        },
    { ".IFREF"                          },
    { ".IMPORT"                         },
    { ".IMPORTZP"                       },
    { ".INCBIN"                         },
    { ".INCLUDE"                        },
    { ".INTERRUPTOR"                    },
    { ".ISIZE"                          },
    { ".ISMNEMONIC"                     },
    { ".LEFT"                           },
    { ".LINECONT"                       },
    { ".LIST"                           },
    { ".LISTBYTES"                      },
    { ".LITERAL"                        },
    { ".LOBYTE"                         },
    { ".LOBYTES"                        },
    { ".LOCAL"                          },
    { ".LOCALCHAR"                      },
    { ".LOWORD"                         },
    { ".MACPACK"                        },
    { ".MACRO"                          },
    { ".MATCH"                          },
    { ".MAX"                            },
    { ".MID"                            },
    { ".MIN"                            },
    { ".NULL"                           },
    { ".ORG"                            },
    { ".OUT"                            },
    { ".P02"                            },
    { ".P02X"                           },
    { ".P4510"                          },
    { ".P45GS02"                        },
    { ".P6280"                          },
    { ".P816"                           },
    { ".PAGELENGTH"                     },
    { ".PARAMCOUNT"                     },
    { ".PC02"                           },
    { ".PCE02"                          },
    { ".PDTV"                           },
    { ".PM740"                          },
    { ".POPCHARMAP"                     },
    { ".POPCPU"                         },
    { ".POPSEG"                         },
    { ".PROC"                           },
    { ".PSC02"                          },
    { ".PSWEET16"                       },
    { ".PUSHCHARMAP"                    },
    { ".PUSHCPU"                        },
    { ".PUSHSEG"                        },
    { ".PWC02"                          },
    { ".REFERENCED"                     },
    { ".REFERTO"                        },
    { ".RELOC"                          },
    { ".REPEAT"                         },
    { ".RES"                            },
    { ".RIGHT"                          },
    { ".RODATA"                         },
    { ".SCOPE"                          },
    { ".SEGMENT"                        },
    { ".SET"                            },
    { ".SETCPU"                         },
    { ".SIZEOF"                         },
    { ".SMART"                          },
    { ".SPRINTF"                        },
    { ".STRAT"                          },
    { ".STRING"                         },
    { ".STRLEN"                         },
    { ".STRUCT"                         },
    { ".TAG"                            },
    { ".TCOUNT"                         },
    { ".TIME"                           },
    { ".UNDEF"                          },
    { ".UNION"                          },
    { ".VERSION"                        },
    { ".WARNING"                        },
    { ".WORD"                           },
    { ".XMATCH"                         },
    { ".ZEROPAGE"                       },
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void CopyToken (Token* Dst, const Token* Src)
/* Copy a token from Src to Dst. The current value of Dst.SVal is free'd,
** so Dst must be initialized.
*/
{
    /* Copy the fields */
    Dst->Tok  = Src->Tok;
    Dst->WS   = Src->WS;
    Dst->IVal = Src->IVal;
    SB_Copy (&Dst->SVal, &Src->SVal);
    Dst->Pos  = Src->Pos;
}



StrBuf* TokenDesc (const Token* T, StrBuf* S)
/* Place a textual description of the given token into S. */
{
    PRECONDITION (sizeof (TokDesc) / sizeof (TokDesc[0]) == TOK_COUNT);

    /* Clear the target buffer */
    SB_Clear (S);

    /* Get the description for the token */
    const char* Desc = TokDesc[T->Tok].Desc;

    /* Repeatedly replace {c}, {i} and {s} */
    size_t Start = 0;
    while (1) {
        const char* P = strchr (Desc + Start, '{');
        if (P) {
            /* Check if this is really {c}, {i} or {s} */
            if ((P[1] != 'c' &&
                 P[1] != 'i' &&
                 P[1] != 's') ||
                P[2] != '}') {
                ++Start;
                continue;
            }
            /* Append the text before the replacement token */
            SB_AppendBuf (S, Desc + Start, P - (Desc + Start));
            Start += P - (Desc + Start) + 3;
            /* Append the replacement text */
            if (P[1] == 'c') {
                SB_AppendChar (S, (char)T->IVal);
            } else if (P[1] == 'i') {
                char Buf[64];
                snprintf (Buf, sizeof (Buf), "%ld", T->IVal);
                SB_AppendStr (S, Buf);
            } else {
                SB_Append (S, &T->SVal);
            }
        } else {
            /* No more replacements found, append remainder */
            SB_AppendStr (S, Desc + Start);
            break;
        }
    }

    /* Zero-terminate the buffer and return it */
    SB_Terminate (S);
    return S;
}
