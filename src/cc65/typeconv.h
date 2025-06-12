/*****************************************************************************/
/*                                                                           */
/*                                typeconv.h                                 */
/*                                                                           */
/*                          Handle type conversions                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2008 Ullrich von Bassewitz                                       */
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



#ifndef TYPECONV_H
#define TYPECONV_H



/* cc65 */
#include "exprdesc.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void TypeConversion (ExprDesc* Expr, const Type* NewType);
/* Do an automatic conversion of the given expression to the new type. Output
** warnings or errors where this automatic conversion is suspicious or
** impossible.
*/

void TypeCast (ExprDesc* Expr);
/* Handle an explicit cast. */

void TypeComposition (Type* lhs, const Type* rhs);
/* Recursively compose two types into lhs. The two types must have compatible
** type or this fails with a critical check.
*/



/* End of typeconv.h */

#endif
