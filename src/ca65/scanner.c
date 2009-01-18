/*****************************************************************************/
/*                                                                           */
/*				   scanner.c				     */
/*                                                                           */
/*		    The scanner for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>		/* EMX needs this */
#include <sys/stat.h>

/* common */
#include "addrsize.h"
#include "attrib.h"
#include "chartype.h"
#include "check.h"
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
/*     	       	    	       	     Data 				     */
/*****************************************************************************/



Token Tok = TOK_NONE;                   /* Current token */
int WS;	  				/* Flag: Whitespace before token */
long IVal;	       	       	    	/* Integer token attribute */
StrBuf SVal = STATIC_STRBUF_INITIALIZER;/* String token attribute */

FilePos	CurPos = { 0, 0, 0 };		/* Name and position in current file */



/* Struct to handle include files. */
typedef struct InputFile InputFile;
struct InputFile {
    FILE*      	    F;		       	/* Input file descriptor */
    FilePos	    Pos;	       	/* Position in file */
    Token           Tok;	       	/* Last token */
    int		    C;			/* Last character */
    char       	    Line[256];		/* The current input line */
    InputFile*	    Next;      	       	/* Linked list of input files */
};

/* Struct to handle textual input data */
typedef struct InputData InputData;
struct InputData {
    char*      	    Text;               /* Pointer to the text data */
    const char*     Pos;		/* Pointer to current position */
    int		    Malloced;		/* Memory was malloced */
    Token           Tok;	    	/* Last token */
    int		    C;			/* Last character */
    InputData*	    Next;		/* Linked list of input data */
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
    Token                       Tok;	/* Last token */
    int		                C;	/* Last character */
    const CharSourceFunctions*  Func;   /* Pointer to function table */
    union {
        InputFile               File;   /* File data */
        InputData               Data;   /* Textual data */
    }                           V;
};

/* Current input variables */
static CharSource* Source       = 0;    /* Current char source */
static unsigned	    FCount      = 0;  	/* Count of input files */
static int	    C           = 0;	/* Current input character */

/* Force end of assembly */
int 		  ForcedEnd     = 0;

