/*****************************************************************************/
/*                                                                           */
/*                                  coptstore.h                              */
/*                                                                           */
/*                                Optimize stores                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2006, Ullrich von Bassewitz                                      */
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



#ifndef COPTSTORE_H
#define COPTSTORE_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptStore1 (CodeSeg* S);
/* Search for the sequence
**
**      ldy     #n
**      jsr     staxysp
**      ldy     #n+1
**      jsr     ldaxysp
**
** and remove the useless load, provided that the next insn doesn't use flags
** from the load.
*/

unsigned OptStore2 (CodeSeg* S);
/* Search for a call to staxysp. If the ax register is not used later, and
** the value is constant, just use the A register and store directly into the
** stack.
*/

unsigned OptStore3 (CodeSeg* S);
/* Search for a call to steaxysp. If the eax register is not used later, and
** the value is constant, just use the A register and store directly into the
** stack.
*/

unsigned OptStore4 (CodeSeg* S);
/* Search for the sequence
**
**      sta     xx
**      stx     yy
**      lda     xx
**      ldx     yy
**
** and remove the useless load, provided that the next insn doesn't use flags
** from the load.
*/

unsigned OptStore5 (CodeSeg* S);
/* Search for the sequence
**
**      lda     foo
**      ldx     bar
**      sta     something
**      stx     something-else
**
** and replace it by
**
**      lda     foo
**      sta     something
**      lda     bar
**      sta     something-else
**
** if X is not used later. This replacement doesn't save any cycles or bytes,
** but it keeps the value of X, which may be reused later.
*/



/* End of coptstore.h */

#endif
