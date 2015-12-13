/*****************************************************************************/
/*                                                                           */
/*                                coptshift.h                                */
/*                                                                           */
/*                              Optimize shifts                              */
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



#ifndef COPTSHIFT_H
#define COPTSHIFT_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                              Optimize shifts                              */
/*****************************************************************************/



unsigned OptShift1 (CodeSeg* S);
/* A call to the shlaxN routine may get replaced by one or more asl insns
** if the value of X is not used later. If X is used later, but it is zero
** on entry and it's a shift by one, it may get replaced by:
**
**      asl     a
**      bcc     L1
**      inx
**  L1:
*/

unsigned OptShift2 (CodeSeg* S);
/* The sequence
**
**      bpl     L
**      dex
** L:   jsr     asraxN
**
** might be replaced by N copies of
**
**      cmp     #$80
**      ror     a
**
** if X is not used later (X is assumed to be zero on entry).
*/

unsigned OptShift3 (CodeSeg* S);
/* The sequence
**
**      bcc     L
**      inx
** L:   jsr     shrax1
**
** may get replaced by
**
**      ror     a
**
** if X is zero on entry and unused later. For shift counts > 1, more
**
**      shr     a
**
** must be added.
*/

unsigned OptShift4 (CodeSeg* S);
/* Calls to the asraxN or shraxN routines may get replaced by one or more lsr
** insns if the value of X is zero.
*/

unsigned OptShift5 (CodeSeg* S);
/* Search for the sequence
**
**      lda     xxx
**      ldx     yyy
**      jsr     aslax1/asrax1/shlax1/shrax1
**      sta     aaa
**      stx     bbb
**
** and replace it by
**
**      lda     xxx
**      asl     a
**      sta     aaa
**      lda     yyy
**      rol     a
**      sta     bbb
**
** or similar, provided that a/x is not used later
*/

unsigned OptShift6 (CodeSeg* S);
/* Inline the shift subroutines. */



/* End of coptshift.h */

#endif
