/*****************************************************************************/
/*                                                                           */
/*                                  stat.h                                   */
/*                                                                           */
/*                             stat(2) definition                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef _STAT_H
#define _STAT_H

#include <time.h>
#include <target.h>
#include <sys/types.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define S_IREAD  0x01
#define S_IWRITE 0x02

#define S_IFMT   0x03

struct stat {
    dev_t           st_dev;
    ino_t           st_ino;
    mode_t          st_mode;
    nlink_t         st_nlink;
    uid_t           st_uid;
    gid_t           st_gid;
    off_t           st_size;
    struct timespec st_atim;
    struct timespec st_ctim;
    struct timespec st_mtim;
    #ifdef __APPLE2__
    unsigned char   st_access;
    unsigned char   st_type;
    unsigned int    st_auxtype;
    unsigned char   st_storagetype;
    unsigned int    st_blocks;
    struct datetime st_mtime;
    struct datetime st_ctime;
    #endif
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int __fastcall__ stat (const char* pathname, struct stat* statbuf);



/* End of stat.h */
#endif
