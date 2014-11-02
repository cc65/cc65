/*****************************************************************************/
/*                                                                           */
/*                                 condasm.h                                 */
/*                                                                           */
/*                   Conditional assembly support for ca65                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#ifndef CONDASM_H
#define CONDASM_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The overall .IF condition */
extern int IfCond;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void DoConditionals (void);
/* Catch all for conditional directives */

int CheckConditionals (void);
/* Check if the current token is one that starts a conditional directive, and
** call DoConditionals if so. Return true if a conditional directive was found,
** return false otherwise.
*/

void CheckOpenIfs (void);
/* Called from the scanner before closing an input file. Will check for any
** open .ifs in this file.
*/

unsigned GetIfStack (void);
/* Get the current .IF stack pointer */

void CleanupIfStack (unsigned SP);
/* Cleanup the .IF stack, remove anything above the given stack pointer */



/* End of condasm.h */

#endif
