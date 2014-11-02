/*****************************************************************************/
/*                                                                           */
/*                                 scanner.c                                 */
/*                                                                           */
/*              Configuration file scanner for the ld65 linker               */
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
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Current token and attributes */
cfgtok_t        CfgTok;
StrBuf          CfgSVal = STATIC_STRBUF_INITIALIZER;
unsigned long   CfgIVal;

/* Error location */
FilePos                 CfgErrorPos;

/* Input source for the configuration */
static const char*      CfgName         = 0;

/* Other input stuff */
static int              C               = ' ';
static FilePos          InputPos;
static FILE*            InputFile       = 0;



/*****************************************************************************/
/*                              Error handling                               */
/*****************************************************************************/



void CfgWarning (const FilePos* Pos, const char* Format, ...)
/* Print a warning message adding file name and line number of a given file */
{
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;
    va_list ap;

    va_start (ap, Format);
    SB_VPrintf (&Buf, Format, ap);
    va_end (ap);

    Warning ("%s(%u): %s",
             GetString (Pos->Name), Pos->Line, SB_GetConstBuf (&Buf));
    SB_Done (&Buf);
}



void CfgError (const FilePos* Pos, const char* Format, ...)
/* Print an error message adding file name and line number of a given file */
{
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;
    va_list ap;

    va_start (ap, Format);
    SB_VPrintf (&Buf, Format, ap);
    va_end (ap);

    Error ("%s(%u): %s",
           GetString (Pos->Name), Pos->Line, SB_GetConstBuf (&Buf));
    SB_Done (&Buf);
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
        ++InputPos.Col;
    }

    /* Count lines */
    if (C == '\n') {
        ++InputPos.Line;
        InputPos.Col = 0;
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



static void StrVal (void)
/* Parse a string value and expand escape sequences */
{
    /* Skip the starting double quotes */
    NextChar ();

    /* Read input chars */
    SB_Clear (&CfgSVal);
    while (C != '\"') {
        switch (C) {

            case EOF:
            case '\n':
                CfgError (&CfgErrorPos, "Unterminated string");
                break;

            case '%':
                NextChar ();
                switch (C) {

                    case EOF:
                    case '\n':
                    case '\"':
                        CfgError (&CfgErrorPos, "Unterminated '%%' escape sequence");
                        break;

                    case '%':
                        SB_AppendChar (&CfgSVal, '%');
                        NextChar ();
                        break;

                    case 'O':
                        /* Replace by output file */
                        if (OutputName) {
                            SB_AppendStr (&CfgSVal, OutputName);
                        }
                        OutputNameUsed = 1;
                        NextChar ();
                        break;

                    default:
                        CfgWarning (&CfgErrorPos,
                                    "Unkown escape sequence `%%%c'", C);
                        SB_AppendChar (&CfgSVal, '%');
                        SB_AppendChar (&CfgSVal, C);
                        NextChar ();
                        break;
                }
                break;

            default:
                SB_AppendChar (&CfgSVal, C);
                NextChar ();
        }
    }

    /* Skip the terminating double quotes */
    NextChar ();

    /* Terminate the string */
    SB_Terminate (&CfgSVal);

    /* We've read a string value */
    CfgTok = CFGTOK_STRCON;
}



void CfgNextTok (void)
/* Read the next token from the input stream */
{
Again:
    /* Skip whitespace */
    while (isspace (C)) {
        NextChar ();
    }

    /* Remember the current position */
    CfgErrorPos = InputPos;

    /* Identifier? */
    if (C == '_' || IsAlpha (C)) {

        /* Read the identifier */
        SB_Clear (&CfgSVal);
        while (C == '_' || IsAlNum (C)) {
            SB_AppendChar (&CfgSVal, C);
            NextChar ();
        }
        SB_Terminate (&CfgSVal);
        CfgTok = CFGTOK_IDENT;
        return;
    }

    /* Hex number? */
    if (C == '$') {
        NextChar ();
        if (!isxdigit (C)) {
            CfgError (&CfgErrorPos, "Hex digit expected");
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
            StrVal ();
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
                        SB_CopyStr (&CfgSVal, OutputName);
                    } else {
                        SB_Clear (&CfgSVal);
                    }
                    SB_Terminate (&CfgSVal);
                    OutputNameUsed = 1;
                    CfgTok = CFGTOK_STRCON;
                    break;

                case 'S':
                    NextChar ();
                    CfgIVal = StartAddr;
                    CfgTok = CFGTOK_INTCON;
                    break;

                default:
                    CfgError (&CfgErrorPos, "Invalid format specification");
            }
            break;

        case EOF:
            CfgTok = CFGTOK_EOF;
            break;

        default:
            CfgError (&CfgErrorPos, "Invalid character `%c'", C);

    }
}



