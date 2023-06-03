/*****************************************************************************/
/*                                                                           */
/*                                 scanner.c                                 */
/*                                                                           */
/*                  The scanner for the ca65 macroassembler                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* common */
#include "addrsize.h"
#include "attrib.h"
#include "chartype.h"
#include "check.h"
#include "filestat.h"
#include "fname.h"
#include "xmalloc.h"

/* ca65 */
#include "condasm.h"
#include "error.h"
#include "filetab.h"
#include "global.h"
#include "incpath.h"
#include "instr.h"
#include "istack.h"
#include "listing.h"
#include "macro.h"
#include "toklist.h"
#include "scanner.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Current input token incl. attributes */
Token CurTok = STATIC_TOKEN_INITIALIZER;

/* Struct to handle include files. */
typedef struct InputFile InputFile;
struct InputFile {
    FILE*           F;                  /* Input file descriptor */
    FilePos         Pos;                /* Position in file */
    token_t         Tok;                /* Last token */
    int             C;                  /* Last character */
    StrBuf          Line;               /* The current input line */
    int             IncSearchPath;      /* True if we've added a search path */
    int             BinSearchPath;      /* True if we've added a search path */
    InputFile*      Next;               /* Linked list of input files */
};

/* Struct to handle textual input data */
typedef struct InputData InputData;
struct InputData {
    char*           Text;               /* Pointer to the text data */
    const char*     Pos;                /* Pointer to current position */
    int             Malloced;           /* Memory was malloced */
    token_t         Tok;                /* Last token */
    int             C;                  /* Last character */
    InputData*      Next;               /* Linked list of input data */
};

/* Input source: Either file or data */
typedef struct CharSource CharSource;

/* Set of input functions */
typedef struct CharSourceFunctions CharSourceFunctions;
struct CharSourceFunctions {
    void (*MarkStart) (CharSource*);    /* Mark the start pos of a token */
    void (*NextChar) (CharSource*);     /* Read next char from input */
    void (*Done) (CharSource*);         /* Close input source */
};

/* Input source: Either file or data */
struct CharSource {
    CharSource*                 Next;   /* Linked list of char sources */
    token_t                     Tok;    /* Last token */
    int                         C;      /* Last character */
    int                         SkipN;  /* For '\r\n' line endings, skip '\n\ if next */
    const CharSourceFunctions*  Func;   /* Pointer to function table */
    union {
        InputFile               File;   /* File data */
        InputData               Data;   /* Textual data */
    }                           V;
};

/* Current input variables */
static CharSource* Source       = 0;    /* Current char source */
static unsigned    FCount       = 0;    /* Count of input files */
static int         C            = 0;    /* Current input character */

/* Force end of assembly */
int               ForcedEnd     = 0;

