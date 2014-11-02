/*****************************************************************************/
/*                                                                           */
/*                                 asminc.c                                  */
/*                                                                           */
/*             Read an assembler include file containing symbols             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2005-2008 Ullrich von Bassewitz                                       */
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
#include <errno.h>

/* common */
#include "chartype.h"
#include "strbuf.h"

/* da65 */
#include "asminc.h"
#include "comments.h"
#include "error.h"
#include "labels.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static char* SkipWhitespace (char* L)
/* Ignore white space in L */
{
    while (IsBlank (*L)) {
        ++L;
    }
    return L;
}



unsigned DigitVal (unsigned char C)
/* Return the value of the given digit */
{
    if (IsDigit (C)) {
        return C - '0';
    } else {
        return tolower (C) - 'a' + 10;
    }
}



void AsmInc (const char* Filename, char CommentStart, int IgnoreUnknown)
/* Read an assembler include file */
{
    char        Buf[1024];
    char*       L;
    const char* Comment;
    unsigned    Line;
    unsigned    Len;
    long        Val;
    unsigned    DVal;
    int         Sign;
    unsigned    Base;
    unsigned    Digits;
    StrBuf      Ident = STATIC_STRBUF_INITIALIZER;

    /* Try to open the file for reading */
    FILE* F = fopen (Filename, "r");
    if (F == 0) {
        Error ("Cannot open asm include file \"%s\": %s",
               Filename, strerror (errno));
    }

    /* Read line by line, check for NAME = VALUE lines */
    Line = 0;
    while ((L = fgets (Buf, sizeof (Buf), F)) != 0) {

        /* One more line read */
        ++Line;

        /* Ignore leading white space */
        while (IsBlank (*L)) {
            ++L;
        }

        /* Remove trailing whitespace */
        Len = strlen (L);
        while (Len > 0 && IsSpace (L[Len-1])) {
            --Len;
        }
        L[Len] = '\0';

        /* If the line is empty or starts with a comment char, ignore it */
        if (*L == '\0' || *L == CommentStart) {
            continue;
        }

        /* Read an identifier */
        SB_Clear (&Ident);
        if (IsAlpha (*L) || *L == '_') {
            SB_AppendChar (&Ident, *L++);
            while (IsAlNum (*L) || *L == '_') {
                SB_AppendChar (&Ident, *L++);
            }
            SB_Terminate (&Ident);
        } else {
            if (!IgnoreUnknown) {
                Error ("%s(%u): Syntax error", Filename, Line);
            }
            continue;
        }

        /* Ignore white space */
        L = SkipWhitespace (L);

        /* Check for := or = */
        if (*L == '=') {
            ++L;
        } else if (*L == ':' && *++L == '=') {
            ++L;
        } else {
            if (!IgnoreUnknown) {
                Error ("%s(%u): Missing `='", Filename, Line);
            }
            continue;
        }

        /* Allow white space once again */
        L = SkipWhitespace (L);

        /* A number follows. Read the sign. */
        if (*L == '-') {
            Sign = -1;
            ++L;
        } else {
            Sign = 1;
            if (*L == '+') {
                ++L;
            }
        }

        /* Determine the base of the number. Allow $ and % as prefixes for
        ** hex and binary numbers respectively.
        */
        if (*L == '$') {
            Base = 16;
            ++L;
        } else if (*L == '%') {
            Base = 2;
            ++L;
        } else {
            Base = 10;
        }

        /* Decode the number */
        Digits = 0;
        Val = 0;
        while (IsXDigit (*L) && (DVal = DigitVal (*L)) < Base) {
            Val = (Val * Base) + DVal;
            ++Digits;
            ++L;
        }

        /* Must have at least one digit */
        if (Digits == 0) {
            if (!IgnoreUnknown) {
                Error ("%s(%u): Error in number format", Filename, Line);
            }
            continue;
        }

        /* Skip whitespace again */
        L = SkipWhitespace (L);

        /* Check for a comment */
        if (*L == CommentStart) {
            Comment = SkipWhitespace (L+1);
            if (*Comment == '\0') {
                Comment = 0;
            }
        } else {
            Comment = 0;
        }

        /* Check for a comment character or end of line */
        if (*L != CommentStart && *L != '\0') {
            if (!IgnoreUnknown) {
                Error ("%s(%u): Trailing garbage", Filename, Line);
            }
            continue;
        }

        /* Apply the sign */
        Val *= Sign;

        /* Define the symbol and the comment */
        AddExtLabel (Val, SB_GetConstBuf (&Ident));
        SetComment (Val, Comment);

    }

    /* Delete the string buffer contents */
    SB_Done (&Ident);

    /* Close the include file ignoring errors (we were just reading). */
    (void) fclose (F);
}
