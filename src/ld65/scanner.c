/*****************************************************************************/
/*                                                                           */
/*				   scanner.c				     */
/*                                                                           */
/*		Configuration file scanner for the ld65 linker		     */
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



#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/* common */
#include "chartype.h"
#include "strbuf.h"
#include "xsprintf.h"

/* ld65 */
#include "global.h"
#include "error.h"
#include "scanner.h"



/*****************************************************************************/
/*     	       	       	       	     Data     				     */
/*****************************************************************************/



/* Current token and attributes */
cfgtok_t	CfgTok;
char   	       	CfgSVal [CFG_MAX_IDENT_LEN+1];
unsigned long   CfgIVal;

/* Error location */
unsigned        	CfgErrorLine;
unsigned        	CfgErrorCol;

/* Input sources for the configuration */
static const char*     	CfgName		= 0;
static const char*      CfgBuf 		= 0;

/* Other input stuff */
static int     	       	C      	     	= ' ';
static unsigned	       	InputLine    	= 1;
static unsigned	       	InputCol     	= 0;
static FILE*   	       	InputFile    	= 0;



/*****************************************************************************/
/*	  	    	       	Error handling				     */
/*****************************************************************************/



void CfgWarning (const char* Format, ...)
/* Print a warning message adding file name and line number of the config file */
{
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;
    va_list ap;

    va_start (ap, Format);
    SB_VPrintf (&Buf, Format, ap);
    va_end (ap);

    Warning ("%s(%u): %s", CfgGetName(), CfgErrorLine, SB_GetConstBuf (&Buf));
    DoneStrBuf (&Buf);
}



void CfgError (const char* Format, ...)
/* Print an error message adding file name and line number of the config file */
{
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;
    va_list ap;

    va_start (ap, Format);
    SB_VPrintf (&Buf, Format, ap);
    va_end (ap);

    Error ("%s(%u): %s", CfgGetName(), CfgErrorLine, SB_GetConstBuf (&Buf));
    DoneStrBuf (&Buf);
}



/*****************************************************************************/
/*     	       	       	       	     Code     				     */
/*****************************************************************************/



static void NextChar (void)
/* Read the next character from the input file */
{
    if (CfgBuf) {
	/* Read from buffer */
	C = (unsigned char)(*CfgBuf);
	if (C == 0) {
     	    C = EOF;
	} else {
	    ++CfgBuf;
	}
    } else {
	/* Read from the file */
	C = getc (InputFile);
    }

    /* Count columns */
    if (C != EOF) {
	++InputCol;
    }

    /* Count lines */
    if (C == '\n') {
     	++InputLine;
     	InputCol = 0;
    }
}



static unsigned DigitVal (int C)
/* Return the value for a numeric digit */
{
    if (isdigit (C)) {
	return C - '0';
    } else {
	return toupper (C) - 'A' + 10;
    }
}



