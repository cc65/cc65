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



unsigned OptRTSJumps1 (CodeSeg* S);
/* Replace jumps to RTS by RTS */

unsigned OptRTSJumps2 (CodeSeg* S);
/* Replace long conditional jumps to RTS */

unsigned OptDeadJumps (CodeSeg* S);
/* Remove dead jumps (jumps to the next instruction) */

unsigned OptDeadCode (CodeSeg* S);
/* Remove dead code (code that follows an unconditional jump or an rts/rti
** and has no label)
*/

unsigned OptJumpCascades (CodeSeg* S);
/* Optimize jump cascades (jumps to jumps). In such a case, the jump is
** replaced by a jump to the final location. This will in some cases produce
** worse code, because some jump targets are no longer reachable by short
** branches, but this is quite rare, so there are more advantages than
** disadvantages.
*/

unsigned OptRTS (CodeSeg* S);
/* Optimize subroutine calls followed by an RTS. The subroutine call will get
** replaced by a jump. Don't bother to delete the RTS if it does not have a
** label, the dead code elimination should take care of it.
*/

unsigned OptJumpTarget1 (CodeSeg* S);
/* If the instruction preceeding an unconditional branch is the same as the
** instruction preceeding the jump target, the jump target may be moved
** one entry back. This is a size optimization, since the instruction before
** the branch gets removed.
*/

unsigned OptJumpTarget2 (CodeSeg* S);
/* If a bcs jumps to a sec insn or a bcc jumps to clc, skip this insn, since
** it's job is already done.
*/

unsigned OptJumpTarget3 (CodeSeg* S);
/* Jumps to load instructions of a register, that do already have the matching
** register contents may skip the load instruction, since it's job is already
** done.
*/

unsigned OptCondBranches1 (CodeSeg* S);
/* If an immidiate load of a register is followed by a conditional jump that
** is never taken because the load of the register sets the flags in such a
** manner, remove the conditional branch.
*/

unsigned OptCondBranches2 (CodeSeg* S);
/* If on entry to a "rol a" instruction the accu is zero, and a beq/bne follows,
** we can remove the rol and branch on the state of the carry.
*/

unsigned OptUnusedLoads (CodeSeg* S);
/* Remove loads of registers where the value loaded is not used later. */

unsigned OptUnusedStores (CodeSeg* S);
/* Remove stores into zero page registers that aren't used later */

unsigned OptDupLoads (CodeSeg* S);
/* Remove loads of registers where the value loaded is already in the register. */

unsigned OptStoreLoad (CodeSeg* S);
/* Remove a store followed by a load from the same location. */

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

unsigned OptPushPop (CodeSeg* S);
/* Remove a PHA/PLA sequence were A is not used later */

unsigned OptPrecalc (CodeSeg* S);
/* Replace immediate operations with the accu where the current contents are
** known by a load of the final value.
*/

unsigned OptBranchDist (CodeSeg* S);
/* Change branches for the distance needed. */

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



/* End of coptind.h */

#endif
