/*****************************************************************************/
/*                                                                           */
/*                                copttest.h                                 */
/*                                                                           */
/*                          Optimize test sequences                          */
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



#ifndef COPTTEST_H
#define COPTTEST_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                              Optimize tests                               */
/*****************************************************************************/



unsigned OptTest1 (CodeSeg* S);
/* Given a sequence
**
**     stx     xxx
**     ora     xxx
**     beq/bne ...
**
** If X is zero, the sequence may be changed to
**
**     cmp     #$00
**     beq/bne ...
**
** which may be optimized further by another step.
**
** If A is zero, the sequence may be changed to
**
**     txa
**     beq/bne ...
*/

unsigned OptTest2 (CodeSeg* S);
/* Search for an inc/dec operation followed by a load and a conditional
** branch based on the flags from the load. Remove the load if the insn
** isn't used later.
*/



/* End of copttest.h */

#endif