void CfgConsume (cfgtok_t T, const char* Msg)
/* Skip a token, print an error message if not found */
{
    if (CfgTok != T) {
        CfgError (&CfgErrorPos, "%s", Msg);
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
        CfgError (&CfgErrorPos, "Integer constant expected");
    }
}



void CfgAssureStr (void)
/* Make sure the next token is a string constant */
{
    if (CfgTok != CFGTOK_STRCON) {
        CfgError (&CfgErrorPos, "String constant expected");
    }
}



void CfgAssureIdent (void)
/* Make sure the next token is an identifier */
{
    if (CfgTok != CFGTOK_IDENT) {
        CfgError (&CfgErrorPos, "Identifier expected");
    }
}



void CfgRangeCheck (unsigned long Lo, unsigned long Hi)
/* Check the range of CfgIVal */
{
    if (CfgIVal < Lo || CfgIVal > Hi) {
        CfgError (&CfgErrorPos, "Range error");
    }
}



void CfgSpecialToken (const IdentTok* Table, unsigned Size, const char* Name)
/* Map an identifier to one of the special tokens in the table */
{
    unsigned I;

    /* We need an identifier */
    if (CfgTok == CFGTOK_IDENT) {

        /* Make it upper case */
        SB_ToUpper (&CfgSVal);

        /* Linear search */
        for (I = 0; I < Size; ++I) {
            if (SB_CompareStr (&CfgSVal, Table[I].Ident) == 0) {
                CfgTok = Table[I].Tok;
                return;
            }
        }

    }

    /* Not found or no identifier */
    CfgError (&CfgErrorPos, "%s expected", Name);
}



void CfgBoolToken (void)
/* Map an identifier or integer to a boolean token */
{
    static const IdentTok Booleans [] = {
        {   "YES",      CFGTOK_TRUE     },
        {   "NO",       CFGTOK_FALSE    },
        {   "TRUE",     CFGTOK_TRUE     },
        {   "FALSE",    CFGTOK_FALSE    },
    };

    /* If we have an identifier, map it to a boolean token */
    if (CfgTok == CFGTOK_IDENT) {
        CfgSpecialToken (Booleans, ENTRY_COUNT (Booleans), "Boolean");
    } else {
        /* We expected an integer here */
        if (CfgTok != CFGTOK_INTCON) {
            CfgError (&CfgErrorPos, "Boolean value expected");
        }
        CfgTok = (CfgIVal == 0)? CFGTOK_FALSE : CFGTOK_TRUE;
    }
}



void CfgSetName (const char* Name)
/* Set a name for a config file */
{
    CfgName = Name;
}



int CfgAvail (void)
/* Return true if we have a configuration available */
{
    return CfgName != 0;
}



void CfgOpenInput (void)
/* Open the input file if we have one */
{
    /* Open the file */
    InputFile = fopen (CfgName, "r");
    if (InputFile == 0) {
        Error ("Cannot open `%s': %s", CfgName, strerror (errno));
    }

    /* Initialize variables */
    C         = ' ';
    InputPos.Line = 1;
    InputPos.Col  = 0;
    InputPos.Name = GetStringId (CfgName);

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
