/*****************************************************************************/
/*                                                                           */
/*                                  statvfs.h                                */
/*                                                                           */
/*                             statvfs(3) definition                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2023      Colin Leroy-Mira                                            */
/* EMail:        colin@colino.net                                            */
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



#ifndef _STATVFS_H
#define _STATVFS_H

#include <sys/types.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



struct statvfs {
    unsigned long     f_bsize;
    unsigned long     f_frsize;
    fsblkcnt_t        f_blocks;
    fsblkcnt_t        f_bfree;
    fsblkcnt_t        f_bavail;
    fsfilcnt_t        f_files;
    fsfilcnt_t        f_ffree;
    fsfilcnt_t        f_favail;
    unsigned long     f_fsid;
    unsigned long     f_flag;
    unsigned long     f_namemax;
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int __fastcall__ statvfs (const char* pathname, struct statvfs* buf);



/* End of statvfs.h */
#endif
