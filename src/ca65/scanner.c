/*****************************************************************************/
/*                                                                           */
/*				   scanner.c				     */
/*                                                                           */
/*		    The scanner for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

/* common */
#include "check.h"
#include "fname.h"
#include "xmalloc.h"

/* ca65 */
#include "condasm.h"
#include "error.h"
#include "global.h"
#include "incpath.h"
#include "instr.h"
#include "istack.h"
#include "listing.h"
#include "macro.h"
#include "objfile.h"
#include "toklist.h"
#include "scanner.h"



/*****************************************************************************/
/*     	       	    	       	     Data 				     */
/*****************************************************************************/



enum Token Tok = TOK_NONE;		/* Current token */
int WS;	  				/* Flag: Whitespace before token */
long IVal;	       	       	    	/* Integer token attribute */
char SVal [MAX_STR_LEN+1];	    	/* String token attribute */

FilePos	CurPos = { 0, 0, 0 };		/* Name and position in current file */



/* Struct to handle include files. Note: The length of the input line may
 * not exceed 255+1, since the column is stored in the file position struct
 * as a character. Increasing this value means changing the FilePos struct,
 * and the read and write routines in the assembler and linker.
 */
typedef struct InputFile_ InputFile;
struct InputFile_ {
    FILE*      	    F;		       	/* Input file descriptor */
    FilePos	    Pos;	       	/* Position in file */
    enum Token	    Tok;	       	/* Last token */
    int		    C;			/* Last character */
    char       	    Line[256];		/* The current input line */
    InputFile*	    Next;      	       	/* Linked list of input files */
};

/* Struct to handle textual input data */
typedef struct InputData_ InputData;
struct InputData_ {
    const char*	    Data;		/* Pointer to the data */
    const char*     Pos;		/* Pointer to current position */
    int		    Malloced;		/* Memory was malloced */
    enum Token	    Tok;	    	/* Last token */
    int		    C;			/* Last character */
    InputData*	    Next;		/* Linked list of input data */
};

/* List of input files */
static struct {
    unsigned long  MTime;		/* Time of last modification */
    unsigned long  Size;		/* Size of file */
    const char*	   Name;		/* Name of file */
} Files [MAX_INPUT_FILES];
static unsigned    FileCount = 0;

/* Current input variables */
static InputFile* IFile        	= 0;	/* Current input file */
static InputData* IData        	= 0;	/* Current input memory data */
static unsigned	  ICount 	= 0;  	/* Count of input files */
static int	  C 		= 0;	/* Current input character */

/* Force end of assembly */
int 		  ForcedEnd = 0;

