/*****************************************************************************/
/*                                                                           */
/*                                tgttrans.h                                 */
/*                                                                           */
/*                         Character set translation                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2012, Ullrich von Bassewitz                                      */
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



#ifndef TGTTRANS_H
#define TGTTRANS_H



/* common */
#include "strbuf.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void TgtTranslateInit (void);
/* Initialize the translation tables */

int TgtTranslateChar (int C);
/* Translate one character from the source character set into the target
** system character set.
*/

void TgtTranslateBuf (void* Buf, unsigned Len);
/* Translate a buffer of the given length from the source character set into
** the target system character set.
*/

void TgtTranslateStrBuf (StrBuf* Buf);
/* Translate a string buffer from the source character set into the target
** system character set.
*/

void TgtTranslateSet (unsigned Index, unsigned char C);
/* Set the translation code for the given character */



/* End of tgttrans.h */

#endif
