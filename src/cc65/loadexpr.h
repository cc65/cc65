/*****************************************************************************/
/*                                                                           */
/*                                loadexpr.h                                 */
/*                                                                           */
/*               Load an expression into the primary register                */
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



#ifndef LOADEXPR_H
#define LOADEXPR_H



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct ExprDesc;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void LoadExpr (unsigned Flags, struct ExprDesc* Expr);
/* Load an expression into the primary register if it is not already there.
** If Flags contains any CF_TYPEMASK bits, it then overrides the codegen type
** info that would be otherwise taken from the expression type.
*/



/* End of loadexpr.h */

#endif