/* List of dot keywords with the corresponding tokens */
struct DotKeyword {
    const char*	Key;			/* MUST be first field */
    enum Token  Tok;
} DotKeywords [] = {
    { "A16",  	    	TOK_A16		},
    { "A8",   	    	TOK_A8		},
    { "ADDR", 	    	TOK_ADDR	},
    { "ALIGN",	       	TOK_ALIGN     	},
    { "AND", 	    	TOK_BAND 	},
    { "ASCIIZ",	    	TOK_ASCIIZ	},
    { "AUTOIMPORT", 	TOK_AUTOIMPORT	},
    { "BITAND",	    	TOK_AND		},
    { "BITNOT",	    	TOK_NOT		},
    { "BITOR",	    	TOK_OR		},
    { "BITXOR",	    	TOK_XOR		},
    { "BLANK",	    	TOK_BLANK	},
    { "BSS",  	    	TOK_BSS		},
    { "BYTE", 	    	TOK_BYTE	},
    { "CASE",  	    	TOK_CASE	},
    { "CODE", 	    	TOK_CODE    	},
    { "CONCAT",		TOK_CONCAT	},
    { "CONST", 	    	TOK_CONST	},
    { "CPU", 		TOK_CPU		},
    { "DATA",  		TOK_DATA	},
    { "DBYT",  		TOK_DBYT	},
    { "DEBUGINFO",	TOK_DEBUGINFO	},
    { "DEF",   		TOK_DEFINED	},
    { "DEFINE",	    	TOK_DEFINE	},
    { "DEFINED",	TOK_DEFINED	},
    { "DWORD", 		TOK_DWORD	},
    { "ELSE",  		TOK_ELSE	},
    { "ELSEIF",		TOK_ELSEIF	},
    { "END",   	    	TOK_END		},
    { "ENDIF", 		TOK_ENDIF	},
    { "ENDMAC",		TOK_ENDMACRO	},
    { "ENDMACRO",	TOK_ENDMACRO	},
    { "ENDPROC",	TOK_ENDPROC 	},
    { "ENDREP",		TOK_ENDREP	},
    { "ENDREPEAT",	TOK_ENDREP	},
    { "ERROR", 		TOK_ERROR   	},
    { "EXITMAC",	TOK_EXITMACRO 	},
    { "EXITMACRO",	TOK_EXITMACRO	},
    { "EXPORT",		TOK_EXPORT	},
    { "EXPORTZP",	TOK_EXPORTZP	},
    { "FARADDR",	TOK_FARADDR	},
    { "FEATURE",	TOK_FEATURE	},
    { "FILEOPT",	TOK_FILEOPT	},
    { "FOPT",  		TOK_FILEOPT	},
    { "FORCEWORD",	TOK_FORCEWORD	},
    { "GLOBAL",		TOK_GLOBAL	},
    { "GLOBALZP",	TOK_GLOBALZP	},
    { "I16",   		TOK_I16		},
    { "I8",    		TOK_I8		},
    { "IF",    		TOK_IF		},
    { "IFBLANK",	TOK_IFBLANK	},
    { "IFCONST",	TOK_IFCONST	},
    { "IFDEF", 		TOK_IFDEF	},
    { "IFNBLANK",	TOK_IFNBLANK	},
    { "IFNCONST",	TOK_IFNCONST	},
    { "IFNDEF",		TOK_IFNDEF	},
    { "IFNREF",		TOK_IFNREF	},
    { "IFP02",		TOK_IFP02	},
    { "IFP816",		TOK_IFP816	},
    { "IFPC02",	    	TOK_IFPC02	},
    { "IFREF",		TOK_IFREF	},
    { "IMPORT",		TOK_IMPORT	},
    { "IMPORTZP",	TOK_IMPORTZP	},
    { "INCBIN",		TOK_INCBIN	},
    { "INCLUDE",    	TOK_INCLUDE 	},
    { "LEFT",		TOK_LEFT	},
    { "LINECONT",	TOK_LINECONT	},
    { "LIST",		TOK_LIST	},
    { "LISTBYTES",	TOK_LISTBYTES	},
    { "LOCAL",		TOK_LOCAL	},
    { "LOCALCHAR",	TOK_LOCALCHAR	},
    { "MAC",  		TOK_MACRO	},
    { "MACPACK",	TOK_MACPACK	},
    { "MACRO",	  	TOK_MACRO	},
    { "MATCH",		TOK_MATCH	},
    { "MID",   	       	TOK_MID		},
    { "MOD", 		TOK_MOD		},
    { "NOT", 		TOK_BNOT 	},
    { "NULL",		TOK_NULL	},
    { "OR",  		TOK_BOR  	},
    { "ORG",  		TOK_ORG		},
    { "OUT",  		TOK_OUT		},
    { "P02",  		TOK_P02		},
    { "P816", 		TOK_P816	},
    { "PAGELEN",	TOK_PAGELENGTH	},
    { "PAGELENGTH",	TOK_PAGELENGTH	},
    { "PARAMCOUNT", 	TOK_PARAMCOUNT  },
    { "PC02", 		TOK_PC02	},
    { "PROC", 		TOK_PROC	},
    { "REF", 		TOK_REFERENCED  },
    { "REFERENCED",	TOK_REFERENCED  },
    { "RELOC",		TOK_RELOC	},
    { "REPEAT",		TOK_REPEAT	},
    { "RES",  		TOK_RES		},
    { "RIGHT",	  	TOK_RIGHT	},
    { "RODATA",	  	TOK_RODATA	},
    { "SEGMENT",  	TOK_SEGMENT	},
    { "SHL", 	  	TOK_SHL		},
    { "SHR", 	  	TOK_SHR		},
    { "SMART",	  	TOK_SMART	},
    { "STRAT",		TOK_STRAT	},
    { "STRING",	  	TOK_STRING	},
    { "STRLEN",		TOK_STRLEN	},
    { "SUNPLUS",  	TOK_SUNPLUS	},
    { "TCOUNT",	  	TOK_TCOUNT	},
    { "WARNING",	TOK_WARNING	},
    { "WORD", 	  	TOK_WORD	},
    { "XMATCH",	  	TOK_XMATCH	},
    { "XOR",  	  	TOK_BXOR  	},
    { "ZEROPAGE", 	TOK_ZEROPAGE	},
};



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



