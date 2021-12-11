/*****************************************************************************/
/*                                                                           */
/*                                 dirent.h                                  */
/*                                                                           */
/*                        Directory entries for cc65                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2005  Oliver Schmidt, <ol.sc@web.de>                                  */
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



#ifndef _DIRENT_H
#define _DIRENT_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct DIR DIR;

#if defined(__APPLE2__)

struct dirent {
    char          d_name[16];
    unsigned      d_ino;
    unsigned      d_blocks;
    unsigned long d_size;
    unsigned char d_type;
    struct {
        unsigned day  :5;
        unsigned mon  :4;
        unsigned year :7;
    }             d_cdate;
    struct {
        unsigned char min;
        unsigned char hour;
    }             d_ctime;
    unsigned char d_access;
    unsigned      d_auxtype;
    struct {
        unsigned day  :5;
        unsigned mon  :4;
        unsigned year :7;
    }             d_mdate;
    struct {
        unsigned char min;
        unsigned char hour;
    }             d_mtime;
};

#define _DE_ISREG(t)  ((t) != 0x0F)
#define _DE_ISDIR(t)  ((t) == 0x0F)
#define _DE_ISLBL(t)  (0)
#define _DE_ISLNK(t)  (0)

#elif defined(__ATARI__)

struct dirent {
    char          d_name[13];  /* 8.3 + trailing 0 */
    unsigned char d_type;
};

#define _DE_ISREG(t)  ((t) != 0xC4)
#define _DE_ISDIR(t)  ((t) == 0xC4)
#define _DE_ISLBL(t)  (0)
#define _DE_ISLNK(t)  (0)

#elif defined(__CBM__)

struct dirent {
    char                d_name[16+1];
    unsigned int        d_off;
    unsigned int        d_blocks;
    unsigned char       d_type;         /* See _CBM_T_xxx defines */

    /* bsd extensions */
    unsigned char       d_namlen;
};

/* File type specification macros. We need definitions of CBM file types. */
#include <cbm_filetype.h>

#define _DE_ISREG(t)    (((t) & _CBM_T_REG) != 0)
#define _DE_ISDIR(t)    ((t) == _CBM_T_DIR)
#define _DE_ISLBL(t)    ((t) == _CBM_T_HEADER)
#define _DE_ISLNK(t)    ((t) == _CBM_T_LNK)

#elif defined(__LYNX__)

struct dirent {
    unsigned char       d_blocks;
    unsigned int        d_offset;
    char                d_type;
    void                *d_address;
    unsigned int        d_size;
};

extern struct dirent FileEntry;
#pragma zpsym ("FileEntry");

#define _DE_ISREG(t)    (1)
#define _DE_ISDIR(t)    (0)
#define _DE_ISLBL(t)    (0)
#define _DE_ISLNK(t)    (0)

#else

struct dirent {
    char d_name[1];
};

#define _DE_ISREG(t)  (1)
#define _DE_ISDIR(t)  (0)
#define _DE_ISLBL(t)  (0)
#define _DE_ISLNK(t)  (0)

#endif



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



DIR* __fastcall__ opendir (const char* name);

struct dirent* __fastcall__ readdir (DIR* dir);

int __fastcall__ closedir (DIR* dir);

long __fastcall__ telldir (DIR* dir);

void __fastcall__ seekdir (DIR* dir, long offs);

void __fastcall__ rewinddir (DIR* dir);



/* End of dirent.h */
#endif