void CfgNextTok (void)
/* Read the next token from the input stream */
{
    unsigned I;


Again:
    /* Skip whitespace */
    while (isspace (C)) {
     	NextChar ();
    }

    /* Remember the current position */
    CfgErrorLine = InputLine;
    CfgErrorCol  = InputCol;

    /* Identifier? */
    if (C == '_' || IsAlpha (C)) {

	/* Read the identifier */
	I = 0;
	while (C == '_' || IsAlNum (C)) {
	    if (I < CFG_MAX_IDENT_LEN) {
	        CfgSVal [I++] = C;
	    }
	    NextChar ();
     	}
	CfgSVal [I] = '\0';
     	CfgTok = CFGTOK_IDENT;
	return;
    }

    /* Hex number? */
    if (C == '$') {
	NextChar ();
	if (!isxdigit (C)) {
	    CfgError ("Hex digit expected");
	}
	CfgIVal = 0;
	while (isxdigit (C)) {
       	    CfgIVal = CfgIVal * 16 + DigitVal (C);
	    NextChar ();
	}
	CfgTok = CFGTOK_INTCON;
	return;
    }

    /* Decimal number? */
    if (isdigit (C)) {
	CfgIVal = 0;
	while (isdigit (C)) {
       	    CfgIVal = CfgIVal * 10 + DigitVal (C);
	    NextChar ();
	}
	CfgTok = CFGTOK_INTCON;
	return;
    }

    /* Other characters */
    switch (C) {

        case '-':
            NextChar ();
            CfgTok = CFGTOK_MINUS;
            break;

        case '+':
            NextChar ();
            CfgTok = CFGTOK_PLUS;
            break;

        case '*':
            NextChar ();
            CfgTok = CFGTOK_MUL;
            break;

        case '/':
            NextChar ();
            CfgTok = CFGTOK_DIV;
            break;

	case '(':
	    NextChar ();
	    CfgTok = CFGTOK_LPAR;
	    break;

	case ')':
	    NextChar ();
	    CfgTok = CFGTOK_RPAR;
	    break;

	case '{':
	    NextChar ();
	    CfgTok = CFGTOK_LCURLY;
	    break;

	case '}':
	    NextChar ();
	    CfgTok = CFGTOK_RCURLY;
	    break;

	case ';':
	    NextChar ();
     	    CfgTok = CFGTOK_SEMI;
	    break;

	case '.':
	    NextChar ();
	    CfgTok = CFGTOK_DOT;
	    break;

	case ',':
	    NextChar ();
	    CfgTok = CFGTOK_COMMA;
	    break;

	case '=':
	    NextChar ();
	    CfgTok = CFGTOK_EQ;
	    break;

        case ':':
	    NextChar ();
	    CfgTok = CFGTOK_COLON;
     	    break;

        case '\"':
	    NextChar ();
	    I = 0;
	    while (C != '\"') {
		if (C == EOF || C == '\n') {
		    CfgError ("Unterminated string");
		}
	       	if (I < CFG_MAX_IDENT_LEN) {
		    CfgSVal [I++] = C;
		}
		NextChar ();
	    }
       	    NextChar ();
	    CfgSVal [I] = '\0';
	    CfgTok = CFGTOK_STRCON;
	    break;

        case '#':
	    /* Comment */
	    while (C != '\n' && C != EOF) {
   		NextChar ();
	    }
     	    if (C != EOF) {
	     	goto Again;
	    }
	    CfgTok = CFGTOK_EOF;
	    break;

        case '%':
	    NextChar ();
	    switch (C) {

	        case 'O':
		    NextChar ();
		    if (OutputName) {
		        strncpy (CfgSVal, OutputName, CFG_MAX_IDENT_LEN);
		    	CfgSVal [CFG_MAX_IDENT_LEN] = '\0';
		    } else {
		    	CfgSVal [0] = '\0';
		    }
		    CfgTok = CFGTOK_STRCON;
     		    break;

	        case 'S':
		    NextChar ();
		    CfgIVal = StartAddr;
		    CfgTok = CFGTOK_INTCON;
		    break;

	        default:
	            CfgError ("Invalid format specification");
	    }
	    break;

        case EOF:
	    CfgTok = CFGTOK_EOF;
	    break;

	default:
	    CfgError ("Invalid character `%c'", C);

    }
}



void CfgConsume (cfgtok_t T, const char* Msg)
/* Skip a token, print an error message if not found */
{
    if (CfgTok != T) {
       	CfgError (Msg);
    }
    CfgNextTok ();
}



void CfgConsumeSemi (void)
/* Consume a semicolon */
{
    CfgConsume (CFGTOK_SEMI, "`;' expected");
}



void CfgConsumeColon (void)
/* Consume a colon */
{
    CfgConsume (CFGTOK_COLON, "`:' expected");
}



void CfgOptionalComma (void)
/* Consume a comma if there is one */
{
    if (CfgTok == CFGTOK_COMMA) {
       	CfgNextTok ();
    }
}



