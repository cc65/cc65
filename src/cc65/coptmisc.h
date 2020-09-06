/*****************************************************************************/
/*                                                                           */
/*                                codemisc.h                                 */
/*                                                                           */
/*                   Miscellaneous optimization operations                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2012, Ullrich von Bassewitz                                      */
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



#ifndef COPTMISC_H
#define COPTMISC_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptDecouple (CodeSeg* S);
/* Decouple operations, that is, do the following replacements:
**
**   dex        -> ldx #imm
**   inx        -> ldx #imm
**   dey        -> ldy #imm
**   iny        -> ldy #imm
**   tax        -> ldx #imm
**   txa        -> lda #imm
**   tay        -> ldy #imm
**   tya        -> lda #imm
**   lda zp     -> lda #imm
**   ldx zp     -> ldx #imm
**   ldy zp     -> ldy #imm
**
** Provided that the register values are known of course.
*/

unsigned OptIndLoads1 (CodeSeg* S);
/* Change
**
**     lda      (zp),y
**
** into
**
**     lda      (zp,x)
**
** provided that x and y are both zero.
*/

unsigned OptIndLoads2 (CodeSeg* S);
/* Change
**
**     lda      (zp,x)
**
** into
**
**     lda      (zp),y
**
** provided that x and y are both zero.
*/

unsigned OptStackPtrOps (CodeSeg* S);
/* Merge adjacent calls to decsp into one. NOTE: This function won't merge all
** known cases!
*/

unsigned OptGotoSPAdj (CodeSeg* S);
/* Optimize SP adjustment for forward 'goto' */

unsigned OptLoad1 (CodeSeg* S);
/* Search for a call to ldaxysp where X is not used later and replace it by
** a load of just the A register.
*/

unsigned OptLoad2 (CodeSeg* S);
/* Replace calls to ldaxysp by inline code */


/* End of coptmisc.h */

#endif
