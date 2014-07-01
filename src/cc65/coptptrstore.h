/*****************************************************************************/
/*                                                                           */
/*                               coptptrstore.h                              */
/*                                                                           */
/*                      Optimize stores through pointers                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



#ifndef COPTPTRSTORE_H
#define COPTPTRSTORE_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptPtrStore1 (CodeSeg* S);
/* Search for the sequence:
**
**      clc
**      adc     xxx
**      bcc     L
**      inx
** L:   jsr     pushax
**      ldx     #$00
**      lda     yyy
**      ldy     #$00
**      jsr     staspidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     xxx
**      ldx     #$00
**      lda     yyy
**      sta     (ptr1),y
**
** or by
**
**      ldy     xxx
**      ldx     #$00
**      lda     yyy
**      sta     (zp),y
**
** or by
**
**      ldy     xxx
**      ldx     #$00
**      lda     yyy
**      sta     label,y
**
** or by
**
**      ldy     xxx
**      ldx     #$00
**      lda     yyy
**      sta     $xxxx,y
**
** depending on the two instructions preceeding the sequence above.
*/

unsigned OptPtrStore2 (CodeSeg* S);
/* Search for the sequence:
**
**      clc
**      adc     xxx
**      bcc     L
**      inx
** L:   jsr     pushax
**      ldy     yyy
**      ldx     #$00
**      lda     (sp),y
**      ldy     #$00
**      jsr     staspidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     yyy-2
**      ldx     #$00
**      lda     (sp),y
**      ldy     xxx
**      sta     (ptr1),y
**
** or by
**
**      ldy     yyy-2
**      ldx     #$00
**      lda     (sp),y
**      ldy     xxx
**      sta     (zp),y
**
** or by
**
**      ldy     yyy-2
**      ldx     #$00
**      lda     (sp),y
**      ldy     xxx
**      sta     label,y
**
** or by
**
**      ldy     yyy-2
**      ldx     #$00
**      lda     (sp),y
**      ldy     xxx
**      sta     $xxxx,y
**
** depending on the code preceeding the sequence above.
*/

unsigned OptPtrStore3 (CodeSeg* S);
/* Search for the sequence:
**
**      jsr     pushax
**      ldy     xxx
**      jsr     ldauidx
**      subop
**      ldy     yyy
**      jsr     staspidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     xxx
**      ldx     #$00
**      lda     (ptr1),y
**      subop
**      ldy     yyy
**      sta     (ptr1),y
**
** In case a/x is loaded from the register bank before the pushax, we can even
** use the register bank instead of ptr1.
*/



/* End of coptptrstore.h */

#endif