/* List of dot keywords with the corresponding tokens */
struct DotKeyword {
    const char* Key;                    /* MUST be first field */
    token_t     Tok;
} DotKeywords [] = {
    { ".A16",           TOK_A16                 },
    { ".A8",            TOK_A8                  },
    { ".ADDR",          TOK_ADDR                },
    { ".ADDRSIZE",      TOK_ADDRSIZE            },
    { ".ALIGN",         TOK_ALIGN               },
    { ".AND",           TOK_BOOLAND             },
    { ".ASCIIZ",        TOK_ASCIIZ              },
    { ".ASIZE",         TOK_ASIZE               },
    { ".ASSERT",        TOK_ASSERT              },
    { ".AUTOIMPORT",    TOK_AUTOIMPORT          },
    { ".BANK",          TOK_BANK                },
    { ".BANKBYTE",      TOK_BANKBYTE            },
    { ".BANKBYTES",     TOK_BANKBYTES           },
    { ".BITAND",        TOK_AND                 },
    { ".BITNOT",        TOK_NOT                 },
    { ".BITOR",         TOK_OR                  },
    { ".BITXOR",        TOK_XOR                 },
    { ".BLANK",         TOK_BLANK               },
    { ".BSS",           TOK_BSS                 },
    { ".BYT",           TOK_BYTE                },
    { ".BYTE",          TOK_BYTE                },
    { ".CASE",          TOK_CASE                },
    { ".CHARMAP",       TOK_CHARMAP             },
    { ".CODE",          TOK_CODE                },
    { ".CONCAT",        TOK_CONCAT              },
    { ".CONDES",        TOK_CONDES              },
    { ".CONST",         TOK_CONST               },
    { ".CONSTRUCTOR",   TOK_CONSTRUCTOR         },
    { ".CPU",           TOK_CPU                 },
    { ".DATA",          TOK_DATA                },
    { ".DBG",           TOK_DBG                 },
    { ".DBYT",          TOK_DBYT                },
    { ".DEBUGINFO",     TOK_DEBUGINFO           },
    { ".DEF",           TOK_DEFINED             },
    { ".DEFINE",        TOK_DEFINE              },
    { ".DEFINED",       TOK_DEFINED             },
    { ".DEFINEDMACRO",  TOK_DEFINEDMACRO        },
    { ".DELMAC",        TOK_DELMAC              },
    { ".DELMACRO",      TOK_DELMAC              },
    { ".DESTRUCTOR",    TOK_DESTRUCTOR          },
    { ".DWORD",         TOK_DWORD               },
    { ".ELSE",          TOK_ELSE                },
    { ".ELSEIF",        TOK_ELSEIF              },
    { ".END",           TOK_END                 },
    { ".ENDENUM",       TOK_ENDENUM             },
    { ".ENDIF",         TOK_ENDIF               },
    { ".ENDMAC",        TOK_ENDMACRO            },
    { ".ENDMACRO",      TOK_ENDMACRO            },
    { ".ENDPROC",       TOK_ENDPROC             },
    { ".ENDREP",        TOK_ENDREP              },
    { ".ENDREPEAT",     TOK_ENDREP              },
    { ".ENDSCOPE",      TOK_ENDSCOPE            },
    { ".ENDSTRUCT",     TOK_ENDSTRUCT           },
    { ".ENDUNION",      TOK_ENDUNION            },
    { ".ENUM",          TOK_ENUM                },
    { ".ERROR",         TOK_ERROR               },
    { ".EXITMAC",       TOK_EXITMACRO           },
    { ".EXITMACRO",     TOK_EXITMACRO           },
    { ".EXPORT",        TOK_EXPORT              },
    { ".EXPORTZP",      TOK_EXPORTZP            },
    { ".FARADDR",       TOK_FARADDR             },
    { ".FATAL",         TOK_FATAL               },
    { ".FEATURE",       TOK_FEATURE             },
    { ".FILEOPT",       TOK_FILEOPT             },
    { ".FOPT",          TOK_FILEOPT             },
    { ".FORCEIMPORT",   TOK_FORCEIMPORT         },
    { ".FORCEWORD",     TOK_FORCEWORD           },
    { ".GLOBAL",        TOK_GLOBAL              },
    { ".GLOBALZP",      TOK_GLOBALZP            },
    { ".HIBYTE",        TOK_HIBYTE              },
    { ".HIBYTES",       TOK_HIBYTES             },
    { ".HIWORD",        TOK_HIWORD              },
    { ".I16",           TOK_I16                 },
    { ".I8",            TOK_I8                  },
    { ".IDENT",         TOK_MAKEIDENT           },
    { ".IF",            TOK_IF                  },
    { ".IFBLANK",       TOK_IFBLANK             },
    { ".IFCONST",       TOK_IFCONST             },
    { ".IFDEF",         TOK_IFDEF               },
    { ".IFNBLANK",      TOK_IFNBLANK            },
    { ".IFNCONST",      TOK_IFNCONST            },
    { ".IFNDEF",        TOK_IFNDEF              },
    { ".IFNREF",        TOK_IFNREF              },
    { ".IFP02",         TOK_IFP02               },
    { ".IFP4510",       TOK_IFP4510             },
    { ".IFP816",        TOK_IFP816              },
    { ".IFPC02",        TOK_IFPC02              },
    { ".IFPDTV",        TOK_IFPDTV              },
    { ".IFPSC02",       TOK_IFPSC02             },
    { ".IFREF",         TOK_IFREF               },
    { ".IMPORT",        TOK_IMPORT              },
    { ".IMPORTZP",      TOK_IMPORTZP            },
    { ".INCBIN",        TOK_INCBIN              },
    { ".INCLUDE",       TOK_INCLUDE             },
    { ".INTERRUPTOR",   TOK_INTERRUPTOR         },
    { ".ISIZE",         TOK_ISIZE               },
    { ".ISMNEM",        TOK_ISMNEMONIC          },
    { ".ISMNEMONIC",    TOK_ISMNEMONIC          },
    { ".LEFT",          TOK_LEFT                },
    { ".LINECONT",      TOK_LINECONT            },
    { ".LIST",          TOK_LIST                },
    { ".LISTBYTES",     TOK_LISTBYTES           },
    { ".LITERAL",       TOK_LITERAL             },
    { ".LOBYTE",        TOK_LOBYTE              },
    { ".LOBYTES",       TOK_LOBYTES             },
    { ".LOCAL",         TOK_LOCAL               },
    { ".LOCALCHAR",     TOK_LOCALCHAR           },
    { ".LOWORD",        TOK_LOWORD              },
    { ".MAC",           TOK_MACRO               },
    { ".MACPACK",       TOK_MACPACK             },
    { ".MACRO",         TOK_MACRO               },
    { ".MATCH",         TOK_MATCH               },
    { ".MAX",           TOK_MAX                 },
    { ".MID",           TOK_MID                 },
    { ".MIN",           TOK_MIN                 },
    { ".MOD",           TOK_MOD                 },
    { ".NOT",           TOK_BOOLNOT             },
    { ".NULL",          TOK_NULL                },
    { ".OR",            TOK_BOOLOR              },
    { ".ORG",           TOK_ORG                 },
    { ".OUT",           TOK_OUT                 },
    { ".P02",           TOK_P02                 },
    { ".P4510",         TOK_P4510               },
    { ".P816",          TOK_P816                },
    { ".PAGELEN",       TOK_PAGELENGTH          },
    { ".PAGELENGTH",    TOK_PAGELENGTH          },
    { ".PARAMCOUNT",    TOK_PARAMCOUNT          },
    { ".PC02",          TOK_PC02                },
    { ".PDTV",          TOK_PDTV                },
    { ".POPCHARMAP",    TOK_POPCHARMAP          },
    { ".POPCPU",        TOK_POPCPU              },
    { ".POPSEG",        TOK_POPSEG              },
    { ".PROC",          TOK_PROC                },
    { ".PSC02",         TOK_PSC02               },
    { ".PUSHCHARMAP",   TOK_PUSHCHARMAP         },
    { ".PUSHCPU",       TOK_PUSHCPU             },
    { ".PUSHSEG",       TOK_PUSHSEG             },
    { ".REF",           TOK_REFERENCED          },
    { ".REFERENCED",    TOK_REFERENCED          },
    { ".REFERTO",       TOK_REFERTO             },
    { ".REFTO",         TOK_REFERTO             },
    { ".RELOC",         TOK_RELOC               },
    { ".REPEAT",        TOK_REPEAT              },
    { ".RES",           TOK_RES                 },
    { ".RIGHT",         TOK_RIGHT               },
    { ".RODATA",        TOK_RODATA              },
    { ".SCOPE",         TOK_SCOPE               },
    { ".SEGMENT",       TOK_SEGMENT             },
    { ".SET",           TOK_SET                 },
    { ".SETCPU",        TOK_SETCPU              },
    { ".SHL",           TOK_SHL                 },
    { ".SHR",           TOK_SHR                 },
    { ".SIZEOF",        TOK_SIZEOF              },
    { ".SMART",         TOK_SMART               },
    { ".SPRINTF",       TOK_SPRINTF             },
    { ".STRAT",         TOK_STRAT               },
    { ".STRING",        TOK_STRING              },
    { ".STRLEN",        TOK_STRLEN              },
    { ".STRUCT",        TOK_STRUCT              },
    { ".TAG",           TOK_TAG                 },
    { ".TCOUNT",        TOK_TCOUNT              },
    { ".TIME",          TOK_TIME                },
    { ".UNDEF",         TOK_UNDEF               },
    { ".UNDEFINE",      TOK_UNDEF               },
    { ".UNION",         TOK_UNION               },
    { ".VERSION",       TOK_VERSION             },
    { ".WARNING",       TOK_WARNING             },
    { ".WORD",          TOK_WORD                },
    { ".XMATCH",        TOK_XMATCH              },
    { ".XOR",           TOK_BOOLXOR             },
    { ".ZEROPAGE",      TOK_ZEROPAGE            },
};



