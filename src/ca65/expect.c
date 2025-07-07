/*****************************************************************************/
/*                                                                           */
/*                                 expect.c                                  */
/*                                                                           */
/*                      Print errors about expected tokens                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2025,      Kugelfuhr                                                  */
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



/* ca65 */
#include "error.h"
#include "expect.h"
#include "nexttok.h"
#include "scanner.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ErrorExpect (const char* Msg)
/* Output an error message about some expected token using Msg and the
 * description of the following token. This means that Msg should contain
 * something like "xyz expected". The actual error message would then be
 * "xyz expected but found zyx".
 */
{
    StrBuf S = AUTO_STRBUF_INITIALIZER;
    TokenDesc (&CurTok, &S);
    Error ("%s but found `%s'", Msg, SB_GetConstBuf (&S));
    SB_Done (&S);
}



int Expect (token_t Expected, const char* Msg)
/* Check if the next token is the expected one. If not, print Msg plus some
 * information about the token that was actually found. This means that Msg
 * should contain something like "xyz expected". The actual error message would
 * then be "xyz expected but found zyx".
 * Returns true if the token was found, otherwise false.
 */
{
    if (CurTok.Tok == Expected) {
        return 1;
    }

    ErrorExpect (Msg);
    return 0;
}



int ExpectSkip (token_t Expected, const char* Msg)
/* Check if the next token is the expected one. If not, print Msg plus some
 * information about the token that was actually found and skip the remainder
 * of the line. This means that Msg should contain something like "xyz
 * expected". The actual error message would then be "xyz expected but found
 * zyx".
 * Returns true if the token was found, otherwise false.
 */
{
    if (CurTok.Tok == Expected) {
        return 1;
    }

    ErrorExpect (Msg);
    SkipUntilSep ();
    return 0;
}



int ExpectSep (void)
/* Check if we've reached a line separator. If so, return true. If not, output
** an error and skip all tokens until the line separator is reached. Then
** return false.
*/
{
    if (!TokIsSep (CurTok.Tok)) {
        /* Try to be helpful by giving information about the token that was
         * unexpected.
         */
        ErrorExpect ("Expected `end-of-line'");
        SkipUntilSep ();
        return 0;
    } else {
        return 1;
    }
}
