/*****************************************************************************/
/*                                                                           */
/*                                filestat.h                                 */
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
** Microsoft. See here for a description of the problem:
**   http://www.codeproject.com/KB/datetime/dstbugs.aspx
** Please let me note that I find it absolutely unacceptable to just declare
** buggy behaviour like this "works as designed" as Microsoft does. The
** problems did even make it into .NET, where the DateTime builtin data type
** has exactly the same problems as described in the article above.
*/



#ifndef FILESTAT_H
#define FILESTAT_H



#include <sys/types.h>
#include <sys/stat.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int FileStat (const char* Path, struct stat* Buf);
/* Replacement function for stat() */



/* End of filestat.h */

#endif
