/*****************************************************************************/
/*                                                                           */
/*                                 scanstrbuf.c                              */
/*                                                                           */
/*                     Small scanner for input from a StrBuf                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



static char ParseChar (StrBuf* B)
/* Parse a character. Converts \n into EOL, etc. */
{
    unsigned I;
    int C;

    /* Check for escape chars */
    if ((C = SB_Get (B)) == '\\') {
	switch (SB_Get (B)) {
	    case 'b':
	       	C = '\b';
	    	break;
     	    case 'f':
	    	C = '\f';
	    	break;
	    case 'r':
	    	C = '\r';
	    	break;
	    case 'n':
	    	C = '\n';
	    	break;
	    case 't':
	    	C = '\t';
	    	break;
	    case '\"':
	    	C = '\"';
	    	break;
	    case '\'':
	    	C = '\'';
	    	break;
	    case '\\':
		C = '\\';
		break;
	    case 'x':
	    case 'X':
		/* Hex character constant */
       	       	C = HexVal (SB_Get (B)) << 4;
       	       	C |= HexVal (SB_Get (B));
		break;
	    case '0':
		/* Octal constant */
                C = 0;
                goto Octal;
	    case '1':
                /* Octal constant */
                C = 1;
Octal:          I = 0;
       	       	while (SB_Peek (B) >= '0' && SB_Peek (B) <= '7' && I++ < 4) {
                    C = (C << 3) | (SB_Get (B) - '0');
     		}
     	       	break;
     	    default:
     	    	Error ("Illegal character constant");
		C = ' ';
		break;
     	}
    }

    /* Return the character */
    return C;
}







/*****************************************************************************/
/*		     		     Code				     */
/*****************************************************************************/



void SB_SkipWhite (StrBuf* B)
/* Skip whitespace in the string buffer */
{
    while (IsBlank (SB_Peek (B))) {
        SB_Skip (B);
    }
}



int SB_GetSym (StrBuf* B, char* S)
/* Get a symbol from the string buffer. S must be able to hold MAX_IDENTLEN
 * characters. Returns 1 if a symbol was found and 0 otherwise.
 */
{
    if (IsIdent (SB_Peek (B))) {
        unsigned I = 0;
        char C = SB_Peek (B);
        do {
            if (I < MAX_IDENTLEN) {
                ++I;
                *S++ = C;
            }
            SB_Skip (B);
            C = SB_Peek (B);
        } while (IsIdent (C) || IsDigit (C));
        *S = '\0';
     	return 1;
    } else {
     	return 0;
    }
}



int SB_GetString (StrBuf* B, StrBuf* S)
/* Get a string from the string buffer. S will be initialized by the function
 * and will return the correctly terminated string on return. The function
 * returns 1 if a string was found and 0 otherwise.
 */
{
    char C;

    /* Initialize S */
    *S = AUTO_STRBUF_INITIALIZER;
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
 * hex and character constants. Numeric constants may be preceeded by a
 * minus or plus sign. The function returns 1 if a number was found and
 * zero otherwise.
 */
{
    int      Sign;
    char     C;
    unsigned Base;
    unsigned DigitVal;

    /* Initialize Val */
    *Val = 0;

    /* Check for a sign */
    Sign = 1;
    switch (SB_Peek (B)) {
        case '-':
            Sign = -1;
            /* FALLTHROUGH */
        case '+':
            SB_Skip (B);
            SB_SkipWhite (B);
            break;
    }

    /* Check for the different formats */
    C = SB_Peek (B);
    if (IsDigit (C)) {

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

    } else if (C == '\'') {

        /* Character constant */
        SB_Skip (B);
        *Val = SignExtendChar (TgtTranslateChar (ParseChar (B)));
        if (SB_Peek (B) != '\'') {
            Error ("`\'' expected");
            return 0;
        } else {
            /* Skip the quote */
            SB_Skip (B);
        }

    } else {

        /* Invalid number */
        Error ("Numeric constant expected");
        return 0;

    }

    /* Success, value read is in Val */
    return 1;
}