static void NextChar (void);
/* Read the next character from the input file */



/*****************************************************************************/
/*		      Character classification functions		     */
/*****************************************************************************/



static int IsBlank (int C)
/* Return true if the character is a blank or tab */
{
    return (C == ' ' || C == '\t');
}



static int IsDigit (int C)
/* Return true if the character is a digit */
{
    return isdigit (C);
}



static int IsXDigit (int C)
/* Return true if the character is a hexadecimal digit */
{
    return isxdigit (C);
}



static int IsDDigit (int C)
/* Return true if the character is a dual digit */
{
    return (C == '0' || C == '1');
}



static int IsIdChar (int C)
/* Return true if the character is a valid character for an identifier */
{
    return isalnum (C) 			||
	   (C == '_')			||
	   (C == '@' && AtInIdents)	||
	   (C == '$' && DollarInIdents);
}



static int IsIdStart (int C)
/* Return true if the character may start an identifier */
{
    return isalpha (C) || C == '_';
}



/*****************************************************************************/
/*     	       	     	       	     Code				     */
/*****************************************************************************/



const char* GetFileName (unsigned char Name)
/* Get the name of a file where the name index is known */
{
    PRECONDITION (Name <= FileCount);
    if (Name == 0) {
	/* Name was defined outside any file scope, use the name of the first
	 * file instead. Errors are then reported with a file position of
     	 * line zero in the first file.
	 */
	if (FileCount == 0) {
    	    /* No files defined until now */
       	    return "(outside file scope)";
	} else {
	    return Files [0].Name;
	}
    } else {
        return Files [Name-1].Name;
    }
}



void NewInputFile (const char* Name)
/* Open a new input file */
{
    InputFile* I;
    FILE* F;

    /* Check for nested include overflow */
    if (FileCount >= MAX_INPUT_FILES) {
     	Fatal (FAT_MAX_INPUT_FILES);
    }

    /* First try to open the file */
    F = fopen (Name, "r");
    if (F == 0) {

	char* PathName;

     	/* Error (fatal error if this is the main file) */
     	if (ICount == 0) {
     	    Fatal (FAT_CANNOT_OPEN_INPUT, Name, strerror (errno));
       	}

       	/* We are on include level. Search for the file in the include
     	 * directories.
     	 */
     	PathName = FindInclude (Name);
       	if (PathName == 0 || (F = fopen (PathName, "r")) == 0) {
     	    /* Not found or cannot open, print an error and bail out */
     	    Error (ERR_CANNOT_OPEN_INCLUDE, Name, strerror (errno));
     	}

     	/* Free the allocated memory */
     	xfree (PathName);

    }

    /* check again if we do now have an open file */
    if (F != 0) {

     	/* Stat the file and remember the values */
     	struct stat Buf;
     	if (fstat (fileno (F), &Buf) != 0) {
     	    Fatal (FAT_CANNOT_STAT_INPUT, Name, strerror (errno));
     	}
     	Files [FileCount].MTime = Buf.st_mtime;
     	Files [FileCount].Size  = Buf.st_size;
     	Files [FileCount].Name  = xstrdup (Name);
     	++FileCount;

     	/* Create a new state variable and initialize it */
     	I  	    = xmalloc (sizeof (*I));
     	I->F   	    = F;
     	I->Pos.Line = 0;
     	I->Pos.Col  = 0;
     	I->Pos.Name = FileCount;
     	I->Tok      = Tok;
     	I->C	    = C;
     	I->Line[0]  = '\0';

     	/* Use the new file */
     	I->Next	    = IFile;
     	IFile 	    = I;
     	++ICount;

     	/* Prime the pump */
     	NextChar ();
    }
}



