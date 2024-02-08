/*****************************************************************************/
/*                                                                           */
/*                                  types.h                                  */
/*                                                                           */
/*                   Primitive system data types for cc65                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
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



#ifndef _TYPES_H
#define _TYPES_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* off_t is also defined in unistd.h */
#ifndef _HAVE_off_t
#define _HAVE_off_t
typedef long int off_t;
#endif

#ifndef _HAVE_dev_t
#define _HAVE_dev_t
typedef unsigned long int dev_t;
#endif

#ifndef _HAVE_ino_t
#define _HAVE_ino_t
typedef unsigned long int ino_t;
#endif

#ifndef _HAVE_nlink_t
#define _HAVE_nlink_t
typedef unsigned long int nlink_t;
#endif

#ifndef _HAVE_uid_t
#define _HAVE_uid_t
typedef unsigned char uid_t;
#endif

#ifndef _HAVE_gid_t
#define _HAVE_gid_t
typedef unsigned char gid_t;
#endif

#ifndef _HAVE_mode_t
#define _HAVE_mode_t
typedef unsigned char mode_t;
#endif

#ifndef _HAVE_fsblkcnt_t
#define _HAVE_fsblkcnt_t
typedef unsigned long int fsblkcnt_t;
#endif

#ifndef _HAVE_fsfilcnt_t
#define _HAVE_fsfilcnt_t
typedef unsigned long int fsfilcnt_t;
#endif



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* End of types.h */
#endif
