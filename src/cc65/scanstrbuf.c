/*****************************************************************************/
/*                                                                           */
/*                                 scanstrbuf.c                              */
/*                                                                           */
/*                     Small scanner for input from a StrBuf                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2009, Ullrich von Bassewitz                                      */
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



/* common */
#include "chartype.h"
#include "tgttrans.h"

/* cc65 */
#include "datatype.h"
#include "error.h"
#include "hexval.h"
#include "ident.h"
#include "scanstrbuf.h"



/*****************************************************************************/
/*                               Helper functions                            */
/*****************************************************************************/



static int ParseChar (StrBuf* B)
/* Parse a character. Converts \n into EOL, etc. */
{
    unsigned I;
    unsigned Val;
    int C;

    /* Check for escape chars */
    if ((C = SB_Get (B)) == '\\') {
        switch (SB_Peek (B)) {
            case '?':
                C = '?';
                SB_Skip (B);
                break;
            case 'a':
                C = '\a';
                SB_Skip (B);
                break;
            case 'b':
                C = '\b';
                SB_Skip (B);
                break;
            case 'f':
                C = '\f';
                SB_Skip (B);
                break;
            case 'r':
                C = '\r';
                SB_Skip (B);
                break;
            case 'n':
                C = '\n';
                SB_Skip (B);
                break;
            case 't':
                C = '\t';
                SB_Skip (B);
                break;
            case 'v':
                C = '\v';
                SB_Skip (B);
                break;
            case '\"':
                C = '\"';
                SB_Skip (B);
                break;
            case '\'':
                C = '\'';
                SB_Skip (B);
                break;
            case '\\':
                C = '\\';
                SB_Skip (B);
                break;
            case 'x':
            case 'X':
                /* Hex character constant */
                SB_Skip (B);
                C = HexVal (SB_Get (B)) << 4;
                C |= HexVal (SB_Get (B));
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                /* Octal constant */
                I = 0;
                Val = SB_Get (B) - '0';
                while (SB_Peek (B) >= '0' && SB_Peek (B) <= '7' && ++I <= 3) {
                    Val = (Val << 3) | (SB_Get (B) - '0');
                }
                C = (int) Val;
                if (Val > 256) {
                    Error ("Character constant out of range");
                    C = ' ';
                }
                break;
            default:
                Error ("Illegal character constant 0x%02X", SB_Get (B));
                C = ' ';
                break;
        }
    }

    /* Return the character */
    return C;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void SB_SkipWhite (StrBuf* B)
/* Skip whitespace in the string buffer */
{
    while (IsBlank (SB_Peek (B))) {
        SB_Skip (B);
    }
}



int SB_GetSym (StrBuf* B, StrBuf* Ident, const char* SpecialChars)
/* Get a symbol from the string buffer. If SpecialChars is not NULL, it
** points to a string that contains characters allowed within the string in
** addition to letters, digits and the underline. Note: The identifier must
** still begin with a letter.
** Returns 1 if a symbol was found and 0 otherwise but doesn't output any
** errors.
*/
{
    /* Handle a NULL argument for SpecialChars transparently */
    if (SpecialChars == 0) {
        SpecialChars = "";
    }

    /* Clear Ident */
    SB_Clear (Ident);

    if (IsIdent (SB_Peek (B))) {
        char C = SB_Peek (B);
        do {
            SB_AppendChar (Ident, C);
            SB_Skip (B);
            C = SB_Peek (B);
        } while (IsIdent (C) || IsDigit (C) || 
                 (C != '\0' && strchr (SpecialChars, C) != 0));
        SB_Terminate (Ident);
        return 1;
    } else {
        return 0;
    }
}



int SB_GetString (StrBuf* B, StrBuf* S)
/* Get a string from the string buffer. Returns 1 if a string was found and 0
** otherwise. Errors are only output in case of invalid strings (missing end
** of string).
*/
{
    char C;

    /* Clear S */
    SB_Clear (S);

    /* A string starts with quote marks */
    if (SB_Peek (B) == '\"') {

        /* String follows, be sure to concatenate strings */
        while (SB_Peek (B) == '\"') {

            /* Skip the quote char */
            SB_Skip (B);

            /* Read the actual string contents */
            while ((C = SB_Peek (B)) != '\"') {
                if (C == '\0') {
                    Error ("Unexpected end of string");
                    break;
                }
                SB_AppendChar (S, ParseChar (B));
            }

            /* Skip the closing quote char if there was one */
            SB_Skip (B);

            /* Skip white space, read new input */
            SB_SkipWhite (B);
        }

        /* Terminate the string */
        SB_Terminate (S);

        /* Success */
        return 1;

    } else {

        /* Not a string */
        SB_Terminate (S);
        return 0;
    }
}



int SB_GetNumber (StrBuf* B, long* Val)
/* Get a number from the string buffer. Accepted formats are decimal, octal,
** hex and character constants. Numeric constants may be preceeded by a
** minus or plus sign. The function returns 1 if a number was found and
** zero otherwise. Errors are only output for invalid numbers.
*/
{
    int      Sign;
    char     C;
    unsigned Base;
    unsigned DigitVal;


    /* Initialize Val */
    *Val = 0;

    /* Handle character constants */
    if (SB_Peek (B) == '\'') {

        /* Character constant */
        SB_Skip (B);
        *Val = SignExtendChar (TgtTranslateChar (ParseChar (B)));
        if (SB_Peek (B) != '\'') {
            Error ("`\'' expected");
            return 0;
        } else {
            /* Skip the quote */
            SB_Skip (B);
            return 1;
        }
    }

    /* Check for a sign. A sign must be followed by a digit, otherwise it's
    ** not a number
    */
    Sign = 1;
    switch (SB_Peek (B)) {
        case '-':
            Sign = -1;
            /* FALLTHROUGH */
        case '+':
            if (!IsDigit (SB_LookAt (B, SB_GetIndex (B) + 1))) {
                return 0;
            }
            SB_Skip (B);
            break;
    }

    /* We must have a digit now, otherwise its not a number */
    C = SB_Peek (B);
    if (!IsDigit (C)) {
        return 0;
    }

    /* Determine the base */
    if (C == '0') {
        /* Hex or octal */
        SB_Skip (B);
        if (tolower (SB_Peek (B)) == 'x') {
            SB_Skip (B);
            Base = 16;
            if (!IsXDigit (SB_Peek (B))) {
                Error ("Invalid hexadecimal number");
                return 0;
            }
        } else {
            Base = 8;
        }
    } else {
        Base = 10;
    }

    /* Read the number */
    while (IsXDigit (C = SB_Peek (B)) && (DigitVal = HexVal (C)) < Base) {
        *Val = (*Val * Base) + DigitVal;
        SB_Skip (B);
    }

    /* Allow optional 'U' and 'L' modifiers */
    C = SB_Peek (B);
    if (C == 'u' || C == 'U') {
        SB_Skip (B);
        C = SB_Peek (B);
        if (C == 'l' || C == 'L') {
            SB_Skip (B);
        }
    } else if (C == 'l' || C == 'L') {
        SB_Skip (B);
        C = SB_Peek (B);
        if (C == 'u' || C == 'U') {
            SB_Skip (B);
        }
    }

    /* Success, value read is in Val */
    *Val *= Sign;
    return 1;
}
