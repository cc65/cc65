/*****************************************************************************/
/*                                                                           */
/*                                 utsname.h                                 */
/*                                                                           */
/*                         Return system information                         */
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



#ifndef _UTSNAME_H
#define _UTSNAME_H



/*****************************************************************************/
/*	       	   	  	     Data				     */
/*****************************************************************************/



/* Beware: The library sources written in assembler have knowledge about this
 * struct!
 */
struct utsname {
    char sysname[9];
    char nodename[9];
    char release[5];
    char version[5];
    char machine[9];
};
   


/*****************************************************************************/
/*	       	   		     Code				     */
/*****************************************************************************/



int __fastcall__ uname (struct utsname* buf);
/* Return system information */



/* End of utsname.h */
#endif



