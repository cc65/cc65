/*****************************************************************************/
/*                                                                           */
/*                                 ulabel.h                                  */
/*                                                                           */
/*                Unnamed labels for the ca65 macroassembler                 */
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



#ifndef ULABEL_H
#define ULABEL_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ExprNode* ULabRef (int Which);
/* Get an unnamed label. If Which is negative, it is a backreference (a
** reference to an already defined label), and the function will return a
** segment relative expression. If Which is positive, it is a forward ref,
** and the function will return a expression node for an unnamed label that
** must be resolved later.
*/

void ULabDef (void);
/* Define an unnamed label at the current PC */

int ULabCanResolve (void);
/* Return true if we can resolve arbitrary ULabels. */

ExprNode* ULabResolve (unsigned Index);
/* Return a valid expression for the unnamed label with the given index. This
** is used to resolve unnamed labels when assembly is done, so it is an error
** if a label is still undefined in this phase.
*/

void ULabDone (void);
/* Run through all unnamed labels, check for anomalies and errors and do
** necessary cleanups.
*/



/* End of ulabel.h */

#endif
