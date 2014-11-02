/*****************************************************************************/
/*                                                                           */
/*                                 istack.h                                  */
/*                                                                           */
/*                        Input stack for the scanner                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#ifndef ISTACK_H
#define ISTACK_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void PushInput (int (*Func) (void*), void* Data, const char* Desc);
/* Push an input function onto the input stack */

void PopInput (void);
/* Pop the current input function from the input stack */

int InputFromStack (void);
/* Try to get input from the input stack. Return true if we had such input,
** return false otherwise.
*/

int HavePushedInput (void);
/* Return true if we have stacked input available, return false if not */

void CheckInputStack (void);
/* Called from the scanner before closing an input file. Will check for any
** stuff on the input stack.
*/



/* End of istack.h */

#endif