/* List of dot keywords with the corresponding tokens */
struct DotKeyword {
    const char*	Key;			/* MUST be first field */
    Token       Tok;
} DotKeywords [] = {
    { ".A16",  	    	TOK_A16		},
    { ".A8",   	    	TOK_A8		},
    { ".ADDR", 	    	TOK_ADDR	},
    { ".ALIGN",	       	TOK_ALIGN     	},
    { ".AND", 	    	TOK_BOOLAND 	},
    { ".ASCIIZ",       	TOK_ASCIIZ	},
    { ".ASSERT",        TOK_ASSERT      },
    { ".AUTOIMPORT", 	TOK_AUTOIMPORT	},
    { ".BANKBYTE",      TOK_BANKBYTE    },
    { ".BITAND",	TOK_AND		},
    { ".BITNOT",	TOK_NOT		},
    { ".BITOR",	    	TOK_OR		},
    { ".BITXOR",	TOK_XOR		},
    { ".BLANK",	    	TOK_BLANK	},
    { ".BSS",  	    	TOK_BSS		},
    { ".BYT", 	    	TOK_BYTE	},
    { ".BYTE", 	    	TOK_BYTE	},
    { ".CASE",  	TOK_CASE	},
    { ".CHARMAP",    	TOK_CHARMAP 	},
    { ".CODE", 	    	TOK_CODE    	},
    { ".CONCAT",       	TOK_CONCAT	},
    { ".CONDES",	TOK_CONDES	},
    { ".CONST", 	TOK_CONST	},
    { ".CONSTRUCTOR",	TOK_CONSTRUCTOR	},
    { ".CPU", 	  	TOK_CPU		},
    { ".DATA",  	TOK_DATA	},
    { ".DBG",		TOK_DBG		},
    { ".DBYT",  	TOK_DBYT	},
    { ".DEBUGINFO",	TOK_DEBUGINFO	},
    { ".DEF",   	TOK_DEFINED	},
    { ".DEFINE",	TOK_DEFINE	},
    { ".DEFINED",	TOK_DEFINED	},
    { ".DESTRUCTOR",	TOK_DESTRUCTOR	},
    { ".DWORD", 	TOK_DWORD	},
    { ".ELSE",  	TOK_ELSE	},
    { ".ELSEIF",	TOK_ELSEIF	},
    { ".END",   	TOK_END		},
    { ".ENDENUM",       TOK_ENDENUM     },
    { ".ENDIF", 	TOK_ENDIF	},
    { ".ENDMAC",	TOK_ENDMACRO	},
    { ".ENDMACRO",	TOK_ENDMACRO	},
    { ".ENDPROC",	TOK_ENDPROC 	},
    { ".ENDREP",	TOK_ENDREP	},
    { ".ENDREPEAT",	TOK_ENDREP	},
    { ".ENDSCOPE",      TOK_ENDSCOPE    },
    { ".ENDSTRUCT",	TOK_ENDSTRUCT	},
    { ".ENDUNION",     	TOK_ENDUNION    },
    { ".ENUM",          TOK_ENUM        },
    { ".ERROR", 	TOK_ERROR   	},
    { ".EXITMAC",	TOK_EXITMACRO 	},
    { ".EXITMACRO",	TOK_EXITMACRO	},
    { ".EXPORT",	TOK_EXPORT	},
    { ".EXPORTZP",	TOK_EXPORTZP	},
    { ".FARADDR",	TOK_FARADDR	},
    { ".FEATURE",	TOK_FEATURE	},
    { ".FILEOPT",	TOK_FILEOPT	},
    { ".FOPT",  	TOK_FILEOPT	},
    { ".FORCEIMPORT",   TOK_FORCEIMPORT },
    { ".FORCEWORD",	TOK_FORCEWORD	},
    { ".GLOBAL",	TOK_GLOBAL	},
    { ".GLOBALZP",	TOK_GLOBALZP	},
    { ".HIBYTE",        TOK_HIBYTE      },
    { ".HIWORD",        TOK_HIWORD      },
    { ".I16",   	TOK_I16		},
    { ".I8",    	TOK_I8		},
    { ".IDENT",         TOK_MAKEIDENT   },
    { ".IF",    	TOK_IF		},
    { ".IFBLANK",	TOK_IFBLANK	},
    { ".IFCONST",	TOK_IFCONST	},
    { ".IFDEF", 	TOK_IFDEF	},
    { ".IFNBLANK",	TOK_IFNBLANK	},
    { ".IFNCONST",	TOK_IFNCONST	},
    { ".IFNDEF",	TOK_IFNDEF	},
    { ".IFNREF",	TOK_IFNREF	},
    { ".IFP02",		TOK_IFP02	},
    { ".IFP816",	TOK_IFP816	},
    { ".IFPC02",	TOK_IFPC02	},
    { ".IFPSC02",      	TOK_IFPSC02	},
    { ".IFREF",		TOK_IFREF	},
    { ".IMPORT",	TOK_IMPORT	},
    { ".IMPORTZP",	TOK_IMPORTZP	},
    { ".INCBIN",	TOK_INCBIN	},
    { ".INCLUDE",    	TOK_INCLUDE 	},
    { ".INTERRUPTOR",   TOK_INTERRUPTOR },
    { ".LEFT",		TOK_LEFT	},
    { ".LINECONT",	TOK_LINECONT	},
    { ".LIST",		TOK_LIST	},
    { ".LISTBYTES",	TOK_LISTBYTES	},
    { ".LOBYTE",        TOK_LOBYTE      },
    { ".LOCAL",		TOK_LOCAL	},
    { ".LOCALCHAR",	TOK_LOCALCHAR	},
    { ".LOWORD",        TOK_LOWORD      },
    { ".MAC",  		TOK_MACRO	},
    { ".MACPACK",	TOK_MACPACK	},
    { ".MACRO",	  	TOK_MACRO	},
    { ".MATCH",		TOK_MATCH	},
    { ".MID",   	TOK_MID		},
    { ".MOD", 		TOK_MOD		},
    { ".NOT", 		TOK_BOOLNOT 	},
    { ".NULL",		TOK_NULL      	},
    { ".OR",  		TOK_BOOLOR  	},
    { ".ORG",  		TOK_ORG		},
    { ".OUT",  		TOK_OUT		},
    { ".P02",  		TOK_P02		},
    { ".P816", 		TOK_P816	},
    { ".PAGELEN",	TOK_PAGELENGTH	},
    { ".PAGELENGTH",	TOK_PAGELENGTH	},
    { ".PARAMCOUNT", 	TOK_PARAMCOUNT  },
    { ".PC02", 		TOK_PC02	},
    { ".POPSEG", 	TOK_POPSEG	},
    { ".PROC", 		TOK_PROC	},
    { ".PSC02",	       	TOK_PSC02	},
    { ".PUSHSEG",      	TOK_PUSHSEG	},
    { ".REF", 		TOK_REFERENCED  },
    { ".REFERENCED",	TOK_REFERENCED  },
    { ".RELOC",		TOK_RELOC	},
    { ".REPEAT",	TOK_REPEAT	},
    { ".RES",  		TOK_RES		},
    { ".RIGHT",	  	TOK_RIGHT	},
    { ".RODATA",	TOK_RODATA	},
    { ".SCOPE",         TOK_SCOPE       },
    { ".SEGMENT",  	TOK_SEGMENT	},
    { ".SET",           TOK_SET         },
    { ".SETCPU",  	TOK_SETCPU    	},
    { ".SHL", 	  	TOK_SHL		},
    { ".SHR", 	  	TOK_SHR		},
    { ".SIZEOF",        TOK_SIZEOF      },
    { ".SMART",	  	TOK_SMART	},
    { ".SPRINTF",       TOK_SPRINTF     },
    { ".STRAT",		TOK_STRAT	},
    { ".STRING",	TOK_STRING	},
    { ".STRLEN",	TOK_STRLEN	},
    { ".STRUCT",        TOK_STRUCT      },
    { ".SUNPLUS",  	TOK_SUNPLUS	},
    { ".TAG",           TOK_TAG         },
    { ".TCOUNT",	TOK_TCOUNT	},
    { ".TIME",         	TOK_TIME	},
    { ".UNION",         TOK_UNION       },
    { ".VERSION",       TOK_VERSION     },
    { ".WARNING",	TOK_WARNING	},
    { ".WORD", 	  	TOK_WORD	},
    { ".XMATCH",	TOK_XMATCH	},
    { ".XOR",  	       	TOK_BOOLXOR  	},
    { ".ZEROPAGE", 	TOK_ZEROPAGE	},
};



