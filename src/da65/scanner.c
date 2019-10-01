/*****************************************************************************/
/*                                                                           */
/*                                 scanner.c                                 */
/*                                                                           */
/*           Configuration file scanner for the da65 disassembler            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2005 Ullrich von Bassewitz                                       */
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



#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "chartype.h"
#include "xsprintf.h"
#include "xmalloc.h"
#include "strbuf.h"

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
static char*            InputSrcName    = 0;



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

    fprintf (stderr, "%s(%u): Warning: %s\n",
            InputSrcName, InfoErrorLine, Buf);
}



void InfoError (const char* Format, ...)
/* Print an error message adding file name and line number of the config file */
{
    char Buf [512];
    va_list ap;

    va_start (ap, Format);
    xvsprintf (Buf, sizeof (Buf), Format, ap);
    va_end (ap);

    fprintf (stderr, "%s(%u): Error: %s\n",
            InputSrcName, InfoErrorLine, Buf);
    exit (EXIT_FAILURE);
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



static void SkipBlanks (int SingleLine)
{
    while (C != EOF && (!SingleLine || C != '\n') && IsSpace (C)) {
        NextChar ();
    }
}



static long GetDecimalToken (void)
{
    long Value = 0;

    while (C != EOF && IsDigit (C)) {
        Value = Value * 10 + DigitVal (C);
        NextChar ();
    }
    return Value;
}



static int GetEncodedChar (char* Buf, unsigned* IPtr, unsigned Size)
{
    char Decoded = 0;
    int Count;

    if (C == EOF) {
        return -1;
    } else if (C != '\\') {
        Decoded = C;
        NextChar ();
        goto Store;
    }
    NextChar (); /* consume '\\' */
    if (C == EOF) {
        return -1;
    } else if (IsODigit (C)) {
        Count = 3;
        do {
            Decoded = Decoded * 8 + DigitVal (C);
            NextChar ();
            --Count;
        } while (Count > 0 && C != EOF && IsODigit (C));
    } else if (C == 'x') {
        NextChar (); /* consume 'x' */
        Count = 2;
        while (Count > 0 && C != EOF && IsXDigit (C)) {
            Decoded = Decoded * 16 + DigitVal (C);
            NextChar ();
            --Count;
        }
    } else {
        switch (C) {
            case '"': case '\'': case '\\':
                        Decoded = C;        break;
            case 't':   Decoded = '\t';     break;
            case 'r':   Decoded = '\r';     break;
            case 'n':   Decoded = '\n';     break;
            default:    return -1;
        }
        NextChar ();
    }
Store:
    if (*IPtr < Size - 1) {
        Buf [(*IPtr)++] = Decoded;
    }
    Buf [*IPtr] = 0;
    return 0;
}



static void LineMarkerOrComment ()
/* Handle a line beginning with '#'. Possible interpretations are:
** - #line <lineno> ["<filename>"]          (C preprocessor input)
** - # <lineno> "<filename>" [<flag>]...    (gcc preprocessor output)
** - #<comment>
*/
{
    unsigned long LineNo = 0;
    int LineDirective = 0;
    StrBuf SrcNameBuf = AUTO_STRBUF_INITIALIZER;

    /* Skip the first "# " */
    NextChar ();
    SkipBlanks (1);

    /* Check "line" */
    if (C == 'l') {
        char MaybeLine [6];
        unsigned I;
        for (I = 0; I < sizeof MaybeLine - 1 && C != EOF && IsAlNum (C); ++I) {
            MaybeLine [I] = C;
            NextChar ();
        }
        MaybeLine [I] = 0;
        if (strcmp (MaybeLine, "line") != 0) {
            goto NotMarker;
        }
        LineDirective = 1;
        SkipBlanks (1);
    }

    /* Get line number */
    if (C == EOF || !IsDigit (C)) {
        goto NotMarker;
    }
    LineNo = GetDecimalToken ();
    SkipBlanks (1);

    /* Get the source file name */
    if (C != '\"') {
        /* The source file name is missing */
        if (LineDirective && C == '\n') {
            /* got #line <lineno> */
            NextChar ();
            InputLine = LineNo;
            goto Last;
        } else {
            goto NotMarker;
        }
    }
    NextChar ();
    while (C != EOF && C != '\n' && C != '\"') {
        char DecodeBuf [2];
        unsigned I = 0;
        if (GetEncodedChar (DecodeBuf, &I, sizeof DecodeBuf) < 0) {
            goto BadMarker;
        }
        SB_AppendBuf (&SrcNameBuf, DecodeBuf, I);
    }
    if (C != '\"') {
        goto BadMarker;
    }
    NextChar ();

    /* Ignore until the end of line */
    while (C != EOF && C != '\n') {
        NextChar ();
    }

    /* Accepted a line marker */
    SB_Terminate (&SrcNameBuf);
    xfree (InputSrcName);
    InputSrcName = SB_GetBuf (&SrcNameBuf);
    SB_Init (&SrcNameBuf);
    InputLine = (unsigned)LineNo;
    NextChar ();
    goto Last;

BadMarker:
    InfoWarning ("Bad line marker");
NotMarker:
    while (C != EOF && C != '\n') {
        NextChar ();
    }
    NextChar ();
Last:
    SB_Done (&SrcNameBuf);
}



void InfoNextTok (void)
/* Read the next token from the input stream */
{
    unsigned I;
    char DecodeBuf [2];

Again:
    /* Skip whitespace */
    SkipBlanks (0);

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
        InfoIVal = GetDecimalToken ();
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
            InfoSVal[0] = '\0';
            while (C != EOF && C != '\"') {
                if (GetEncodedChar (InfoSVal, &I, sizeof InfoSVal) < 0) {
                    if (C == EOF) {
                        InfoError ("Unterminated string");
                    } else  {
                        InfoError ("Invalid escape char: %c", C);
                    }
                }
            }
            if (C != '\"') {
                InfoError ("Unterminated string");
            }
            NextChar ();
            InfoTok = INFOTOK_STRCON;
            break;

        case '\'':
            NextChar ();
            if (C == EOF || IsControl (C) || C == '\'') {
                InfoError ("Invalid character constant");
            }
            if (GetEncodedChar (DecodeBuf, &I, sizeof DecodeBuf) < 0 || I != 1) {
                InfoError ("Invalid character constant");
            }
            InfoIVal = DecodeBuf [0];
            if (C != '\'') {
                InfoError ("Unterminated character constant");
            }
            NextChar ();
            InfoTok = INFOTOK_CHARCON;
            break;

        case '#':
            /* # lineno "sourcefile" or # comment */
            if (SyncLines && InputCol == 1) {
                LineMarkerOrComment ();
            } else {
                do {
                    NextChar ();
                } while (C != EOF && C != '\n');
                NextChar ();
            }
            if (C != EOF) {
                goto Again;
            }
            InfoTok = INFOTOK_EOF;
            break;

        case '/':
            /* C++ style comment */
            NextChar ();
            if (C != '/') {
                InfoError ("Invalid token '/'");
            }
            do {
                NextChar ();
            } while (C != '\n' && C != EOF);
            if (C != EOF) {
                goto Again;
            }
            InfoTok = INFOTOK_EOF;
            break;

        case EOF:
            InfoTok = INFOTOK_EOF;
            break;

        default:
            InfoError ("Invalid character '%c'", C);

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
    InfoConsume (INFOTOK_LCURLY, "'{' expected");
}



void InfoConsumeRCurly (void)
/* Consume a right curly brace */
{
    InfoConsume (INFOTOK_RCURLY, "'}' expected");
}



void InfoConsumeSemi (void)
/* Consume a semicolon */
{
    InfoConsume (INFOTOK_SEMI, "';' expected");
}



void InfoConsumeColon (void)
/* Consume a colon */
{
    InfoConsume (INFOTOK_COLON, "':' expected");
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
    xfree(InputSrcName);
    InputSrcName = xstrdup(Name);
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
        Error ("Cannot open '%s': %s", InfoFile, strerror (errno));
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