/*****************************************************************************/
/*                            CharSource functions                           */
/*****************************************************************************/



static void UseCharSource (CharSource* S)
/* Initialize a new input source and start to use it. */
{
    /* Remember the current input char and token */
    S->Tok      = CurTok.Tok;
    S->C        = C;

    /* Use the new input source */
    S->Next     = Source;
    Source      = S;

    /* Read the first character from the new file */
    S->SkipN    = 0;
    S->Func->NextChar (S);

    /* Setup the next token so it will be skipped on the next call to
    ** NextRawTok().
    */
    CurTok.Tok = TOK_SEP;
}



static void DoneCharSource (void)
/* Close the top level character source */
{
    CharSource* S;

    /* First, call the type specific function */
    Source->Func->Done (Source);

    /* Restore the old token */
    CurTok.Tok = Source->Tok;
    C   = Source->C;

    /* Remember the last stacked input source */
    S = Source->Next;

    /* Delete the top level one ... */
    xfree (Source);

    /* ... and use the one before */
    Source = S;
}



/*****************************************************************************/
/*                            InputFile functions                            */
/*****************************************************************************/



static void IFMarkStart (CharSource* S)
/* Mark the start of the next token */
{
    CurTok.Pos = S->V.File.Pos;
}



static void IFNextChar (CharSource* S)
/* Read the next character from the input file */
{
    /* Check for end of line, read the next line if needed */
    while (SB_GetIndex (&S->V.File.Line) >= SB_GetLen (&S->V.File.Line)) {

        unsigned Len;

        /* End of current line reached, read next line */
        SB_Clear (&S->V.File.Line);
        while (1) {

            int N = fgetc (S->V.File.F);
            if (N == '\n' && S->SkipN) {
                N = fgetc (S->V.File.F);
            }
            S->SkipN = 0;

            if (N == EOF) {
                /* End of file. Accept files without a newline at the end */
                if (SB_NotEmpty (&S->V.File.Line)) {
                    break;
                }

                /* No more data - add an empty line to the listing. This
                ** is a small hack needed to keep the PC output in sync.
                */
                NewListingLine (&EmptyStrBuf, S->V.File.Pos.Name, FCount);
                C = EOF;
                return;

            /* Check for end of line */
            } else if (N == '\n') {
                /* End of line */
                break;
            } else if (N == '\r') {
                /* End of line, skip '\n' if it's the next character */
                S->SkipN = 1;
                break;

            /* Collect other stuff */
            } else {

                /* Append data to line */
                SB_AppendChar (&S->V.File.Line, N);

            }
        }


        /* If we come here, we have a new input line. To avoid problems
        ** with strange line terminators, remove all whitespace from the
        ** end of the line, then add a single newline.
        */
        Len = SB_GetLen (&S->V.File.Line);
        while (Len > 0 && IsSpace (SB_AtUnchecked (&S->V.File.Line, Len-1))) {
            --Len;
        }
        SB_Drop (&S->V.File.Line, SB_GetLen (&S->V.File.Line) - Len);
        SB_AppendChar (&S->V.File.Line, '\n');

        /* Terminate the string buffer */
        SB_Terminate (&S->V.File.Line);

        /* One more line */
        S->V.File.Pos.Line++;

        /* Remember the new line for the listing */
        NewListingLine (&S->V.File.Line, S->V.File.Pos.Name, FCount);

    }

    /* Set the column pointer */
    S->V.File.Pos.Col = SB_GetIndex (&S->V.File.Line);

    /* Return the next character from the buffer */
    C = SB_Get (&S->V.File.Line);
}



