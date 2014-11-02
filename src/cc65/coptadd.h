/*****************************************************************************/
/*                                                                           */
/*                                 coptadd.h                                 */
/*                                                                           */
/*                        Optimize addition sequences                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2005, Ullrich von Bassewitz                                      */
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



#ifndef COPTADD_H
#define COPTADD_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                            Optimize additions                             */
/*****************************************************************************/



unsigned OptAdd1 (CodeSeg* S);
/* Search for the sequence
**
**      jsr     pushax
**      ldy     xxx
**      ldx     #$00
**      lda     (sp),y
**      jsr     tosaddax
**
** and replace it by:
**
**      ldy     xxx-2
**      clc
**      adc     (sp),y
**      bcc     L
**      inx
** L:
*/

unsigned OptAdd2 (CodeSeg* S);
/* Search for the sequence
**
**      ldy     #xx
**      lda     (sp),y
**      tax
**      dey
**      lda     (sp),y
**      ldy     #$yy
**      jsr     addeqysp
**
** and replace it by:
**
**      ldy     #xx-1
**      lda     (sp),y
**      ldy     #yy
**      clc
**      adc     (sp),y
**      sta     (sp),y
**      ldy     #xx
**      lda     (sp),y
**      ldy     #yy+1
**      adc     (sp),y
**      sta     (sp),y
**
** provided that a/x is not used later.
*/

unsigned OptAdd3 (CodeSeg* S);
/* Search for the sequence
**
**      jsr     pushax
**      ldx     #$00
**      lda     xxx
**      jsr     tosaddax
**
** and replace it by
**
**      clc
**      adc     xxx
**      bcc     L1
**      inx
** L1:
*/

unsigned OptAdd4 (CodeSeg* S);
/* Search for the sequence
**
**      jsr     pushax
**      lda     xxx
**      ldx     yyy
**      jsr     tosaddax
**
** and replace it by
**
**      clc
**      adc     xxx
**      pha
**      txa
**      adc     yyy
**      tax
**      pla
*/

unsigned OptAdd5 (CodeSeg* S);
/* Search for a call to incaxn and replace it by an 8 bit add if the X register
** is not used later.
*/

unsigned OptAdd6 (CodeSeg* S);
/* Search for the sequence
**
**      adc     ...
**      bcc     L
**      inx
** L:
**
** and remove the handling of the high byte if X is not used later.
*/



/* End of coptadd.h */

#endif
