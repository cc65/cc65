/*****************************************************************************/
/*                                                                           */
/*                                  test.h                                   */
/*                                                                           */
/*                        Test an expression and jump                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004      Ullrich von Bassewitz                                       */
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



#ifndef TESTEXPR_H
#define TESTEXPR_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define TESTEXPR_UNKNOWN        -1      /* Result of expression unknown */
#define TESTEXPR_TRUE           1       /* Expression yields true */
#define TESTEXPR_FALSE          0       /* Expression yields false */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned Test (unsigned Label, int Invert);
/* Evaluate a boolean test expression and jump depending on the result of
** the test and on Invert. The function returns one of the TESTEXPR_xx codes
** defined above. If the jump is always true, a warning is output.
*/

unsigned TestInParens (unsigned Label, int Invert);
/* Evaluate a boolean test expression in parenthesis and jump depending on
** the result of the test * and on Invert. The function returns one of the
** TESTEXPR_xx codes defined above. If the jump is always true, a warning is
** output.
*/



/* End of testexpr.h */

#endif
