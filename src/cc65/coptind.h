/*****************************************************************************/
/*                                                                           */
/*				   coptind.h				     */
/*                                                                           */
/*		Environment independent low level optimizations		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



#ifndef COPTIND_H
#define COPTIND_H



#include "codeseg.h"



/*****************************************************************************/
/*		    		     Code				     */
/*****************************************************************************/



unsigned OptDeadJumps (CodeSeg* S);
/* Remove dead jumps (jumps to the next instruction) */

unsigned OptDeadCode (CodeSeg* S);
/* Remove dead code (code that follows an unconditional jump or an rts/rti
 * and has no label)
 */

unsigned OptJumpCascades (CodeSeg* S);
/* Optimize jump cascades (jumps to jumps). In such a case, the jump is
 * replaced by a jump to the final location. This will in some cases produce
 * worse code, because some jump targets are no longer reachable by short
 * branches, but this is quite rare, so there are more advantages than
 * disadvantages.
 */

unsigned OptRTS (CodeSeg* S);
/* Optimize subroutine calls followed by an RTS. The subroutine call will get
 * replaced by a jump. Don't bother to delete the RTS if it does not have a
 * label, the dead code elimination should take care of it.
 */

unsigned OptJumpTarget (CodeSeg* S);
/* If the instruction preceeding an unconditional branch is the same as the
 * instruction preceeding the jump target, the jump target may be moved
 * one entry back. This is a size optimization, since the instruction before
 * the branch gets removed.
 */

unsigned OptCondBranches (CodeSeg* S);
/* If an immidiate load of a register is followed by a conditional jump that
 * is never taken because the load of the register sets the flags in such a
 * manner, remove the conditional branch.
 */

unsigned OptUnusedLoads (CodeSeg* S);
/* Remove loads of registers where the value loaded is not used later. */



/* End of coptind.h */
#endif



