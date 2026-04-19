/*****************************************************************************/
/*                                                                           */
/*                                 coptstop.h                                */
/*                                                                           */
/*           Optimize operations that take operands via the stack            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
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



#ifndef COPTSTOP_H
#define COPTSTOP_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptBZero (CodeSeg* S);
/* Optimize __bzero operations that take operands via the stack */



unsigned OptPtrStore4 (CodeSeg* S);
/* Optimize staspidx/staxspidx operations that take operands via the stack */



unsigned OptStkArith1 (CodeSeg* S);
/* Optimize arithmetic operations that take operands via the stack
** where X reg has the same value on left and right sides.
*/


unsigned OptStkArith2 (CodeSeg* S);
/* Optimize arithmetic operations (add/sub) that take operands via the stack */



unsigned OptStkBitwise1 (CodeSeg* S);
/* Optimize bitwise operations that take operands via the stack
** where X reg has the same value on left and right sides.
*/



unsigned OptStkBitwise2 (CodeSeg* S);
/* Optimize bitwise operations (and/or/xor) that take operands via the stack */



unsigned OptStkCmpOps1 (CodeSeg* S);
/* Optimize compare operators that take operands via the stack
** where X reg has the same value on left and right sides.
*/



unsigned OptStkCmpOps2 (CodeSeg* S);
/* Optimize compare operators that take operands via the stack */



unsigned OptStkEqOps1 (CodeSeg* S);
/* Optimize ==/!= operators that take operands via the stack
** where X reg has the same value on left and right sides.
*/



unsigned OptStkEqOps2 (CodeSeg* S);
/* Optimize ==/!= operators that take operands via the stack */



unsigned OptStkShifts (CodeSeg* S);
/* Optimize shift operations that take operands via the stack */



unsigned OptStkICmp1 (CodeSeg* S);
/* Optimize tosicmp operations where X reg has the same value on
** left and right sides.
*/



/* End of coptstop.h */

#endif
