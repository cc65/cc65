/*****************************************************************************/
/*                                                                           */
/*                                   hexval.c                                */
/*                                                                           */
/*                     Convert hex digits to numeric values                  */
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

/* cc65 */
#include "error.h"
#include "hexval.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned HexVal (int C)
/* Convert a hex digit into a value. The function will emit an error for
** invalid hex digits.
*/
{
    if (!IsXDigit (C)) {
        Error ("Invalid hexadecimal digit: `%c'", C);
    }
    if (IsDigit (C)) {
        return C - '0';
    } else {
        return toupper (C) - 'A' + 10;
    }
}