void DoneInputFile (void)
/* Close the current input file */
{
    InputFile* I;

    /* Restore the old token */
    Tok = IFile->Tok;
    C   = IFile->C;

    /* Save a pointer to the current struct, then set it back */
    I     = IFile;
    IFile = I->Next;

    /* Cleanup the current stuff */
    fclose (I->F);
    xfree (I);
    --ICount;
}



void NewInputData (const char* Data, int Malloced)
/* Add a chunk of input data to the input stream */
{
    InputData* I;

    /* Create a new state variable and initialize it */
    I  	      	= xmalloc (sizeof (*I));
    I->Data   	= Data;
    I->Pos    	= Data;
    I->Malloced = Malloced;
    I->Tok     	= Tok;
    I->C        = C;

    /* Use the new data */
    I->Next   	= IData;
    IData     	= I;

    /* Prime the pump */
    NextChar ();
}



static void DoneInputData (void)
/* End the current input data stream */
{
    InputData* I;

    /* Restore the old token */
    Tok = IData->Tok;
    C   = IData->C;

    /* Save a pointer to the current struct, then set it back */
    I     = IData;
    IData = I->Next;

    /* Cleanup the current stuff */
    if (I->Malloced) {
	xfree (I->Data);
    }
    xfree (I);
}



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
    /* If we have an input data structure, read from there */
    if (IData) {

       	C = *IData->Pos++;
       	if (C == '\0') {
       	    /* End of input data, will set to last file char */
       	    DoneInputData ();
       	}

    } else {

      	/* Check for end of line, read the next line if needed */
       	while (IFile->Line [IFile->Pos.Col] == '\0') {

      	    /* End of current line reached, read next line */
      	    if (fgets (IFile->Line, sizeof (IFile->Line), IFile->F) == 0) {
      	       	/* End of file. Add an empty line to the listing. This is a
      		 * small hack needed to keep the PC output in sync.
      		 */
      	      	NewListingLine ("", IFile->Pos.Name, ICount);
      	       	C = EOF;
      	       	return;
      	    }

      	    /* One more line */
      	    IFile->Pos.Line++;
      	    IFile->Pos.Col = 0;

      	    /* Remember the new line for the listing */
      	    NewListingLine (IFile->Line, IFile->Pos.Name, ICount);

      	}

      	/* Return the next character from the file */
      	C = IFile->Line [IFile->Pos.Col++];

    }
}



