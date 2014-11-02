/*****************************************************************************/
/*                                                                           */
/*                                 scanner.c                                 */
/*                                                                           */
/*           Configuration file scanner for the da65 disassembler            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2005 Ullrich von Bassewitz                                       */
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



#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "chartype.h"
#include "xsprintf.h"

/* ld65 */
#include "global.h"
#include "error.h"
#include "scanner.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Current token and attributes */
unsigned        InfoTok;
char            InfoSVal [CFG_MAX_IDENT_LEN+1];
long            InfoIVal;

/* Error location */
unsigned                InfoErrorLine;
unsigned                InfoErrorCol;

/* Input sources for the configuration */
static const char*      InfoFile        = 0;

/* Other input stuff */
static int              C               = ' ';
static unsigned         InputLine       = 1;
static unsigned         InputCol        = 0;
static FILE*            InputFile       = 0;



/*****************************************************************************/
/*                              Error handling                               */
/*****************************************************************************/



void InfoWarning (const char* Format, ...)
/* Print a warning message adding file name and line number of the config file */
{
    char Buf [512];
    va_list ap;

    va_start (ap, Format);
    xvsprintf (Buf, sizeof (Buf), Format, ap);
    va_end (ap);

    Warning ("%s(%u): %s", InfoFile, InfoErrorLine, Buf);
}



