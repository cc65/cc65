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
/*	       	   	  	     Data	   			     */
/*****************************************************************************/



typedef struct DIR DIR;

#if defined(__APPLE2__) || defined(__APPLE2ENH__)

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

#elif defined(__ATARI__)

struct dirent {
    char d_name[13];  /* 8.3 + trailing 0 */
};

#elif defined(__LYNX__)

struct dirent {
    unsigned char	d_blocks;
    unsigned int	d_offset;
    char		d_type;
    void		*d_address;
    unsigned int	d_size;
};

extern struct dirent FileEntry;
#pragma zpsym ("FileEntry");

#else

struct dirent {
    char d_name[1];
};

#endif  /* __APPLE2__ or __APPLE2ENH__ */



/*****************************************************************************/
/*		   		     Code	   			     */
/*****************************************************************************/



DIR* __fastcall__ opendir (const char* name);

struct dirent* __fastcall__ readdir (DIR* dir);

int __fastcall__ closedir (DIR* dir);

void __fastcall__ rewinddir (DIR* dir);



/* End of dirent.h */
#endif