void UpcaseSVal (void)
/* Make SVal upper case */
{
    unsigned I = 0;
    while (SVal [I]) {
	SVal [I] = toupper (SVal [I]);
     	++I;
    }
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
    static const struct DotKeyword K = { SVal, 0 };
    struct DotKeyword* R;

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
/* Read an identifier from the current input position into Ident. It is
 * assumed that the first character has already been checked.
 */
{
    /* Read the identifier */
    unsigned I = 0;
    do {
   	if (I < MAX_STR_LEN) {
	    SVal [I++] = C;
	}
	NextChar ();
    } while (IsIdChar (C));
    SVal [I] = '\0';

    /* If we should ignore case, convert the identifier to upper case */
    if (IgnoreCase) {
	UpcaseSVal ();
    }
}



static unsigned ReadStringConst (int StringTerm)
/* Read a string constant into SVal. Check for maximum string length and all
 * other stuff.	The length of the string is returned.
 */
{
    unsigned I;

    /* Skip the leading string terminator */
    NextChar ();

    /* Read the string */
    I = 0;
    while (1) {
	if (C == StringTerm) {
	    break;
	}
	if (C == '\n' || C == EOF) {
	    Error (ERR_NEWLINE_IN_STRING);
	    break;
      	}

	/* Check for string length, print an error message once */
	if (I == MAX_STR_LEN) {
	    Error (ERR_STRING_TOO_LONG);
	} else if (I < MAX_STR_LEN) {
	    SVal [I] = C;
	}
	++I;

	/* Skip the character */
	NextChar ();
    }

    /* Skip the trailing terminator */
    NextChar ();

    /* Terminate the string */
    if (I >= MAX_STR_LEN) {
	I = MAX_STR_LEN;
    }
    SVal [I] = '\0';

    /* Return the length of the string */
    return I;
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

    /* If we're reading from the file, update the location from where the
     * next token will be read. If we're reading from input data, keep the
     * current position.
     */
    if (IData == 0) {
        CurPos = IFile->Pos;
    }

    /* Hex number or PC symbol? */
    if (C == '$') {
     	NextChar ();

     	/* Hex digit must follow or DollarIsPC must be enabled */
     	if (!IsXDigit (C)) {
	    if (DollarIsPC) {
	     	Tok = TOK_PC;
		return;
	    } else {
     	     	Error (ERR_HEX_DIGIT_EXPECTED);
	    }
     	}

     	/* Read the number */
     	IVal = 0;
     	while (IsXDigit (C)) {
     	    if (IVal & 0xF0000000) {
     	    	Error (ERR_NUM_OVERFLOW);
     		IVal = 0;
     	    }
     	    IVal = (IVal << 4) + DigitVal (C);
     	    NextChar ();
     	}

     	/* This is an integer constant */
    	Tok = TOK_INTCON;
    	return;
    }

    /* Dual number? */
    if (C == '%') {
	NextChar ();

	/* 0 or 1 must follow */
	if (!IsDDigit (C)) {
	    Error (ERR_01_EXPECTED);
	}

	/* Read the number */
	IVal = 0;
	while (IsDDigit (C)) {
	    if (IVal & 0x80000000) {
	      	Error (ERR_NUM_OVERFLOW);
	      	IVal = 0;
	    }
	    IVal = (IVal << 1) + DigitVal (C);
	    NextChar ();
 	}

	/* This is an integer constant */
	Tok = TOK_INTCON;
	return;
    }

    /* Decimal number? */
    if (IsDigit (C)) {

	/* Read the number */
	IVal = 0;
	while (IsDigit (C)) {
	    if (IVal > (0xFFFFFFFF / 10)) {
       	      	Error (ERR_NUM_OVERFLOW);
	    	IVal = 0;
	    }
	    IVal = (IVal * 10) + DigitVal (C);
	    NextChar ();
	}

	/* This is an integer constant */
       	Tok = TOK_INTCON;
	return;
    }

    /* Control command? */
    if (C == '.') {

	NextChar ();

	if (!IsIdStart (C)) {
	    Error (ERR_PSEUDO_EXPECTED);
	    /* Try to read an identifier */
	    goto Again;
	}

	/* Read the identifier */
	ReadIdent ();

	/* Search the keyword */
	Tok = FindDotKeyword ();
	if (Tok == TOK_NONE) {
 	    /* Not found */
	    Error (ERR_PSEUDO_EXPECTED);
	    goto Again;
	}
	return;
    }

    /* Local symbol? */
    if (C == LocalStart) {

    	/* Read the identifier */
    	ReadIdent ();

     	/* Start character alone is not enough */
        if (SVal [1] == '\0') {
	    Error (ERR_IDENT_EXPECTED);
       	    goto Again;
	}

       	/* An identifier */
	Tok = TOK_IDENT;
	return;
    }


    /* Identifier or keyword? */
    if (IsIdStart (C)) {

    	/* Read the identifier */
    	ReadIdent ();

       	/* Check for special names */
        if (SVal [1] == '\0') {
    	    switch (toupper (SVal [0])) {

    	    	case 'A':
    	    	    Tok = TOK_A;
    	            return;

    	     	case 'X':
    	    	    Tok = TOK_X;
	    	    return;

 	      	case 'Y':
	    	    Tok = TOK_Y;
	    	    return;

	        case 'S':
	    	    Tok = TOK_S;
	    	    return;

	      	default:
	    	    Tok = TOK_IDENT;
	    	    return;
   	    }
	}

	/* Search for an opcode */
	IVal = FindInstruction (SVal);
	if (IVal >= 0) {
	    /* This is a mnemonic */
       	    Tok = TOK_MNEMO;
       	} else if (IsDefine (SVal)) {
	    /* This is a define style macro - expand it */
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
	    Tok = TOK_DIV;
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
	    	Tok = TOK_BAND;
	    } else {
	        Tok = TOK_AND;
	    }
       	    return;

	case '|':
	    NextChar ();
	    if (C == '|') {
	    	NextChar ();
	     	Tok = TOK_BOR;
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
	    Tok = TOK_BNOT;
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
		if (ReadStringConst ('\'') == 1) {
		    IVal = SVal[0];
	      	    Tok = TOK_CHARCON;
		} else {
		    Tok = TOK_STRCON;
		}
	    } else {
		/* Always a character constant */
	     	NextChar ();
	     	if (C == '\n' || C == EOF) {
	     	    Error (ERR_ILLEGAL_CHARCON);
	     	    goto CharAgain;
	     	}
	     	IVal = C;
	     	Tok = TOK_CHARCON;
	     	NextChar ();
	     	if (C != '\'') {
	     	    Error (ERR_ILLEGAL_CHARCON);
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
	    /* Check if we have any open .IFs in this file */
	    CheckOpenIfs ();
	    /* Check if we have any open token lists in this file */
	    CheckInputStack ();

	    /* If this was an include file, then close it and handle like a
	     * separator. Do not close the main file, but return EOF.
	     */
	    if (ICount > 1) {
	    	DoneInputFile ();
	    } else {
		Tok = TOK_EOF;
	    }
	    return;

    }

    /* If we go here, we could not identify the current character. Skip it
     * and try again.
     */
    Error (ERR_INVALID_CHAR, C & 0xFF);
    NextChar ();
    goto Again;
}



int TokHasSVal (enum Token Tok)
/* Return true if the given token has an attached SVal */
{
    return (Tok == TOK_IDENT || Tok == TOK_STRCON);
}



int TokHasIVal (enum Token Tok)
/* Return true if the given token has an attached IVal */
{
    return (Tok == TOK_INTCON || Tok == TOK_CHARCON || Tok == TOK_MNEMO);
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
	if (strcmp (SVal, Keys [I]) == 0) {
	    /* Found it */
	    return I;
	}
    }

    /* Not found */
    return -1;
}



void WriteFiles (void)
/* Write the list of input files to the object file */
{
    unsigned I;

    /* Tell the obj file module that we're about to start the file list */
    ObjStartFiles ();

    /* Write the file count */
    ObjWrite8 (FileCount);

    /* Write the file data */
    for (I = 0; I < FileCount; ++I) {
	ObjWrite32 (Files [I].MTime);
	ObjWrite32 (Files [I].Size);
	ObjWriteStr (Files [I].Name);
    }

    /* Done writing files */
    ObjEndFiles ();
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
    DoneInputFile ();
}