void InfoError (const char* Format, ...)
/* Print an error message adding file name and line number of the config file */
{
    char Buf [512];
    va_list ap;

    va_start (ap, Format);
    xvsprintf (Buf, sizeof (Buf), Format, ap);
    va_end (ap);

    Error ("%s(%u): %s", InfoFile, InfoErrorLine, Buf);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void NextChar (void)
/* Read the next character from the input file */
{
    /* Read from the file */
    C = getc (InputFile);

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
    if (IsDigit (C)) {
        return C - '0';
    } else {
        return toupper (C) - 'A' + 10;
    }
}



void InfoNextTok (void)
/* Read the next token from the input stream */
{
    unsigned I;
    int      Esc;

Again:
    /* Skip whitespace */
    while (IsSpace (C)) {
        NextChar ();
    }

    /* Remember the current position */
    InfoErrorLine = InputLine;
    InfoErrorCol  = InputCol;

    /* Identifier? */
    if (C == '_' || IsAlpha (C)) {

        /* Read the identifier */
        I = 0;
        while (C == '_' || IsAlNum (C)) {
            if (I < CFG_MAX_IDENT_LEN) {
                InfoSVal [I++] = C;
            }
            NextChar ();
        }
        InfoSVal [I] = '\0';
        InfoTok = INFOTOK_IDENT;
        return;
    }

    /* Hex number? */
    if (C == '$') {
        NextChar ();
        if (!IsXDigit (C)) {
            InfoError ("Hex digit expected");
        }
        InfoIVal = 0;
        while (IsXDigit (C)) {
            InfoIVal = InfoIVal * 16 + DigitVal (C);
            NextChar ();
        }
        InfoTok = INFOTOK_INTCON;
        return;
    }

    /* Decimal number? */
    if (IsDigit (C)) {
        InfoIVal = 0;
        while (IsDigit (C)) {
            InfoIVal = InfoIVal * 10 + DigitVal (C);
            NextChar ();
        }
        InfoTok = INFOTOK_INTCON;
        return;
    }

    /* Other characters */
    switch (C) {

        case '{':
            NextChar ();
            InfoTok = INFOTOK_LCURLY;
            break;

        case '}':
            NextChar ();
            InfoTok = INFOTOK_RCURLY;
            break;

        case ';':
            NextChar ();
            InfoTok = INFOTOK_SEMI;
            break;

        case '.':
            NextChar ();
            InfoTok = INFOTOK_DOT;
            break;

        case ',':
            NextChar ();
            InfoTok = INFOTOK_COMMA;
            break;

        case '=':
            NextChar ();
            InfoTok = INFOTOK_EQ;
            break;

        case ':':
            NextChar ();
            InfoTok = INFOTOK_COLON;
            break;

        case '\"':
            NextChar ();
            I = 0;
            while (C != '\"') {
                Esc = (C == '\\');
                if (Esc) {
                    NextChar ();
                }
                if (C == EOF || C == '\n') {
                    InfoError ("Unterminated string");
                }
                if (Esc) {
                    switch (C) {
                        case '\"':      C = '\"';       break;
                        case '\'':      C = '\'';       break;
                        default:        InfoError ("Invalid escape char: %c", C);
                    }
                }
                if (I < CFG_MAX_IDENT_LEN) {
                    InfoSVal [I++] = C;
                }
                NextChar ();
            }
            NextChar ();
            InfoSVal [I] = '\0';
            InfoTok = INFOTOK_STRCON;
            break;

        case '\'':
            NextChar ();
            if (C == EOF || IsControl (C)) {
                InfoError ("Invalid character constant");
            }
            InfoIVal = C;
            NextChar ();
            if (C != '\'') {
                InfoError ("Unterminated character constant");
            }
            NextChar ();
            InfoTok = INFOTOK_CHARCON;
            break;

        case '#':
            /* Comment */
            while (C != '\n' && C != EOF) {
                NextChar ();
            }
            if (C != EOF) {
                goto Again;
            }
            InfoTok = INFOTOK_EOF;
            break;

        case EOF:
            InfoTok = INFOTOK_EOF;
            break;

        default:
            InfoError ("Invalid character `%c'", C);

    }
}



void InfoConsume (unsigned T, const char* Msg)
/* Skip a token, print an error message if not found */
{
    if (InfoTok != T) {
        InfoError (Msg);
    }
    InfoNextTok ();
}



void InfoConsumeLCurly (void)
/* Consume a left curly brace */
{
    InfoConsume (INFOTOK_LCURLY, "`{' expected");
}



void InfoConsumeRCurly (void)
/* Consume a right curly brace */
{
    InfoConsume (INFOTOK_RCURLY, "`}' expected");
}



void InfoConsumeSemi (void)
/* Consume a semicolon */
{
    InfoConsume (INFOTOK_SEMI, "`;' expected");
}



void InfoConsumeColon (void)
/* Consume a colon */
{
    InfoConsume (INFOTOK_COLON, "`:' expected");
}



void InfoOptionalComma (void)
/* Consume a comma if there is one */
{
    if (InfoTok == INFOTOK_COMMA) {
        InfoNextTok ();
    }
}



void InfoOptionalAssign (void)
/* Consume an equal sign if there is one */
{
    if (InfoTok == INFOTOK_EQ) {
        InfoNextTok ();
    }
}



void InfoAssureInt (void)
/* Make sure the next token is an integer */
{
    if (InfoTok != INFOTOK_INTCON) {
        InfoError ("Integer constant expected");
    }
}



void InfoAssureStr (void)
/* Make sure the next token is a string constant */
{
    if (InfoTok != INFOTOK_STRCON) {
        InfoError ("String constant expected");
    }
}



void InfoAssureChar (void)
/* Make sure the next token is a char constant */
{
    if (InfoTok != INFOTOK_STRCON) {
        InfoError ("Character constant expected");
    }
}



void InfoAssureIdent (void)
/* Make sure the next token is an identifier */
{
    if (InfoTok != INFOTOK_IDENT) {
        InfoError ("Identifier expected");
    }
}



void InfoRangeCheck (long Lo, long Hi)
/* Check the range of InfoIVal */
{
    if (InfoIVal < Lo || InfoIVal > Hi) {
        InfoError ("Range error");
    }
}



void InfoSpecialToken (const IdentTok* Table, unsigned Size, const char* Name)
/* Map an identifier to one of the special tokens in the table */
{
    unsigned I;

    /* We need an identifier */
    if (InfoTok == INFOTOK_IDENT) {

        /* Make it upper case */
        I = 0;
        while (InfoSVal [I]) {
            InfoSVal [I] = toupper (InfoSVal [I]);
            ++I;
        }

        /* Linear search */
        for (I = 0; I < Size; ++I) {
            if (strcmp (InfoSVal, Table [I].Ident) == 0) {
                InfoTok = Table [I].Tok;
                return;
            }
        }

    }

    /* Not found or no identifier */
    InfoError ("%s expected", Name);
}



void InfoBoolToken (void)
/* Map an identifier or integer to a boolean token */
{
    static const IdentTok Booleans [] = {
        {   "YES",      INFOTOK_TRUE     },
        {   "NO",       INFOTOK_FALSE    },
        {   "TRUE",     INFOTOK_TRUE     },
        {   "FALSE",    INFOTOK_FALSE    },
        {   "ON",       INFOTOK_TRUE     },
        {   "OFF",      INFOTOK_FALSE    },
    };

    /* If we have an identifier, map it to a boolean token */
    if (InfoTok == INFOTOK_IDENT) {
        InfoSpecialToken (Booleans, ENTRY_COUNT (Booleans), "Boolean");
    } else {
        /* We expected an integer here */
        if (InfoTok != INFOTOK_INTCON) {
            InfoError ("Boolean value expected");
        }
        InfoTok = (InfoIVal == 0)? INFOTOK_FALSE : INFOTOK_TRUE;
    }
}



void InfoSetName (const char* Name)
/* Set a name for a config file */
{
    InfoFile = Name;
}



const char* InfoGetName (void)
/* Get the name of the config file */
{
    return InfoFile? InfoFile : "";
}



int InfoAvail ()
/* Return true if we have an info file given */
{
    return (InfoFile != 0);
}



void InfoOpenInput (void)
/* Open the input file */
{
    /* Open the file */
    InputFile = fopen (InfoFile, "r");
    if (InputFile == 0) {
        Error ("Cannot open `%s': %s", InfoFile, strerror (errno));
    }

    /* Initialize variables */
    C         = ' ';
    InputLine = 1;
    InputCol  = 0;

    /* Start the ball rolling ... */
    InfoNextTok ();
}



void InfoCloseInput (void)
/* Close the input file if we have one */
{
    /* Close the input file if we had one */
    if (InputFile) {
        (void) fclose (InputFile);
        InputFile = 0;
    }
}
