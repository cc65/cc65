/*****************************************************************************/
/*                                                                           */
/*				    code.h				     */
/*                                                                           */
/*			    Binary code management			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef CODE_H
#define CODE_H



/*****************************************************************************/
/*	    			     Code				     */
/*****************************************************************************/



void LoadCode (const char* Name, unsigned long StartAddress);
/* Load the code from the given file */

unsigned GetPC (void);
/* Get the current program counter */

unsigned char PeekCodeByte (void);
/* Peek at the byte at the current PC */

unsigned char GetCodeByte (void);
/* Get a byte from the PC and increment it */

unsigned GetCodeWord (void);
/* Get a word from the current PC and increment it */

unsigned GetRemainingBytes (void);
/* Return the number of remaining code bytes */

void ResetCode (void);
/* Reset the code input to start over for the next pass */



/* End of code.h */
#endif



