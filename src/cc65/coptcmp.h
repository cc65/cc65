/*****************************************************************************/
/*                                                                           */
/*                                 coptcmp.h                                 */
/*                                                                           */
/*                             Optimize compares                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



#ifndef COPTCMP_H
#define COPTCMP_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                        Optimizations for compares                         */
/*****************************************************************************/



unsigned OptCmp1 (CodeSeg* S);
/* Search for the sequence
**
**      ldx     xx
**      stx     tmp1
**      ora     tmp1
**
** and replace it by
**
**      ora     xx
*/

unsigned OptCmp2 (CodeSeg* S);
/* Search for the sequence
**
**      stx     xx
**      stx     tmp1
**      ora     tmp1
**
** and replace it by
**
**      stx     xx
**      ora     xx
*/

unsigned OptCmp3 (CodeSeg* S);
/* Search for
**
**      lda/and/ora/eor ...
**      cmp #$00
**      jeq/jne
** or
**      lda/and/ora/eor ...
**      cmp #$00
**      jsr boolxx
**
** and remove the cmp.
*/

unsigned OptCmp4 (CodeSeg* S);
/* Search for
**
**      lda     x
**      ldx     y
**      cpx     #a
**      bne     L1
**      cmp     #b
**      jne/jeq L2
**
** If a is zero, we may remove the compare. If a and b are both zero, we may
** replace it by the sequence
**
**      lda     x
**      ora     x+1
**      jne/jeq ...
**
** L1 may be either the label at the branch instruction, or the target label
** of this instruction.
*/

unsigned OptCmp5 (CodeSeg* S);
/* Optimize compares of local variables:
**
**      ldy     #o
**      lda     (sp),y
**      tax
**      dey
**      lda     (sp),y
**      cpx     #a
**      bne     L1
**      cmp     #b
**      jne/jeq L2
*/

unsigned OptCmp7 (CodeSeg* S);
/* Search for a sequence ldx/txa/branch and remove the txa if A is not
** used later.
*/

unsigned OptCmp8 (CodeSeg* S);
/* Check for register compares where the contents of the register and therefore
** the result of the compare is known.
*/

unsigned OptCmp9 (CodeSeg* S);
/* Search for the sequence
**
**    sbc       xx
**    bvs/bvc   L
**    eor       #$80
** L: asl       a
**    bcc/bcs   somewhere
**
** If A is not used later (which should be the case), we can branch on the N
** flag instead of the carry flag and remove the asl.
*/



/* End of coptcmp.h */

#endif
