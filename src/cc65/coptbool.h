/*****************************************************************************/
/*                                                                           */
/*                                coptbool.h                                 */
/*                                                                           */
/*                        Optimize boolean sequences                         */
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



#ifndef COPTBOOL_H
#define COPTBOOL_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*           Optimize bool comparison and transformer subroutines            */
/*****************************************************************************/



unsigned OptBoolCmp (CodeSeg* S);
/* Search for calls to compare subroutines followed by a conditional branch
** and replace them by cheaper versions, since the branch means that the
** boolean value returned by these routines is not needed (we may also check
** that explicitly, but for the current code generator it is always true).
*/

unsigned OptBoolTrans (CodeSeg* S);
/* Try to remove the call to boolean transformer routines where the call is
** not really needed and change following branch condition accordingly.
*/



/*****************************************************************************/
/*           Remove calls to the boolean cast/negation subroutines           */
/*****************************************************************************/



unsigned OptBoolUnary1 (CodeSeg* S);
/* Search for and remove bcastax adjacent to bnegax */

unsigned OptBoolUnary2 (CodeSeg* S);
/* Search for and remove bcastax/bnegax following a boolean transformer.
** Invert the boolean transformer if it is bnegax to be removed.
*/

unsigned OptBoolUnary3 (CodeSeg* S);
/* Replace bcastax/bnegax with
**
**      cpx #0
**      jsr boolne/booleq
**
** if A == 0, or replace bcastax/bnegax with
**
**      cmp #0
**      jsr boolne/booleq
**
** if X == 0.
*/



/*****************************************************************************/
/*                            bnega optimizations                            */
/*****************************************************************************/



unsigned OptBNegA1 (CodeSeg* S);
/* Check for
**
**      ldx     #$00
**      lda     ..
**      jsr     bnega
**
** Remove the ldx if the lda does not use it.
*/

unsigned OptBNegA2 (CodeSeg* S);
/* Check for
**
**      lda     ..
**      jsr     bnega
**      jeq/jne ..
**
** Adjust the conditional branch and remove the call to the subroutine.
*/



/*****************************************************************************/
/*                            bnegax optimizations                           */
/*****************************************************************************/



unsigned OptBNegAX1 (CodeSeg* S);
/* On a call to bnegax, if X is zero, the result depends only on the value in
** A, so change the call to a call to bnega. This will get further optimized
** later if possible.
*/

unsigned OptBNegAX2 (CodeSeg* S);
/* Search for the sequence:
**
**      lda     (xx),y
**      tax
**      dey
**      lda     (xx),y
**      jsr     bnegax
**      jne/jeq ...
**
** and replace it by
**
**      lda     (xx),y
**      dey
**      ora     (xx),y
**      jeq/jne ...
*/

unsigned OptBNegAX3 (CodeSeg* S);
/* Search for the sequence:
**
**      lda     xx
**      ldx     yy
**      jsr     bnegax
**      jne/jeq ...
**
** and replace it by
**
**      lda     xx
**      ora     xx+1
**      jeq/jne ...
*/

unsigned OptBNegAX4 (CodeSeg* S);
/* Search for the sequence:
**
**      jsr     xxx
**      jsr     bnega(x)
**      jeq/jne ...
**
** and replace it by:
**
**      jsr     xxx
**      <boolean test>
**      jne/jeq ...
*/



/* End of coptbool.h */

#endif
