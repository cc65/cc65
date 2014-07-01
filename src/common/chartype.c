/*****************************************************************************/
/*                                                                           */
/*                                chartype.c                                 */
/*                                                                           */
/*                    Character classification functions                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
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



#include "chartype.h"



/* This module contains replacements for functions in ctype.h besides other
** functions. There is a problem with using ctype.h directly:
** The parameter must have a value of "unsigned char" or EOF.
** So on platforms where a char is signed, this may give problems or at
** least warnings. The wrapper functions below will have an "char" parameter
** but handle it correctly. They will NOT work for EOF, but this is not a
** problem, since EOF is always handled separately.
*/



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int IsAlpha (char C)
/* Check for a letter */
{
    return (C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z');
}



int IsAlNum (char C)
/* Check for letter or digit */
{
    return (C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z') || (C >= '0' && C <= '9');
}



int IsAscii (char C)
/* Check for an ASCII character */
{
    return (C & ~0x7F) == 0;
}



int IsBlank (char C)
/* Check for a space or tab */
{
    return (C == ' ' || C == '\t');
}



int IsSpace (char C)
/* Check for any white space characters */
{
    return (C == ' ' || C == '\n' || C == '\r' || C == '\t' || C == '\v' || C == '\f');
}



int IsDigit (char C)
/* Check for a digit */
{
    return (C >= '0' && C <= '9');
}



int IsLower (char C)
/* Check for a lower case char */
{
    return (C >= 'a' && C <= 'z');
}



int IsUpper (char C)
/* Check for upper case characters */
{
    return (C >= 'A' && C <= 'Z');
}



int IsBDigit (char C)
/* Check for binary digits (0/1) */
{
    return (C == '0' || C == '1');
}



int IsODigit (char C)
/* Check for octal digits (0..7) */
{
    return (C >= '0' && C <= '7');
}



int IsXDigit (char C)
/* Check for hexadecimal digits */
{
    return (C >= 'a' && C <= 'f') || (C >= 'A' && C <= 'F') || (C >= '0' && C <= '9');
}



int IsQuote (char C)
/* Check for a single or double quote */
{
    return (C == '"' || C == '\'');
}