/*****************************************************************************/
/*                            CharSource functions                           */
/*****************************************************************************/



static void UseCharSource (CharSource* S)
/* Initialize a new input source and start to use it. */
{
    /* Remember the current input char and token */
    S->Tok      = Tok;
    S->C        = C;

    /* Use the new input source */
    S->Next   	= Source;
    Source      = S;

    /* Read the first character from the new file */
    S->Func->NextChar (S);

    /* Setup the next token so it will be skipped on the next call to
     * NextRawTok().
     */
    Tok = TOK_SEP;
}



static void DoneCharSource (void)
/* Close the top level character source */
{
    CharSource* S;

    /* First, call the type specific function */
    Source->Func->Done (Source);

    /* Restore the old token */
    Tok = Source->Tok;
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
    CurPos = S->V.File.Pos;
}



static void IFNextChar (CharSource* S)
/* Read the next character from the input file */
{
    /* Check for end of line, read the next line if needed */
    while (S->V.File.Line [S->V.File.Pos.Col] == '\0') {

        unsigned Len, Removed;

        /* End of current line reached, read next line */
        if (fgets (S->V.File.Line, sizeof (S->V.File.Line), S->V.File.F) == 0) {
            /* End of file. Add an empty line to the listing. This is a
             * small hack needed to keep the PC output in sync.
             */
            NewListingLine ("", S->V.File.Pos.Name, FCount);
            C = EOF;
            return;
        }

        /* For better handling of files with unusual line endings (DOS
         * files that are accidently translated on Unix for example),
         * first remove all whitespace at the end, then add a single
         * newline.
         */
        Len = strlen (S->V.File.Line);
        Removed = 0;
        while (Len > 0 && IsSpace (S->V.File.Line[Len-1])) {
            ++Removed;
            --Len;
        }
        if (Removed) {
            S->V.File.Line[Len+0] = '\n';
            S->V.File.Line[Len+1] = '\0';
        }

        /* One more line */
        S->V.File.Pos.Line++;
        S->V.File.Pos.Col = 0;

        /* Remember the new line for the listing */
        NewListingLine (S->V.File.Line, S->V.File.Pos.Name, FCount);

    }

    /* Return the next character from the file */
    C = S->V.File.Line [S->V.File.Pos.Col++];
}