void CfgOptionalAssign (void)
/* Consume an equal sign if there is one */
{
    if (CfgTok == CFGTOK_EQ) {
       	CfgNextTok ();
    }
}



void CfgAssureInt (void)
/* Make sure the next token is an integer */
{
    if (CfgTok != CFGTOK_INTCON) {
       	CfgError ("Integer constant expected");
    }
}



void CfgAssureStr (void)
/* Make sure the next token is a string constant */
{
    if (CfgTok != CFGTOK_STRCON) {
       	CfgError ("String constant expected");
    }
}



void CfgAssureIdent (void)
/* Make sure the next token is an identifier */
{
    if (CfgTok != CFGTOK_IDENT) {
       	CfgError ("Identifier expected");
    }
}



void CfgRangeCheck (unsigned long Lo, unsigned long Hi)
/* Check the range of CfgIVal */
{
    if (CfgIVal < Lo || CfgIVal > Hi) {
	CfgError ("Range error");
    }
}



void CfgSpecialToken (const IdentTok* Table, unsigned Size, const char* Name)
/* Map an identifier to one of the special tokens in the table */
{
    unsigned I;

    /* We need an identifier */
    if (CfgTok == CFGTOK_IDENT) {

	/* Make it upper case */
	I = 0;
	while (CfgSVal [I]) {
	    CfgSVal [I] = toupper (CfgSVal [I]);
	    ++I;
	}

       	/* Linear search */
	for (I = 0; I < Size; ++I) {
     	    if (strcmp (CfgSVal, Table [I].Ident) == 0) {
	    	CfgTok = Table [I].Tok;
	    	return;
	    }
	}

    }

    /* Not found or no identifier */
    CfgError ("%s expected", Name);
}



void CfgBoolToken (void)
/* Map an identifier or integer to a boolean token */
{
    static const IdentTok Booleans [] = {
       	{   "YES",     	CFGTOK_TRUE     },
	{   "NO",    	CFGTOK_FALSE    },
        {   "TRUE",     CFGTOK_TRUE     },
        {   "FALSE",    CFGTOK_FALSE    },
    };

    /* If we have an identifier, map it to a boolean token */
    if (CfgTok == CFGTOK_IDENT) {
	CfgSpecialToken (Booleans, ENTRY_COUNT (Booleans), "Boolean");
    } else {
	/* We expected an integer here */
	if (CfgTok != CFGTOK_INTCON) {
     	    CfgError ("Boolean value expected");
	}
	CfgTok = (CfgIVal == 0)? CFGTOK_FALSE : CFGTOK_TRUE;
    }
}



void CfgSetName (const char* Name)
/* Set a name for a config file */
{
    CfgName = Name;
}



const char* CfgGetName (void)
/* Get the name of the config file */
{
    if (CfgName) {
	return CfgName;
    } else if (CfgBuf) {
	return "[builtin config]";
    } else {
	return "";
    }
}



void CfgSetBuf (const char* Buf)
/* Set a memory buffer for the config */
{
    CfgBuf = Buf;
}



int CfgAvail (void)
/* Return true if we have a configuration available */
{
    return CfgName != 0 || CfgBuf != 0;
}



void CfgOpenInput (void)
/* Open the input file if we have one */
{
    /* If we have a config name given, open the file, otherwise we will read
     * from a buffer.
     */
    if (!CfgBuf) {

	/* Open the file */
	InputFile = fopen (CfgName, "r");
	if (InputFile == 0) {
	    Error ("Cannot open `%s': %s", CfgName, strerror (errno));
	}

    }

    /* Initialize variables */
    C         = ' ';
    InputLine = 1;
    InputCol  = 0;

    /* Start the ball rolling ... */
    CfgNextTok ();
}



void CfgCloseInput (void)
/* Close the input file if we have one */
{
    /* Close the input file if we had one */
    if (InputFile) {
        (void) fclose (InputFile);
	InputFile = 0;
    }
}




