/*****************************************************************************/
/*                                                                           */
/*                                  stat.h                                   */
/*                                                                           */
/*                   Constants for the mode argument of open                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
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



#ifndef _STAT_H
#define _STAT_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* These are the values for the traditional UNIX mode bits:
** https://en.wikipedia.org/wiki/File_system_permissions#Numeric_notation
** (S_IREAD and S_IWRITE are aliases for S_IRUSR and S_IWUSR)
**
** Must match the values in asminc/stat.inc and src/sim65/paravirt.c
*/

#define S_IREAD  0400
#define S_IWRITE 0200


/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* End of stat.h */
#endif
