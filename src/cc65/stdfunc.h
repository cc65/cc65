/*****************************************************************************/
/*                                                                           */
/*                                 stdfunc.h                                 */
/*                                                                           */
/*         Handle inlining of known functions for the cc65 compiler          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
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



#ifndef STDFUNC_H
#define STDFUNC_H



/* cc65 */
#include "expr.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int FindStdFunc (const char* Name);
/* Determine if the given function is a known standard function that may be
** called in a special way. If so, return the index, otherwise return -1.
*/

void HandleStdFunc (int Index, struct FuncDesc* F, ExprDesc* lval);
/* Generate code for a known standard function. */



/* End of stdfunc.h */

#endif
