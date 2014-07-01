/*****************************************************************************/
/*                                                                           */
/*                                alignment.h                                */
/*                                                                           */
/*                             Address aligment                              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                70794 Filderstadt                                          */
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



#ifndef ALIGNMENT_H
#define ALIGNMENT_H



/* common */
#include "inline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Maximum possible alignment. Beware: To increase the possible alignment it
** is not enough to bump this value. Check the code inside.
*/
#define MAX_ALIGNMENT   0x10000UL

/* The following value marks what is considered a "large alignment" and worth
** a warning if not suppressed.
*/
#define LARGE_ALIGNMENT 0x101UL



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned long LeastCommonMultiple (unsigned long Left, unsigned long Right);
/* Calculate the least common multiple of two numbers and return
** the result.
*/

unsigned long AlignAddr (unsigned long Addr, unsigned long Alignment);
/* Align an address to the given alignment */

unsigned long AlignCount (unsigned long Addr, unsigned long Alignment);
/* Calculate how many bytes must be inserted to align Addr to Alignment */



/* End of alignment.h */

#endif