void IFDone (CharSource* S)
/* Close the current input file */
{
    /* We're at the end of an include file. Check if we have any
     * open .IFs, or any open token lists in this file. This
     * enforcement is artificial, using conditionals that start
     * in one file and end in another are uncommon, and don't
     * allowing these things will help finding errors.
     */
    CheckOpenIfs ();

    /* Close the input file and decrement the file count. We will ignore
     * errors here, since we were just reading from the file.
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
 * and false otherwise.
 */
{
    int RetCode = 0;            /* Return code. Assume an error. */
    char* PathName = 0;

    /* First try to open the file */
    FILE* F = fopen (Name, "r");
    if (F == 0) {

     	/* Error (fatal error if this is the main file) */
       	if (FCount == 0) {
     	    Fatal ("Cannot open input file `%s': %s", Name, strerror (errno));
       	}

       	/* We are on include level. Search for the file in the include
     	 * directories.
     	 */
     	PathName = FindInclude (Name);
       	if (PathName == 0 || (F = fopen (PathName, "r")) == 0) {
     	    /* Not found or cannot open, print an error and bail out */
     	    Error ("Cannot open include file `%s': %s", Name, strerror (errno));
            goto ExitPoint;
     	}

       	/* Use the path name from now on */
        Name = PathName;
    }

    /* check again if we do now have an open file */
    if (F != 0) {

        StrBuf          NameBuf;
     	unsigned        FileIdx;
        CharSource*     S;

     	/* Stat the file and remember the values. There a race condition here,
         * since we cannot use fileno() (non standard identifier in standard
         * header file), and therefore not fstat. When using stat with the
         * file name, there's a risk that the file was deleted and recreated
         * while it was open. Since mtime and size are only used to check
         * if a file has changed in the debugger, we will ignore this problem
         * here.
         */
     	struct stat Buf;
     	if (stat (Name, &Buf) != 0) {
     	    Fatal ("Cannot stat input file `%s': %s", Name, strerror (errno));
     	}

     	/* Add the file to the input file table and remember the index */
     	FileIdx = AddFile (SB_InitFromString (&NameBuf, Name), Buf.st_size, Buf.st_mtime);

       	/* Create a new input source variable and initialize it */
     	S                   = xmalloc (sizeof (*S));
        S->Func             = &IFFunc;
     	S->V.File.F         = F;
     	S->V.File.Pos.Line  = 0;
     	S->V.File.Pos.Col   = 0;
     	S->V.File.Pos.Name  = FileIdx;
       	S->V.File.Line[0]   = '\0';

        /* Count active input files */
       	++FCount;

        /* Use this input source */
        UseCharSource (S);
    }

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
/*	      	      Character classification functions		     */
/*****************************************************************************/



int IsIdChar (int C)
/* Return true if the character is a valid character for an identifier */
{
    return IsAlNum (C) 	     		||
	   (C == '_')	     		||
	   (C == '@' && AtInIdents)	||
	   (C == '$' && DollarInIdents);
}



int IsIdStart (int C)
/* Return true if the character may start an identifier */
{
    return IsAlpha (C) || C == '_';
}



/*****************************************************************************/
/*     	       	     	       	     Code				     */
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
    SB_ToLower (&SVal);
}



void UpcaseSVal (void)
/* Make SVal upper case */
{
    SB_ToUpper (&SVal);
}



static int CmpDotKeyword (const void* K1, const void* K2)
/* Compare function for the dot keyword search */
{
    return strcmp (((struct DotKeyword*)K1)->Key, ((struct DotKeyword*)K2)->Key);
}



static unsigned char FindDotKeyword (void)
/* Find the dot keyword in SVal. Return the corresponding token if found,
 * return TOK_NONE if not found.
 */
{
    struct DotKeyword K;
    struct DotKeyword* R;

    /* Initialize K */
    K.Key = SB_GetConstBuf (&SVal);
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
 * starts at the current position with the next character in C. It is assumed
 * that any characters already filled in are ok, and the character in C is
 * checked.
 */
{
    /* Read the identifier */
    do {
        SB_AppendChar (&SVal, C);
    	NextChar ();
    } while (IsIdChar (C));
    SB_Terminate (&SVal);

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

       	/* Append the char to the string */
        SB_AppendChar (&SVal, C);

     	/* Skip the character */
	NextChar ();
    }

    /* Skip the trailing terminator */
    NextChar ();

    /* Terminate the string */
    SB_Terminate (&SVal);
}



static int Sweet16Reg (const StrBuf* Id)
/* Check if the given identifier is a sweet16 register. Return -1 if this is
 * not the case, return the register number otherwise.
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
    /* If we've a forced end of assembly, don't read further */
    if (ForcedEnd) {
     	Tok = TOK_EOF;
     	return;
    }

Restart:
    /* Check if we have tokens from another input source */
    if (InputFromStack ()) {
     	return;
    }

Again:
    /* Skip whitespace, remember if we had some */
    if ((WS = IsBlank (C)) != 0) {
	do {
     	    NextChar ();
        } while (IsBlank (C));
    }

    /* Mark the file position of the next token */
    Source->Func->MarkStart (Source);

    /* Clear the string attribute */
    SB_Clear (&SVal);

    /* Hex number or PC symbol? */
    if (C == '$') {
     	NextChar ();

     	/* Hex digit must follow or DollarIsPC must be enabled */
     	if (!IsXDigit (C)) {
	    if (DollarIsPC) {
	     	Tok = TOK_PC;
		return;
	    } else {
     	     	Error ("Hexadecimal digit expected");
	    }
     	}

      	/* Read the number */
     	IVal = 0;
     	while (IsXDigit (C)) {
     	    if (IVal & 0xF0000000) {
     	    	Error ("Overflow in hexadecimal number");
     		IVal = 0;
     	    }
     	    IVal = (IVal << 4) + DigitVal (C);
     	    NextChar ();
     	}

     	/* This is an integer constant */
    	Tok = TOK_INTCON;
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
	IVal = 0;
	while (IsBDigit (C)) {
	    if (IVal & 0x80000000) {
	      	Error ("Overflow in binary number");
	      	IVal = 0;
	    }
	    IVal = (IVal << 1) + DigitVal (C);
	    NextChar ();
 	}

	/* This is an integer constant */
	Tok = TOK_INTCON;
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
        while (IsXDigit (C)) {

            /* Buf is big enough to allow any decimal and hex number to
             * overflow, so ignore excess digits here, they will be detected
             * when we convert the value.
             */
            if (Digits < sizeof (Buf)) {
                Buf[Digits++] = C;
            }

            NextChar ();
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
	IVal = 0;
        for (I = 0; I < Digits; ++I) {
       	    if (IVal > Max) {
       	       	Error ("Number out of range");
	       	IVal = 0;
                break;
	    }
            DVal = DigitVal (Buf[I]);
            if (DVal > Base) {
                Error ("Invalid digits in number");
                IVal = 0;
                break;
            }
	    IVal = (IVal * Base) + DVal;
        }

	/* This is an integer constant */
       	Tok = TOK_INTCON;
      	return;
    }

    /* Control command? */
    if (C == '.') {

	/* Remember and skip the dot */
	NextChar ();

    	/* Check if it's just a dot */
       	if (!IsIdStart (C)) {

	    /* Just a dot */
	    Tok = TOK_DOT;

	} else {

	    /* Read the remainder of the identifier */
            SB_AppendChar (&SVal, '.');
	    ReadIdent ();

	    /* Dot keyword, search for it */
	    Tok = FindDotKeyword ();
	    if (Tok == TOK_NONE) {

      	    	/* Not found */
		if (!LeadingDotInIdents) {
		    /* Invalid pseudo instruction */
		    Error ("`%m%p' is not a recognized control command", &SVal);
		    goto Again;
		}

		/* An identifier with a dot. Check if it's a define style
		 * macro.
		 */
       	       	if (IsDefine (&SVal)) {
 		    /* This is a define style macro - expand it */
		    MacExpandStart ();
		    goto Restart;
		}

		/* Just an identifier with a dot */
		Tok = TOK_IDENT;
	    }

	}
	return;
    }

    /* Indirect op for sweet16 cpu. Must check this before checking for local
     * symbols, because these may also use the '@' symbol.
     */
    if (CPU == CPU_SWEET16 && C == '@') {
        NextChar ();
        Tok = TOK_AT;
        return;
    }

    /* Local symbol? */
    if (C == LocalStart) {

    	/* Read the identifier. */
    	ReadIdent ();

     	/* Start character alone is not enough */
        if (SB_GetLen (&SVal) == 1) {
	    Error ("Invalid cheap local symbol");
       	    goto Again;
	}

       	/* A local identifier */
	Tok = TOK_LOCAL_IDENT;
	return;
    }


    /* Identifier or keyword? */
    if (IsIdStart (C)) {

    	/* Read the identifier */
    	ReadIdent ();

       	/* Check for special names. Bail out if we have identified the type of
	 * the token. Go on if the token is an identifier.
	 */
        if (SB_GetLen (&SVal) == 1) {
    	    switch (toupper (SB_AtUnchecked (&SVal, 0))) {

    	     	case 'A':
                    if (C == ':') {
                        NextChar ();
                        Tok = TOK_OVERRIDE_ABS;
                    } else {
    	     	        Tok = TOK_A;
                    }
    	            return;

                case 'F':
                    if (C == ':') {
                        NextChar ();
                        Tok = TOK_OVERRIDE_FAR;
		       	return;
                    }
		    break;

	        case 'S':
	     	    Tok = TOK_S;
	     	    return;

    	     	case 'X':
     	     	    Tok = TOK_X;
	     	    return;

 	      	case 'Y':
	     	    Tok = TOK_Y;
	     	    return;

                case 'Z':
                    if (C == ':') {
                        NextChar ();
                        Tok = TOK_OVERRIDE_ZP;
		       	return;
                    }
                    break;

      	      	default:
	     	    break;
   	    }

	} else if (CPU == CPU_SWEET16 && (IVal = Sweet16Reg (&SVal)) >= 0) {

            /* A sweet16 register number in sweet16 mode */
            Tok = TOK_REG;
            return;

        }

	/* Check for define style macro */
       	if (IsDefine (&SVal)) {
	    /* Macro - expand it */
	    MacExpandStart ();
	    goto Restart;
	} else {
	    /* An identifier */
	    Tok = TOK_IDENT;
	}
	return;
    }

    /* Ok, let's do the switch */
CharAgain:
    switch (C) {

	case '+':
	    NextChar ();
	    Tok = TOK_PLUS;
	    return;

	case '-':
	    NextChar ();
     	    Tok = TOK_MINUS;
	    return;

	case '/':
	    NextChar ();
            if (C != '*') {
                Tok = TOK_DIV;
            } else if (CComments) {
                /* Remember the position, then skip the '*' */
                FilePos Pos = CurPos;
                NextChar ();
                do {
                    while (C !=  '*') {
                        if (C == EOF) {
                            PError (&Pos, "Unterminated comment");
                            goto CharAgain;
                        }
                        NextChar ();
                    }
                    NextChar ();
                } while (C != '/');
                NextChar ();
                goto Again;
            }
	    return;

	case '*':
	    NextChar ();
	    Tok = TOK_MUL;
	    return;

	case '^':
	    NextChar ();
      	    Tok = TOK_XOR;
	    return;

	case '&':
   	    NextChar ();
	    if (C == '&') {
	    	NextChar ();
	    	Tok = TOK_BOOLAND;
	    } else {
	        Tok = TOK_AND;
	    }
       	    return;

	case '|':
	    NextChar ();
	    if (C == '|') {
	    	NextChar ();
	     	Tok = TOK_BOOLOR;
	    } else {
	        Tok = TOK_OR;
	    }
	    return;

	case ':':
	    NextChar ();
      	    switch (C) {

		case ':':
		    NextChar ();
		    Tok = TOK_NAMESPACE;
		    break;

		case '-':
		    IVal = 0;
		    do {
     		     	--IVal;
	  	     	NextChar ();
		    } while (C == '-');
		    Tok = TOK_ULABEL;
		    break;

		case '+':
		    IVal = 0;
		    do {
		     	++IVal;
	     	     	NextChar ();
	       	    } while (C == '+');
	 	    Tok = TOK_ULABEL;
		    break;

                case '=':
                    NextChar ();
                    Tok = TOK_ASSIGN;
                    break;

		default:
	            Tok = TOK_COLON;
		    break;
	    }
	    return;

	case ',':
	    NextChar ();
	    Tok = TOK_COMMA;
	    return;

	case ';':
	    NextChar ();
	    while (C != '\n' && C != EOF) {
	    	NextChar ();
	    }
	    goto CharAgain;

	case '#':
	    NextChar ();
      	    Tok = TOK_HASH;
	    return;

	case '(':
	    NextChar ();
	    Tok = TOK_LPAREN;
	    return;

	case ')':
       	    NextChar ();
            Tok = TOK_RPAREN;
	    return;

	case '[':
	    NextChar ();
	    Tok = TOK_LBRACK;
	    return;

	case ']':
	    NextChar ();
	    Tok = TOK_RBRACK;
	    return;

	case '{':
	    NextChar ();
      	    Tok = TOK_LCURLY;
	    return;

	case '}':
	    NextChar ();
	    Tok = TOK_RCURLY;
	    return;

	case '<':
 	    NextChar ();
	    if (C == '=') {
	     	NextChar ();
	      	Tok = TOK_LE;
	    } else if (C == '<') {
		NextChar ();
		Tok = TOK_SHL;
	    } else if (C == '>') {
	      	NextChar ();
		Tok = TOK_NE;
	    } else {
		Tok = TOK_LT;
	    }
	    return;

	case '=':
      	    NextChar ();
       	    Tok = TOK_EQ;
	    return;

	case '!':
	    NextChar ();
	    Tok = TOK_BOOLNOT;
   	    return;

	case '>':
     	    NextChar ();
	    if (C == '=') {
		NextChar ();
		Tok = TOK_GE;
       	    } else if (C == '>') {
		NextChar ();
	       	Tok = TOK_SHR;
	    } else {
	       	Tok = TOK_GT;
	    }
	    return;

        case '~':
	    NextChar ();
	    Tok = TOK_NOT;
      	    return;

 	case '\'':
	    /* Hack: If we allow ' as terminating character for strings, read
	     * the following stuff as a string, and check for a one character
	     * string later.
	     */
	    if (LooseStringTerm) {
		ReadStringConst ('\'');
                if (SB_GetLen (&SVal) == 1) {
		    IVal = SB_AtUnchecked (&SVal, 0);
	      	    Tok = TOK_CHARCON;
		} else {
		    Tok = TOK_STRCON;
		}
	    } else {
		/* Always a character constant */
	     	NextChar ();
	     	if (C == EOF || IsControl (C)) {
	     	    Error ("Illegal character constant");
	     	    goto CharAgain;
	     	}
	     	IVal = C;
	     	Tok = TOK_CHARCON;
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
	    Tok = TOK_STRCON;
	    return;

	case '\\':
	    /* Line continuation? */
	    if (LineCont) {
	    	NextChar ();
	    	if (C == '\n') {
     		    /* Handle as white space */
     		    NextChar ();
     		    C = ' ';
     	     	    goto Again;
     	       	}
     	    }
      	    break;

        case '\n':
     	    NextChar ();
     	    Tok = TOK_SEP;
     	    return;

        case EOF:
            CheckInputStack ();
            /* In case of the main file, do not close it, but return EOF. */
            if (Source && Source->Next) {
                DoneCharSource ();
                goto Again;
            } else {
     	     	Tok = TOK_EOF;
            }
            return;
    }

    /* If we go here, we could not identify the current character. Skip it
     * and try again.
     */
    Error ("Invalid input character: 0x%02X", C & 0xFF);
    NextChar ();
    goto Again;
}



int GetSubKey (const char** Keys, unsigned Count)
/* Search for a subkey in a table of keywords. The current token must be an
 * identifier and all keys must be in upper case. The identifier will be
 * uppercased in the process. The function returns the index of the keyword,
 * or -1 if the keyword was not found.
 */
{
    unsigned I;

    /* Must have an identifier */
    PRECONDITION (Tok == TOK_IDENT);

    /* If we aren't in ignore case mode, we have to uppercase the identifier */
    if (!IgnoreCase) {
     	UpcaseSVal ();
    }

    /* Do a linear search (a binary search is not worth the effort) */
    for (I = 0; I < Count; ++I) {
       	if (SB_CompareStr (&SVal, Keys [I]) == 0) {
     	    /* Found it */
     	    return I;
     	}
    }

    /* Not found */
    return -1;
}



unsigned char ParseAddrSize (void)
/* Check if the next token is a keyword that denotes an address size specifier.
 * If so, return the corresponding address size constant, otherwise output an
 * error message and return ADDR_SIZE_DEFAULT.
 */
{
    static const char* Keys[] = {
        "DIRECT", "ZEROPAGE", "ZP",
        "ABSOLUTE", "ABS", "NEAR",
        "FAR",
        "LONG", "DWORD",
    };

    /* Check for an identifier */
    if (Tok != TOK_IDENT) {
        Error ("Address size specifier expected");
        return ADDR_SIZE_DEFAULT;
    }

    /* Search for the attribute */
    switch (GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]))) {
        case 0:
        case 1:
        case 2: return ADDR_SIZE_ZP;
        case 3:
        case 4:
        case 5: return ADDR_SIZE_ABS;
        case 6: return ADDR_SIZE_FAR;
        case 7:
        case 8: return ADDR_SIZE_LONG;
        default:
            Error ("Address size specifier expected");
            return ADDR_SIZE_DEFAULT;
    }
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



