/*****************************************************************************/
/*                                                                           */
/*                                 scanstrbuf.h                              */
/*                                                                           */
/*                     Small scanner for input from a StrBuf                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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



#ifndef SCANSTRBUF_H
#define SCANSTRBUF_H



/* common */
#include "strbuf.h"



/*****************************************************************************/
/*		     		     Code				     */
/*****************************************************************************/



void SB_SkipWhite (StrBuf* B);
/* Skip whitespace in the string buffer */

int SB_GetSym (StrBuf* B, char* S);
/* Get a symbol from the string buffer. S must be able to hold MAX_IDENTLEN
 * characters. Returns 1 if a symbol was found and 0 otherwise.
 */

int SB_GetString (StrBuf* B, StrBuf* S);
/* Get a string from the string buffer. S will be initialized by the function
 * and will return the correctly terminated string on return. The function
 * returns 1 if a string was found and 0 otherwise.
 */

int SB_GetNumber (StrBuf* B, long* Val);
/* Get a number from the string buffer. Accepted formats are decimal, octal,
 * hex and character constants. Numeric constants may be preceeded by a 
 * minus or plus sign. The function returns 1 if a number was found and
 * zero otherwise.
 */



/* End of scanstrbuf.h */
#endif