void IFDone (CharSource* S)
/* Close the current input file */
{
    /* We're at the end of an include file. Check if we have any
    ** open .IFs, or any open token lists in this file. This
    ** enforcement is artificial, using conditionals that start
    ** in one file and end in another are uncommon, and don't
    ** allowing these things will help finding errors.
    */
    CheckOpenIfs ();

    /* If we've added search paths for this file, remove them */
    if (S->V.File.IncSearchPath) {
        PopSearchPath (IncSearchPath);
    }
    if (S->V.File.BinSearchPath) {
        PopSearchPath (BinSearchPath);
    }

    /* Free the line buffer */
    SB_Done (&S->V.File.Line);

    /* Close the input file and decrement the file count. We will ignore
    ** errors here, since we were just reading from the file.
    */
    (void) fclose (S->V.File.F);
    --FCount;
}



/* Set of input file handling functions */
static const CharSourceFunctions IFFunc = {
    IFMarkStart,
    IFNextChar,
    IFDone
};



int NewInputFile (const char* Name)
/* Open a new input file. Returns true if the file could be successfully opened
** and false otherwise.
*/
{
    int         RetCode = 0;            /* Return code. Assume an error. */
    char*       PathName = 0;
    FILE*       F;
    struct stat Buf;
    StrBuf      NameBuf;                /* No need to initialize */
    StrBuf      Path = AUTO_STRBUF_INITIALIZER;
    unsigned    FileIdx;
    CharSource* S;


    /* If this is the main file, just try to open it. If it's an include file,
    ** search for it using the include path list.
    */
    if (FCount == 0) {
        /* Main file */
        F = fopen (Name, "r");
        if (F == 0) {
            Fatal ("Cannot open input file '%s': %s", Name, strerror (errno));
        }
    } else {
        /* We are on include level. Search for the file in the include
        ** directories.
        */
        PathName = SearchFile (IncSearchPath, Name);
        if (PathName == 0 || (F = fopen (PathName, "r")) == 0) {
            /* Not found or cannot open, print an error and bail out */
            Error ("Cannot open include file '%s': %s", Name, strerror (errno));
            goto ExitPoint;
        }

        /* Use the path name from now on */
        Name = PathName;
    }

    /* Stat the file and remember the values. There's a race condition here,
    ** since we cannot use fileno() (non-standard identifier in standard
    ** header file), and therefore not fstat. When using stat with the
    ** file name, there's a risk that the file was deleted and recreated
    ** while it was open. Since mtime and size are only used to check
    ** if a file has changed in the debugger, we will ignore this problem
    ** here.
    */
    if (FileStat (Name, &Buf) != 0) {
        Fatal ("Cannot stat input file '%s': %s", Name, strerror (errno));
    }

    /* Add the file to the input file table and remember the index */
    FileIdx = AddFile (SB_InitFromString (&NameBuf, Name),
                       (FCount == 0)? FT_MAIN : FT_INCLUDE,
                       Buf.st_size, (unsigned long) Buf.st_mtime);

    /* Create a new input source variable and initialize it */
    S                   = xmalloc (sizeof (*S));
    S->Func             = &IFFunc;
    S->V.File.F         = F;
    S->V.File.Pos.Line  = 0;
    S->V.File.Pos.Col   = 0;
    S->V.File.Pos.Name  = FileIdx;
    SB_Init (&S->V.File.Line);

    /* Push the path for this file onto the include search lists */
    SB_CopyBuf (&Path, Name, FindName (Name) - Name);
    SB_Terminate (&Path);
    S->V.File.IncSearchPath = PushSearchPath (IncSearchPath, SB_GetConstBuf (&Path));
    S->V.File.BinSearchPath = PushSearchPath (BinSearchPath, SB_GetConstBuf (&Path));
    SB_Done (&Path);

    /* Count active input files */
    ++FCount;

    /* Use this input source */
    UseCharSource (S);

    /* File successfully opened */
    RetCode = 1;

ExitPoint:
    /* Free an allocated name buffer */
    xfree (PathName);

    /* Return the success code */
    return RetCode;
}



/*****************************************************************************/
/*                            InputData functions                            */
/*****************************************************************************/



static void IDMarkStart (CharSource* S attribute ((unused)))
/* Mark the start of the next token */
{
    /* Nothing to do here */
}



static void IDNextChar (CharSource* S)
/* Read the next character from the input text */
{
    C = *S->V.Data.Pos++;
    if (C == '\0') {
        /* End of input data */
        --S->V.Data.Pos;
        C = EOF;
    }
}



void IDDone (CharSource* S)
/* Close the current input data */
{
    /* Cleanup the current stuff */
    if (S->V.Data.Malloced) {
        xfree (S->V.Data.Text);
    }
}



/* Set of input data handling functions */
static const CharSourceFunctions IDFunc = {
    IDMarkStart,
    IDNextChar,
    IDDone
};



void NewInputData (char* Text, int Malloced)
/* Add a chunk of input data to the input stream */
{
    CharSource* S;

    /* Create a new input source variable and initialize it */
    S                   = xmalloc (sizeof (*S));
    S->Func             = &IDFunc;
    S->V.Data.Text      = Text;
    S->V.Data.Pos       = Text;
    S->V.Data.Malloced  = Malloced;

    /* Use this input source */
    UseCharSource (S);
}



/*****************************************************************************/
/*                    Character classification functions                     */
/*****************************************************************************/



int IsIdChar (int C)
/* Return true if the character is a valid character for an identifier */
{
    return IsAlNum (C)                  ||
           (C == '_')                   ||
           (C == '@' && AtInIdents)     ||
           (C == '$' && DollarInIdents);
}



