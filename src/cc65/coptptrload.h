/*****************************************************************************/
/*                                                                           */
/*                               coptptrload.h                               */
/*                                                                           */
/*                      Optimize loads through pointers                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009 Ullrich von Bassewitz                                       */
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



#ifndef COPTPTRLOAD_H
#define COPTPTRLOAD_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptPtrLoad1 (CodeSeg* S);
/* Search for the sequence:
**
**      clc
**      adc     xxx
**      tay
**      txa
**      adc     yyy
**      tax
**      tya
**      ldy
**      jsr     ldauidx
**
** and replace it by:
**
**      clc
**      adc     xxx
**      sta     ptr1
**      txa
**      adc     yyy
**      sta     ptr1+1
**      ldy
**      ldx     #$00
**      lda     (ptr1),y
*/

unsigned OptPtrLoad2 (CodeSeg* S);
/* Search for the sequence:
**
**      adc     xxx
**      pha
**      txa
**      iny
**      adc     yyy
**      tax
**      pla
**      ldy
**      jsr     ldauidx
**
** and replace it by:
**
**      adc     xxx
**      sta     ptr1
**      txa
**      iny
**      adc     yyy
**      sta     ptr1+1
**      ldy
**      ldx     #$00
**      lda     (ptr1),y
*/

unsigned OptPtrLoad3 (CodeSeg* S);
/* Search for the sequence:
**
**      lda     #<(label+0)
**      ldx     #>(label+0)
**      clc
**      adc     xxx
**      bcc     L
**      inx
** L:   ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     xxx
**      ldx     #$00
**      lda     label,y
*/

unsigned OptPtrLoad4 (CodeSeg* S);
/* Search for the sequence:
**
**      lda     #<(label+0)
**      ldx     #>(label+0)
**      ldy     #$xx
**      clc
**      adc     (sp),y
**      bcc     L
**      inx
** L:   ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     #$xx
**      lda     (sp),y
**      tay
**      ldx     #$00
**      lda     label,y
*/

unsigned OptPtrLoad5 (CodeSeg* S);
/* Search for the sequence:
**
**      jsr     pushax
**      ldx     #$00
**      lda     yyy
**      jsr     tosaddax
**      ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     yyy
**      lda     (ptr1),y
*/

unsigned OptPtrLoad6 (CodeSeg* S);
/* Search for the sequence:
**
**      jsr     pushax
**      ldy     xxx
**      ldx     #$00
**      lda     (sp),y
**      jsr     tosaddax
**      ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     xxx
**      lda     (sp),y
**      tay
**      lda     (ptr1),y
*/

unsigned OptPtrLoad7 (CodeSeg* S);
/* Search for the sequence:
**
**      jsr     aslax1/shlax1
**      clc
**      adc     xxx
**      tay
**      txa
**      adc     yyy
**      tax
**      tya
**      ldy     zzz
**      jsr     ldaxidx
**
** and replace it by:
**
**      stx     tmp1
**      asl     a
**      rol     tmp1
**      clc
**      adc     xxx
**      sta     ptr1
**      lda     tmp1
**      adc     yyy
**      sta     ptr1+1
**      ldy     zzz
**      lda     (ptr1),y
**      tax
**      dey
**      lda     (ptr1),y
*/

unsigned OptPtrLoad11 (CodeSeg* S);
/* Search for the sequence:
**
**      clc
**      adc     xxx
**      bcc     L
**      inx
** L:   ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     xxx
**      sta     ptr1
**      stx     ptr1+1
**      ldx     #$00
**      lda     (ptr1),y
*/

unsigned OptPtrLoad12 (CodeSeg* S);
/* Search for the sequence:       
**
**      lda     regbank+n
**      ldx     regbank+n+1
**      sta     regsave
**      stx     regsave+1
**      clc
**      adc     #$01
**      bcc     L0005
**      inx
** L:   sta     regbank+n
**      stx     regbank+n+1
**      lda     regsave
**      ldx     regsave+1
**      ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     #$00
**      ldx     #$00
**      lda     (regbank+n),y
**      inc     regbank+n
**      bne     L1
**      inc     regbank+n+1
** L1:  tay                     <- only if flags are used
**
** This function must execute before OptPtrLoad7!
**
*/

unsigned OptPtrLoad13 (CodeSeg* S);
/* Search for the sequence:
**
**      lda     zp
**      ldx     zp+1
**      ldy     xx
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     xx
**      ldx     #$00
**      lda     (zp),y
*/

unsigned OptPtrLoad14 (CodeSeg* S);
/* Search for the sequence:
**
**      lda     zp
**      ldx     zp+1
**      (anything that doesn't change a/x)
**      ldy     xx
**      jsr     ldauidx
**
** and replace it by:
**
**      lda     zp
**      ldx     zp+1
**      (anything that doesn't change a/x)
**      ldy     xx
**      ldx     #$00
**      lda     (zp),y
**
** Must execute before OptPtrLoad10!
*/

unsigned OptPtrLoad15 (CodeSeg* S);
/* Search for the sequence:
**
**      lda     zp
**      ldx     zp+1
**      ldy     xx
**      jsr     ldaxidx
**
** and replace it by:
**
**      ldy     xx
**      lda     (zp),y
**      tax
**      dey
**      lda     (zp),y
*/

unsigned OptPtrLoad16 (CodeSeg* S);
/* Search for the sequence
**
**      ldy     ...
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     ...
**      stx     ptr1+1
**      sta     ptr1
**      ldx     #$00
**      lda     (ptr1),y
**
** This step must be executed *after* OptPtrLoad1!
*/

unsigned OptPtrLoad17 (CodeSeg* S);
/* Search for the sequence
**
**      ldy     ...
**      jsr     ldaxidx
**
** and replace it by:
**
**      ldy     ...
**      sta     ptr1
**      stx     ptr1+1
**      lda     (ptr1),y
**      tax
**      dey
**      lda     (ptr1),y
**
** This step must be executed *after* OptPtrLoad9! While code size increases
** by more than 200%, inlining will greatly improve visibility for the
** optimizer, so often part of the code gets improved later. So we will mark
** the step with less than 200% so it gets executed when -Oi is in effect.
*/



/* End of coptptrload.h */

#endif
