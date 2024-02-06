/*****************************************************************************/
/*                                                                           */
/*                                  matchpat.c                               */
/*                                                                           */
/*                       Unix shell like pattern matching                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#include <string.h>

/* common */
#include "matchpat.h"



/*****************************************************************************/
/*                       Character bit set implementation                    */
/*****************************************************************************/



typedef unsigned char CharSet[32];      /* 256 bits */



/* Clear a character set */
#define CS_CLEAR(CS)            memset (CS, 0, sizeof (CharSet))

/* Set all characters in the set */
#define CS_SETALL(CS)           memset (CS, 0xFF, sizeof (CharSet))

/* Add one char to the set */
#define CS_ADD(CS, C)           ((CS)[(C) >> 3] |= (0x01 << ((C) & 0x07)))

/* Check if a character is a member of the set */
#define CS_CONTAINS(CS, C)      ((CS)[(C) >> 3] & (0x01 << ((C) & 0x07)))

/* Invert a character set */
#define CS_INVERT(CS)                                   \
    do {                                                \
        unsigned I;                                     \
        for (I = 0; I < sizeof (CharSet); ++I) {        \
            CS[I] ^= 0xFF;                              \
        }                                               \
    } while (0)





/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* Escape character */
#define ESCAPE_CHAR     '\\'

/* Utility macro used in RecursiveMatch */
#define IncPattern()    Pattern++;                      \
                        if (*Pattern == '\0') {         \
                            return 0;                   \
                        }



static int RealChar (const unsigned char* Pattern)
/* Return the next character from Pattern. If the next character is the
** escape character, skip it and return the following.
*/
{
    if (*Pattern == ESCAPE_CHAR) {
        Pattern++;
        return (*Pattern == '\0') ? -1 : *Pattern;
    } else {
        return *Pattern;
    }
}



static int RecursiveMatch (const unsigned char* Source, const unsigned char* Pattern)
/* A recursive pattern matcher */
{

    CharSet CS;

    while (1) {

        if (*Pattern == '\0') {

            /* Reached the end of Pattern, what about Source? */
            return (*Source == '\0') ? 1 : 0;

        } else if (*Pattern == '*') {

            if (*++Pattern == '\0') {
                /* A trailing '*' is always a match */
                return 1;
            }

            /* Check the rest of the string */
            while (*Source) {
                if (RecursiveMatch (Source++, Pattern)) {
                    /* Match! */
                    return 1;
                }
            }

            /* No match... */
            return 0;

        } else if (*Source == '\0') {

            /* End of Source reached, no match */
            return 0;

        } else {

            /* Check a single char. Build a set of all possible characters in
            ** CS, then check if the current char of Source is contained in
            ** there.
            */
            CS_CLEAR (CS);      /* Clear the character set */

            if (*Pattern == '?') {

                /* All chars are allowed */
                CS_SETALL (CS);
                ++Pattern;                      /* Skip '?' */

            } else if (*Pattern == ESCAPE_CHAR) {

                /* Use the next char as is */
                IncPattern ();
                CS_ADD (CS, *Pattern);
                ++Pattern;                      /* Skip the character */

            } else if (*Pattern == '[') {

                /* A set follows */
                int Invert = 0;
                IncPattern ();
                if (*Pattern == '!') {
                    IncPattern ();
                    Invert = 1;
                }
                while (*Pattern != ']') {

                    int C1;
                    if ((C1 = RealChar (Pattern)) == -1) {
                        return 0;
                    }
                    IncPattern ();
                    if (*Pattern != '-') {
                        CS_ADD (CS, C1);
                    } else {
                        int C2;
                        unsigned char C;
                        IncPattern ();
                        if ((C2 = RealChar (Pattern)) == -1) {
                            return 0;
                        }
                        IncPattern ();
                        for (C = C1; C <= C2; C++) {
                            CS_ADD (CS, C);
                        }
                    }
                }
                /* Skip ']' */
                ++Pattern;
                if (Invert) {
                    /* Reverse all bits in the set */
                    CS_INVERT (CS);
                }

            } else {

                /* Include the char in the charset, then skip it */
                CS_ADD (CS, *Pattern);
                ++Pattern;

            }

            if (!CS_CONTAINS (CS, *Source)) {
                /* No match */
                return 0;
            }
            ++Source;
        }
    }
}




int MatchPattern (const char* Source, const char* Pattern)
/* Match the string in Source against Pattern. Pattern may contain the
** wildcards '*', '?', '[abcd]' '[ab-d]', '[!abcd]', '[!ab-d]'. The
** function returns a value of zero if Source does not match Pattern,
** otherwise a non zero value is returned. If Pattern contains an invalid
** wildcard pattern (e.g. 'A[x'), the function returns zero.
*/
{
    /* Handle the trivial cases */
    if (Pattern == 0 || *Pattern == '\0') {
        return (Source == 0 || *Source == '\0');
    }

    /* Do the real thing */
    return RecursiveMatch ((const unsigned char*) Source, (const unsigned char*) Pattern);
}
