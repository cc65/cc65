/*****************************************************************************/
/*                                                                           */
/*                                 coptind.h                                 */
/*                                                                           */
/*              Environment independent low level optimizations              */
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



#ifndef COPTIND_H
#define COPTIND_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptUnusedLoads (CodeSeg* S);
/* Remove loads of registers where the value loaded is not used later. */

unsigned OptUnusedStores (CodeSeg* S);
/* Remove stores into zero page registers that aren't used later */

unsigned OptLoad3 (CodeSeg* S);
/* Remove repeated loads from one and the same memory location */

unsigned OptDupLoads (CodeSeg* S);
/* Remove loads of registers where the value loaded is already in the register. */

unsigned OptStoreLoad (CodeSeg* S);
/* Remove a store followed by a load from the same location. */

unsigned OptLoadStoreLoad (CodeSeg* S);
/* Remove a load, store followed by a reload of the same location. */

unsigned OptTransfers1 (CodeSeg* S);
/* Remove transfers from one register to another and back */

unsigned OptTransfers2 (CodeSeg* S);
/* Replace loads followed by a register transfer by a load with the second
** register if possible.
*/

unsigned OptTransfers3 (CodeSeg* S);
/* Replace a register transfer followed by a store of the second register by a
** store of the first register if this is possible.
*/

unsigned OptTransfers4 (CodeSeg* S);
/* Replace a load of a register followed by a transfer insn of the same register
** by a load of the second register if possible.
*/

unsigned OptPushPop1 (CodeSeg* S);
/* Remove a PHA/PLA sequence were A not used later */

unsigned OptPushPop2 (CodeSeg* S);
/* Remove a PHP/PLP sequence were no processor flags changed inside */

unsigned OptPrecalc (CodeSeg* S);
/* Replace immediate operations with the accu where the current contents are
** known by a load of the final value.
*/

unsigned OptShiftBack (CodeSeg* S);
/* Remove a pair of shifts to the opposite directions if none of the bits of
** the register A or the Z/N flags modified by these shifts are used later.
*/

unsigned OptSignExtended (CodeSeg* S);
/* Change
**
**      lda     xxx     ; X is 0
**      bpl     L1
**      dex/ldx #$FF
**  L1: cpx     #$00
**      bpl     L2
**
** or
**
**      lda     xxx     ; X is 0
**      bpl     L1
**      dex/ldx #$FF
**  L1: cpx     #$80
**      bcc/bmi L2
**
** into
**      lda     xxx     ; X is 0
**      bpl     L2
**      dex/ldx #$FF
**
** provided the C flag isn't used later.
*/



/* End of coptind.h */

#endif
