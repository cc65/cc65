/*****************************************************************************/
/*                                                                           */
/*                                 coptjmp.h                                 */
/*                                                                           */
/*           Low level optimizations regarding branches and jumps            */
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



#ifndef COPTJMP_H
#define COPTJMP_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptBranchDist (CodeSeg* S);
/* Change branches for the distance needed. */

unsigned OptBranchDist2 (CodeSeg* S);
/* If BRA points to an external symbol, change it to JMP */

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

unsigned OptCondBranch1 (CodeSeg* S);
/* Performs some optimization steps:
**  - If an immediate load of a register is followed by a conditional jump that
**    is never taken because the load of the register sets the flags in such a
**    manner, remove the conditional branch.
**  - If the conditional branch is always taken because of the register load,
**    replace it by a jmp.
*/

unsigned OptCondBranch2 (CodeSeg* S);
/* If a conditional branch jumps around an unconditional branch, remove the
** conditional branch and make the jump a conditional branch with the inverse
** condition of the first one.
*/

unsigned OptCondBranch3 (CodeSeg* S);
/* If the conditional branch is always taken because it follows an inverse
** conditional branch, replace it by a jmp.
*/

unsigned OptCondBranchC (CodeSeg* S);
/* If on entry to a "rol a" instruction the accu is zero, and a beq/bne follows,
** we can remove the rol and branch on the state of the carry.
*/



/* End of coptjmp.h */

#endif
