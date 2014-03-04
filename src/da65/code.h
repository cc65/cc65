/*****************************************************************************/
/*                                                                           */
/*                                  code.h                                   */
/*                                                                           */
/*                          Binary code management                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
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



#ifndef CODE_H
#define CODE_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



extern unsigned char CodeBuf [0x10000];         /* Code buffer */
extern unsigned long CodeStart;                 /* Start address */
extern unsigned long CodeEnd;                   /* End address */
extern unsigned long PC;                        /* Current PC */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void LoadCode (void);
/* Load the code from the given file */

unsigned char GetCodeByte (unsigned Addr);
/* Get a byte from the given address */

unsigned GetCodeDByte (unsigned Addr);
/* Get a dbyte from the given address */

unsigned GetCodeWord (unsigned Addr);
/* Get a word from the given address */

unsigned long GetCodeDWord (unsigned Addr);
/* Get a dword from the given address */

unsigned GetRemainingBytes (void);
/* Return the number of remaining code bytes */

int CodeLeft (void);
/* Return true if there are code bytes left */

void ResetCode (void);
/* Reset the code input to start over for the next pass */



/* End of code.h */

#endif