int IsIdStart (int C)
/* Return true if the character may start an identifier */
{
    return IsAlpha (C) || C == '_';
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static unsigned DigitVal (unsigned char C)
/* Convert a digit into it's numerical representation */
{
    if (IsDigit (C)) {
        return C - '0';
    } else {
        return toupper (C) - 'A' + 10;
    }
}



static void NextChar (void)
/* Read the next character from the input file */
{
    Source->Func->NextChar (Source);
}



void LocaseSVal (void)
/* Make SVal lower case */
{
    SB_ToLower (&CurTok.SVal);
}



void UpcaseSVal (void)
/* Make SVal upper case */
{
    SB_ToUpper (&CurTok.SVal);
}



static int CmpDotKeyword (const void* K1, const void* K2)
/* Compare function for the dot keyword search */
{
    return strcmp (((struct DotKeyword*)K1)->Key, ((struct DotKeyword*)K2)->Key);
}



static token_t FindDotKeyword (void)
/* Find the dot keyword in SVal. Return the corresponding token if found,
** return TOK_NONE if not found.
*/
{
    struct DotKeyword K;
    struct DotKeyword* R;

    /* Initialize K */
    K.Key = SB_GetConstBuf (&CurTok.SVal);
    K.Tok = 0;

    /* If we aren't in ignore case mode, we have to uppercase the keyword */
    if (!IgnoreCase) {
        UpcaseSVal ();
    }

    /* Search for the keyword */
    R = bsearch (&K, DotKeywords, sizeof (DotKeywords) / sizeof (DotKeywords [0]),
                 sizeof (DotKeywords [0]), CmpDotKeyword);
    if (R != 0) {
        return R->Tok;
    } else {
        return TOK_NONE;
    }
}



static void ReadIdent (void)
/* Read an identifier from the current input position into Ident. Filling SVal
** starts at the current position with the next character in C. It is assumed
** that any characters already filled in are ok, and the character in C is
** checked.
*/
{
    /* Read the identifier */
    do {
        SB_AppendChar (&CurTok.SVal, C);
        NextChar ();
    } while (IsIdChar (C));
    SB_Terminate (&CurTok.SVal);

    /* If we should ignore case, convert the identifier to upper case */
    if (IgnoreCase) {
        UpcaseSVal ();
    }
}



static void ReadStringConst (int StringTerm)
/* Read a string constant into SVal. */
{
    /* Skip the leading string terminator */
    NextChar ();

    /* Read the string */
    while (1) {
        if (C == StringTerm) {
            break;
        }
        if (C == '\n' || C == EOF) {
            Error ("Newline in string constant");
            break;
        }

        if (C == '\\' && StringEscapes) {
            NextChar ();

            switch (C) {
                case EOF:
                    Error ("Unterminated escape sequence in string constant");
                    break;
                case '\\':
                case '\'':
                case '"':
                    break;
                case 't':
                    C = '\x09';
                    break;
                case 'r':
                    C = '\x0D';
                    break;
                case 'n':
                    C = '\x0A';
                    break;
                case 'x':
                    NextChar ();
                    if (IsXDigit (C)) {
                        char high_nibble = DigitVal (C) << 4;
                        NextChar ();
                        if (IsXDigit (C)) {
                            C = high_nibble | DigitVal (C);
                            break;
                        }
                    }
                    /* FALLTHROUGH */
                default:
                    Error ("Unsupported escape sequence in string constant");
                    break;
            }
        }

        /* Append the char to the string */
        SB_AppendChar (&CurTok.SVal, C);

        /* Skip the character */
        NextChar ();
    }

    /* Skip the trailing terminator */
    NextChar ();

    /* Terminate the string */
    SB_Terminate (&CurTok.SVal);
}



static int Sweet16Reg (const StrBuf* Id)
/* Check if the given identifier is a sweet16 register. Return -1 if this is
** not the case, return the register number otherwise.
*/
{
    unsigned RegNum;
    char Check;

    if (SB_GetLen (Id) < 2) {
        return -1;
    }
    if (toupper (SB_AtUnchecked (Id, 0)) != 'R') {
        return -1;
    }
    if (!IsDigit (SB_AtUnchecked (Id, 1))) {
        return -1;
    }

    if (sscanf (SB_GetConstBuf (Id)+1, "%u%c", &RegNum, &Check) != 1 || RegNum > 15) {
        /* Invalid register */
        return -1;
    }

    /* The register number is valid */
    return (int) RegNum;
}



void NextRawTok (void)
/* Read the next raw token from the input stream */
{
    Macro* M;

    /* If we've a forced end of assembly, don't read further */
    if (ForcedEnd) {
        CurTok.Tok = TOK_EOF;
        return;
    }

Restart:
    /* Check if we have tokens from another input source */
    if (InputFromStack ()) {
        if (CurTok.Tok == TOK_IDENT && (M = FindDefine (&CurTok.SVal)) != 0) {
            /* This is a define style macro - expand it */
            MacExpandStart (M);
            goto Restart;
        }
        return;
    }

Again:
    /* Skip whitespace, remember if we had some */
    if ((CurTok.WS = IsBlank (C)) != 0) {
        do {
            NextChar ();
        } while (IsBlank (C));
    }

    /* Mark the file position of the next token */
    Source->Func->MarkStart (Source);

    /* Clear the string attribute */
    SB_Clear (&CurTok.SVal);

    /* Generate line info for the current token */
    NewAsmLine ();

    /* Hex number or PC symbol? */
    if (C == '$') {
        NextChar ();

        /* Hex digit must follow or DollarIsPC must be enabled */
        if (!IsXDigit (C)) {
            if (DollarIsPC) {
                CurTok.Tok = TOK_PC;
                return;
            } else {
                Error ("Hexadecimal digit expected");
            }
        }

        /* Read the number */
        CurTok.IVal = 0;
        while (1) {
            if (UnderlineInNumbers && C == '_') {
                while (C == '_') {
                    NextChar ();
                }
                if (!IsXDigit (C)) {
                    Error ("Number may not end with underline");
                }
            }
            if (IsXDigit (C)) {
                if (CurTok.IVal & 0xF0000000) {
                    Error ("Overflow in hexadecimal number");
                    CurTok.IVal = 0;
                }
                CurTok.IVal = (CurTok.IVal << 4) + DigitVal (C);
                NextChar ();
            } else {
                break;
            }
        }

        /* This is an integer constant */
        CurTok.Tok = TOK_INTCON;
        return;
    }

    /* Binary number? */
    if (C == '%') {
        NextChar ();

        /* 0 or 1 must follow */
        if (!IsBDigit (C)) {
            Error ("Binary digit expected");
        }

        /* Read the number */
        CurTok.IVal = 0;
        while (1) {
            if (UnderlineInNumbers && C == '_') {
                while (C == '_') {
                    NextChar ();
                }
                if (!IsBDigit (C)) {
                    Error ("Number may not end with underline");
                }
            }
            if (IsBDigit (C)) {
                if (CurTok.IVal & 0x80000000) {
                    Error ("Overflow in binary number");
                    CurTok.IVal = 0;
                }
                CurTok.IVal = (CurTok.IVal << 1) + DigitVal (C);
                NextChar ();
            } else {
                break;
            }
        }

        /* This is an integer constant */
        CurTok.Tok = TOK_INTCON;
        return;
    }

    /* Number? */
    if (IsDigit (C)) {

        char Buf[16];
        unsigned Digits;
        unsigned Base;
        unsigned I;
        long     Max;
        unsigned DVal;

        /* Ignore leading zeros */
        while (C == '0') {
            NextChar ();
        }

        /* Read the number into Buf counting the digits */
        Digits = 0;
        while (1) {
            if (UnderlineInNumbers && C == '_') {
                while (C == '_') {
                    NextChar ();
                }
                if (!IsXDigit (C)) {
                    Error ("Number may not end with underline");
                }
            }
            if (IsXDigit (C)) {
                /* Buf is big enough to allow any decimal and hex number to
                ** overflow, so ignore excess digits here, they will be detected
                ** when we convert the value.
                */
                if (Digits < sizeof (Buf)) {
                    Buf[Digits++] = C;
                }
                NextChar ();
            } else {
                break;
            }
        }

        /* Allow zilog/intel style hex numbers with a 'h' suffix */
        if (C == 'h' || C == 'H') {
            NextChar ();
            Base = 16;
            Max  = 0xFFFFFFFFUL / 16;
        } else {
            Base = 10;
            Max  = 0xFFFFFFFFUL / 10;
        }

        /* Convert the number using the given base */
        CurTok.IVal = 0;
        for (I = 0; I < Digits; ++I) {
            if (CurTok.IVal > Max) {
                Error ("Number out of range");
                CurTok.IVal = 0;
                break;
            }
            DVal = DigitVal (Buf[I]);
            if (DVal >= Base) {
                Error ("Invalid digits in number");
                CurTok.IVal = 0;
                break;
            }
            CurTok.IVal = (CurTok.IVal * Base) + DVal;
        }

        /* This is an integer constant */
        CurTok.Tok = TOK_INTCON;
        return;
    }

    /* Control command? */
    if (C == '.') {

        /* Remember and skip the dot */
        NextChar ();

        /* Check if it's just a dot */
        if (!IsIdStart (C)) {

            /* Just a dot */
            CurTok.Tok = TOK_DOT;

        } else {

            /* Read the remainder of the identifier */
            SB_AppendChar (&CurTok.SVal, '.');
            ReadIdent ();

            /* Dot keyword, search for it */
            CurTok.Tok = FindDotKeyword ();
            if (CurTok.Tok == TOK_NONE) {

                /* Not found */
                if (!LeadingDotInIdents) {
                    /* Invalid pseudo instruction */
                    Error ("'%m%p' is not a recognized control command", &CurTok.SVal);
                    goto Again;
                }

                /* An identifier with a dot. Check if it's a define style
                ** macro.
                */
                if ((M = FindDefine (&CurTok.SVal)) != 0) {
                    /* This is a define style macro - expand it */
                    MacExpandStart (M);
                    goto Restart;
                }

                /* Just an identifier with a dot */
                CurTok.Tok = TOK_IDENT;
            }

        }
        return;
    }

    /* Indirect op for sweet16 cpu. Must check this before checking for local
    ** symbols, because these may also use the '@' symbol.
    */
    if (CPU == CPU_SWEET16 && C == '@') {
        NextChar ();
        CurTok.Tok = TOK_AT;
        return;
    }

    /* Local symbol? */
    if (C == LocalStart) {

        NextChar ();

        if (IsIdChar (C)) {
            /* Read a local identifier */
            CurTok.Tok = TOK_LOCAL_IDENT;
            SB_AppendChar (&CurTok.SVal, LocalStart);
            ReadIdent ();
        } else {
            /* Read an unnamed label */
            CurTok.IVal = 0;
            CurTok.Tok = TOK_ULABEL;

            if (C == '-' || C == '<') {
                int PrevC = C;
                do {
                    --CurTok.IVal;
                    NextChar ();
                } while (C == PrevC);
            } else if (C == '+' || C == '>') {
                int PrevC = C;
                do {
                    ++CurTok.IVal;
                    NextChar ();
                } while (C == PrevC);
            }
        }

        return;
    }


    /* Identifier or keyword? */
    if (IsIdStart (C)) {

        /* Read the identifier */
        ReadIdent ();

        /* Check for special names. Bail out if we have identified the type of
        ** the token. Go on if the token is an identifier.
        */
        switch (SB_GetLen (&CurTok.SVal)) {
            case 1:
                switch (toupper (SB_AtUnchecked (&CurTok.SVal, 0))) {

                    case 'A':
                        if (C == ':') {
                            NextChar ();
                            CurTok.Tok = TOK_OVERRIDE_ABS;
                        } else {
                            CurTok.Tok = TOK_A;
                        }
                        return;

                    case 'F':
                        if (C == ':') {
                            NextChar ();
                            CurTok.Tok = TOK_OVERRIDE_FAR;
                            return;
                        }
                        break;

                    case 'S':
                        if ((CPU == CPU_4510) || (CPU == CPU_65816)) {
                            CurTok.Tok = TOK_S;
                            return;
                        }
                        break;

                    case 'X':
                        CurTok.Tok = TOK_X;
                        return;

                    case 'Y':
                        CurTok.Tok = TOK_Y;
                        return;

                    case 'Z':
                        if (C == ':') {
                            NextChar ();
                            CurTok.Tok = TOK_OVERRIDE_ZP;
                           return;
                        } else {
                            if (CPU == CPU_4510) {
                                CurTok.Tok = TOK_Z;
                                return;
                            }
                        }
                        break;

                    default:
                        break;
                }
                break;
            case 2:
                if ((CPU == CPU_4510) &&
                    (toupper (SB_AtUnchecked (&CurTok.SVal, 0)) == 'S') &&
                    (toupper (SB_AtUnchecked (&CurTok.SVal, 1)) == 'P')) {

                    CurTok.Tok = TOK_S;
                    return;
                }
                /* FALL THROUGH */
            default:
                if (CPU == CPU_SWEET16 &&
                   (CurTok.IVal = Sweet16Reg (&CurTok.SVal)) >= 0) {

                    /* A sweet16 register number in sweet16 mode */
                    CurTok.Tok = TOK_REG;
                    return;
                }
        }

        /* Check for define style macro */
        if ((M = FindDefine (&CurTok.SVal)) != 0) {
            /* Macro - expand it */
            MacExpandStart (M);
            goto Restart;
        } else {
            /* An identifier */
            CurTok.Tok = TOK_IDENT;
        }
        return;
    }

    /* Ok, let's do the switch */
CharAgain:
    switch (C) {

        case '+':
            NextChar ();
            CurTok.Tok = TOK_PLUS;
            return;

        case '-':
            NextChar ();
            CurTok.Tok = TOK_MINUS;
            return;

        case '/':
            NextChar ();
            if (C != '*') {
                CurTok.Tok = TOK_DIV;
            } else if (CComments) {
                /* Remember the position, then skip the '*' */
                Collection LineInfos = STATIC_COLLECTION_INITIALIZER;
                GetFullLineInfo (&LineInfos);
                NextChar ();
                do {
                    while (C !=  '*') {
                        if (C == EOF) {
                            LIError (&LineInfos, "Unterminated comment");
                            ReleaseFullLineInfo (&LineInfos);
                            DoneCollection (&LineInfos);
                            goto CharAgain;
                        }
                        NextChar ();
                    }
                    NextChar ();
                } while (C != '/');
                NextChar ();
                ReleaseFullLineInfo (&LineInfos);
                DoneCollection (&LineInfos);
                goto Again;
            }
            return;

        case '*':
            NextChar ();
            CurTok.Tok = TOK_MUL;
            return;

        case '^':
            NextChar ();
            CurTok.Tok = TOK_XOR;
            return;

        case '&':
            NextChar ();
            if (C == '&') {
                NextChar ();
                CurTok.Tok = TOK_BOOLAND;
            } else {
                CurTok.Tok = TOK_AND;
            }
            return;

        case '|':
            NextChar ();
            if (C == '|') {
                NextChar ();
                CurTok.Tok = TOK_BOOLOR;
            } else {
                CurTok.Tok = TOK_OR;
            }
            return;

        case ':':
            NextChar ();
            switch (C) {

                case ':':
                    NextChar ();
                    CurTok.Tok = TOK_NAMESPACE;
                    break;

                case '-':
                case '<':
                {
                    int PrevC = C;
                    CurTok.IVal = 0;
                    do {
                        --CurTok.IVal;
                        NextChar ();
                    } while (C == PrevC);
                    CurTok.Tok = TOK_ULABEL;
                    break;
                }

                case '+':
                case '>':
                {
                    int PrevC = C;
                    CurTok.IVal = 0;
                    do {
                        ++CurTok.IVal;
                        NextChar ();
                    } while (C == PrevC);
                    CurTok.Tok = TOK_ULABEL;
                    break;
                }

                case '=':
                    NextChar ();
                    CurTok.Tok = TOK_ASSIGN;
                    break;

                default:
                    CurTok.Tok = TOK_COLON;
                    break;
            }
            return;

        case ',':
            NextChar ();
            CurTok.Tok = TOK_COMMA;
            return;

        case ';':
            NextChar ();
            while (C != '\n' && C != EOF) {
                NextChar ();
            }
            goto CharAgain;

        case '#':
            NextChar ();
            CurTok.Tok = TOK_HASH;
            return;

        case '(':
            NextChar ();
            CurTok.Tok = TOK_LPAREN;
            return;

        case ')':
            NextChar ();
            CurTok.Tok = TOK_RPAREN;
            return;

        case '[':
            NextChar ();
            CurTok.Tok = TOK_LBRACK;
            return;

        case ']':
            NextChar ();
            CurTok.Tok = TOK_RBRACK;
            return;

        case '{':
            NextChar ();
            CurTok.Tok = TOK_LCURLY;
            return;

        case '}':
            NextChar ();
            CurTok.Tok = TOK_RCURLY;
            return;

        case '<':
            NextChar ();
            if (C == '=') {
                NextChar ();
                CurTok.Tok = TOK_LE;
            } else if (C == '<') {
                NextChar ();
                CurTok.Tok = TOK_SHL;
            } else if (C == '>') {
                NextChar ();
                CurTok.Tok = TOK_NE;
            } else {
                CurTok.Tok = TOK_LT;
            }
            return;

        case '=':
            NextChar ();
            CurTok.Tok = TOK_EQ;
            return;

        case '!':
            NextChar ();
            CurTok.Tok = TOK_BOOLNOT;
            return;

        case '>':
            NextChar ();
            if (C == '=') {
                NextChar ();
                CurTok.Tok = TOK_GE;
            } else if (C == '>') {
                NextChar ();
                CurTok.Tok = TOK_SHR;
            } else {
                CurTok.Tok = TOK_GT;
            }
            return;

        case '~':
            NextChar ();
            CurTok.Tok = TOK_NOT;
            return;

        case '\'':
            /* Hack: If we allow ' as terminating character for strings, read
            ** the following stuff as a string, and check for a one character
            ** string later.
            */
            if (LooseStringTerm) {
                ReadStringConst ('\'');
                if (SB_GetLen (&CurTok.SVal) == 1) {
                    CurTok.IVal = SB_AtUnchecked (&CurTok.SVal, 0);
                    CurTok.Tok = TOK_CHARCON;
                } else {
                    CurTok.Tok = TOK_STRCON;
                }
            } else {
                /* Always a character constant */
                NextChar ();
                if (C == EOF || IsControl (C)) {
                    Error ("Illegal character constant");
                    goto CharAgain;
                }
                CurTok.IVal = C;
                CurTok.Tok = TOK_CHARCON;
                NextChar ();
                if (C != '\'') {
                    if (!MissingCharTerm) {
                        Error ("Illegal character constant");
                    }
                } else {
                    NextChar ();
                }
            }
            return;

        case '\"':
            ReadStringConst ('\"');
            CurTok.Tok = TOK_STRCON;
            return;

        case '\\':
            /* Line continuation? */
            if (LineCont) {
                NextChar ();
                /* Next char should be a LF, if not, will result in an error later */
                if (C == '\n') {
                    /* Ignore the '\n' */
                    NextChar ();
                    goto Again;
                } else {
                    /* Make it clear what the problem is: */
                    Error ("EOL expected.");
                }
            }
            break;

        case '\n':
            NextChar ();
            CurTok.Tok = TOK_SEP;
            return;

        case EOF:
            CheckInputStack ();
            /* In case of the main file, do not close it, but return EOF. */
            if (Source && Source->Next) {
                DoneCharSource ();
                goto Again;
            } else {
                CurTok.Tok = TOK_EOF;
            }
            return;
    }

    /* If we go here, we could not identify the current character. Skip it
    ** and try again.
    */
    Error ("Invalid input character: 0x%02X", C & 0xFF);
    NextChar ();
    goto Again;
}



int GetSubKey (const char* const* Keys, unsigned Count)
/* Search for a subkey in a table of keywords. The current token must be an
** identifier and all keys must be in upper case. The identifier will be
** uppercased in the process. The function returns the index of the keyword,
** or -1 if the keyword was not found.
*/
{
    unsigned I;

    /* Must have an identifier */
    PRECONDITION (CurTok.Tok == TOK_IDENT);

    /* If we aren't in ignore case mode, we have to uppercase the identifier */
    if (!IgnoreCase) {
        UpcaseSVal ();
    }

    /* Do a linear search (a binary search is not worth the effort) */
    for (I = 0; I < Count; ++I) {
        if (SB_CompareStr (&CurTok.SVal, Keys [I]) == 0) {
            /* Found it */
            return I;
        }
    }

    /* Not found */
    return -1;
}



unsigned char ParseAddrSize (void)
/* Check if the next token is a keyword that denotes an address size specifier.
** If so, return the corresponding address size constant, otherwise output an
** error message and return ADDR_SIZE_DEFAULT.
*/
{
    unsigned char AddrSize;

    /* Check for an identifier */
    if (CurTok.Tok != TOK_IDENT) {
        Error ("Address size specifier expected");
        return ADDR_SIZE_DEFAULT;
    }

    /* Convert the attribute */
    AddrSize = AddrSizeFromStr (SB_GetConstBuf (&CurTok.SVal));
    if (AddrSize == ADDR_SIZE_INVALID) {
        Error ("Address size specifier expected");
        AddrSize = ADDR_SIZE_DEFAULT;
    }

    /* Done */
    return AddrSize;
}



void InitScanner (const char* InFile)
/* Initialize the scanner, open the given input file */
{
    /* Open the input file */
    NewInputFile (InFile);
}



void DoneScanner (void)
/* Release scanner resources */
{
    DoneCharSource ();
}
