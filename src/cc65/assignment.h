/*****************************************************************************/
/*                                                                           */
/*                               assignment.h                                */
/*                                                                           */
/*                             Parse assignments                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2004 Ullrich von Bassewitz                                       */
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



#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H



/* cc65 */
#include "exprdesc.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Whether to save/restore the original lhs or result value */
enum {
    OA_NEED_NONE,
    OA_NEED_OLD,
    OA_NEED_NEW,
};

/* Forward */
struct GenDesc;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void DoIncDecBitField (ExprDesc* Expr, long Val, unsigned KeepResult);
/* Process inc/dec for bit-field */

void OpAssign (const struct GenDesc* Gen, ExprDesc* lval, const char* Op);
/* Parse an "=" (if 'Gen' is 0) or "op=" operation */

void OpAddSubAssign (const struct GenDesc* Gen, ExprDesc *Expr, const char* Op);
/* Parse a "+=" or "-=" operation */



/* End of assignment.h */

#endif
