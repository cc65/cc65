/*****************************************************************************/
/*                                                                           */
/*                                xsprintf.h                                 */
/*                                                                           */
/*                       Replacement sprintf function                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



/* The snprintf style functions in this module are obsolete. They call just
** snprintf under the hood. New code should call snprintf directly. 
** The sprintf style functions are still useful. They do some checks and will
** FAIL in case of errors (buffer too small or similar).
*/



#ifndef XSPRINTF_H
#define XSPRINTF_H



#include <stdlib.h>
#include <stdarg.h>

#include "attrib.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int xvsnprintf (char* Buf, size_t Size, const char* Format, va_list ap)
        attribute ((format (printf, 3, 0)));
/* A basic vsnprintf implementation. Does currently only support integer
** formats.
*/

int xsnprintf (char* Buf, size_t Size, const char* Format, ...)
        attribute ((format (printf, 3, 4)));
/* A basic snprintf implementation. Does currently only support integer
** formats.
*/

int xsprintf (char* Buf, size_t BufSize, const char* Format, ...)
        attribute ((format (printf, 3, 4)));
/* Replacement function for sprintf. Will FAIL on errors. */

int xvsprintf (char* Buf, size_t BufSize, const char* Format, va_list ap)
        attribute ((format (printf, 3, 0)));
/* Replacement function for sprintf. Will FAIL on errors. */



/* End of xsprintf.h */

#endif
