/*****************************************************************************/
/*                                                                           */
/*                                filetime.h                                 */
/*                                                                           */
/*                       Replacement for Windows code                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



/* This module works around bugs in the time conversion code supplied by
** Microsoft. The problem described here:
**   http://www.codeproject.com/KB/datetime/dstbugs.aspx
** is also true when setting file times via utime(), so we need a
** replacement
*/



#ifndef FILETIME_H
#define FILETIME_H



#include <time.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int SetFileTimes (const char* Path, time_t T);
/* Set the time of last modification and the time of last access of a file to
** the given time T. This calls utime() for system where it works, and applies
** workarounds for all others (which in fact means "WINDOWS").
*/



/* End of filestat.h */

#endif
