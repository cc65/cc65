/*****************************************************************************/
/*                                                                           */
/*                                   cbm.h                                   */
/*                                                                           */
/*                      CBM system-specific definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2015, Ullrich von Bassewitz                                      */
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



#ifndef _CBM_H
#define _CBM_H



/* Check for errors */
#if !defined(__CBM__)
#  error This module may be used only when compiling for CBM machines!
#endif



/* We need NULL. */
#include <stddef.h>

/* Load the system-specific files here, if needed. */
#if   defined(__C64__)    && !defined(_C64_H)
#  include <c64.h>
#elif defined(__VIC20__)  && !defined(_VIC20_H)
#  include <vic20.h>
#elif defined(__C128__)   && !defined(_C128_H)
#  include <c128.h>
#elif defined(__PLUS4__)  && !defined(_PLUS4_H)
#  include <plus4.h>
#elif defined(__C16__)    && !defined(_C16_H)
#  include <c16.h>
#elif defined(__CBM510__) && !defined(_CBM510_H)
#  include <cbm510.h>
#elif defined(__CBM610__) && !defined(_CBM610_H)
#  include <cbm610.h>
#elif defined(__PET__)    && !defined(_PET_H)
#  include <pet.h>
#elif defined(__CX16__)   && !defined(_CX16_H)
#  include <cx16.h>
#elif defined(__C65__)   && !defined(_C65_H)
#  include <c65.h>
#elif defined(__MEGA65__)   && !defined(_MEGA65_H)
#  include <mega65.h>
#endif

/* Include definitions for CBM file types */
#include <cbm_filetype.h>



#define JOY_FIRE_MASK   JOY_BTN_1_MASK
#define JOY_FIRE(v)     ((v) & JOY_FIRE_MASK)



/*****************************************************************************/
/*                                 Variables                                 */
/*****************************************************************************/



/* The file stream implementation and the POSIX I/O functions will
** use the following variable to determine the file type to use.
*/
extern char _filetype;          /* Defaults to 's' */



/*****************************************************************************/
/*                       Character-codes (CBM charset)                       */
/*****************************************************************************/



#define CH_HLINE        192
#define CH_VLINE        221
#define CH_ULCORNER     176
#define CH_URCORNER     174
#define CH_LLCORNER     173
#define CH_LRCORNER     189
#define CH_TTEE         178
#define CH_BTEE         177
#define CH_LTEE         171
#define CH_RTEE         179
#define CH_CROSS        219
#define CH_CURS_UP      145
#define CH_CURS_DOWN     17
#define CH_CURS_LEFT    157
#define CH_CURS_RIGHT    29
#define CH_PI           222
#define CH_HOME          19
#define CH_DEL           20
#define CH_INS          148
#define CH_ENTER         13
#define CH_STOP           3
#define CH_LIRA          92
#define CH_ESC           27
#define CH_FONT_LOWER    14
#define CH_FONT_UPPER   142



/*****************************************************************************/
/*                Definitions for directory reading functions                */
/*****************************************************************************/



/* CBM FILE ACCESS */
#define CBM_A_RO    1           /* Read only   */
#define CBM_A_WO    2           /* Write only  */
#define CBM_A_RW    3           /* Read, Write */

struct cbm_dirent {
             char name[17];     /* File name in PetSCII, limited to 16 chars */
    unsigned int  size;         /* Size, in 254-/256-byte blocks */
    unsigned char type;
    unsigned char access;
};



/*****************************************************************************/
/*                               Machine info                                */
/*****************************************************************************/



#define TV_NTSC         0
#define TV_PAL          1
#define TV_OTHER        2

unsigned char get_tv (void);
/* Return the video mode the machine is using. */

#define KBREPEAT_CURSOR 0x00
#define KBREPEAT_NONE   0x40
#define KBREPEAT_ALL    0x80

unsigned char __fastcall__ kbrepeat (unsigned char mode);
/* Changes which keys have automatic repeat. */

#if !defined(__CBM610__)
void waitvsync (void);
/* Wait for the start of the next video field. */
#endif

/*****************************************************************************/
/*                           CBM kernal functions                            */
/*****************************************************************************/



/* Constants to use with cbm_open() for openning a file for reading or
** writing without the need to append ",r" or ",w" to the filename.
**
** e.g., cbm_open(2, 8, CBM_READ, "0:data,s");
*/
#define CBM_READ        0       /* default is ",p" */
#define CBM_WRITE       1       /* ditto */
#define CBM_SEQ         2       /* default is ",r" -- or ",s" when writing */

/* Kernal-level functions */
unsigned char cbm_k_acptr (void);
unsigned char cbm_k_basin (void);
void __fastcall__ cbm_k_bsout (unsigned char C);
unsigned char __fastcall__ cbm_k_chkin (unsigned char FN);
unsigned char cbm_k_chrin (void);
void __fastcall__ cbm_k_chrout (unsigned char C);
void __fastcall__ cbm_k_ciout (unsigned char C);
unsigned char __fastcall__ cbm_k_ckout (unsigned char FN);
void cbm_k_clall (void);
void __fastcall__ cbm_k_close (unsigned char FN);
void cbm_k_clrch (void);
unsigned char cbm_k_getin (void);
unsigned cbm_k_iobase (void);
void __fastcall__ cbm_k_listen (unsigned char dev);
unsigned int __fastcall__ cbm_k_load(unsigned char flag, unsigned addr);
unsigned char cbm_k_open (void);
unsigned char cbm_k_readst (void);
unsigned char __fastcall__ cbm_k_save(unsigned int start, unsigned int end);
void cbm_k_scnkey (void);
void __fastcall__ cbm_k_second (unsigned char addr);
void __fastcall__ cbm_k_setlfs (unsigned char LFN, unsigned char DEV,
                                unsigned char SA);
void __fastcall__ cbm_k_setnam (const char* Name);
void __fastcall__ cbm_k_settim (unsigned long timer);
void __fastcall__ cbm_k_talk (unsigned char dev);
void __fastcall__ cbm_k_tksa (unsigned char addr);
void cbm_k_udtim (void);
void cbm_k_unlsn (void);
void cbm_k_untlk (void);



/*****************************************************************************/
/*                       BASIC-like file I/O functions                       */
/*****************************************************************************/



/* The cbm_* I/O functions below set __oserror (see errno.h),
** in case of an error.
**
** error-code   BASIC error
** ----------   -----------
**       1  =   too many files
**       2  =   file open
**       3  =   file not open
**       4  =   file not found
**       5  =   device not present
**       6  =   not input-file
**       7  =   not output-file
**       8  =   missing file-name
**       9  =   illegal device-number
**
**      10  =   STOP-key pushed
**      11  =   general I/O-error
*/



unsigned int __fastcall__ cbm_load (const char* name, unsigned char device, void* data);
/* Loads file "name", from given device, to given address -- or, to the load
** address of the file if "data" is the null pointer (like load"name",8,1
** in BASIC).
** Returns number of bytes that were loaded if loading was successful;
** otherwise 0, "__oserror" contains an error-code, then (see table above).
*/

unsigned char __fastcall__ cbm_save (const char* name, unsigned char device,
                                     const void* addr, unsigned int size);
/* Saves "size" bytes, starting at "addr", to a file.
** Returns 0 if saving was successful, otherwise an error-code (see table
** above).
*/

unsigned char __fastcall__ cbm_open (unsigned char lfn, unsigned char device,
                                     unsigned char sec_addr, const char* name);
/* Opens a file. Works just like the BASIC command.
** Returns 0 if openning was successful, otherwise an error-code (see table
** above).
*/

void __fastcall__ cbm_close (unsigned char lfn);
/* Closes a file */

int __fastcall__ cbm_read (unsigned char lfn, void* buffer, unsigned int size);
/* Reads up to "size" bytes from a file into "buffer".
** Returns the number of actually-read bytes, 0 if there are no bytes left.
** -1 in case of an error; then, __oserror contains an error-code (see table
** above).  (Remember:  0 means end-of-file; -1 means error.)
*/

int __fastcall__ cbm_write (unsigned char lfn, const void* buffer,
                            unsigned int size);
/* Writes up to "size" bytes from "buffer" to a file.
** Returns the number of actually-written bytes, or -1 in case of an error;
** __oserror contains an error-code, then (see above table).
*/

unsigned char cbm_opendir (unsigned char lfn, unsigned char device, ...);
/* Opens directory listing. Returns 0 if opening directory was successful;
** otherwise, an error-code corresponding to cbm_open(). As an optional
** argument, the name of the directory may be passed to the function. If
** no explicit name is specified, "$" is used.
*/

unsigned char __fastcall__ cbm_readdir (unsigned char lfn,
                                        struct cbm_dirent* l_dirent);
/* Reads one directory line into cbm_dirent structure.
** Returns 0 if reading directory-line was successful.
** Returns non-zero if reading directory failed, or no more file-names to read.
** Returns 2 on last line.  Then, l_dirent->size = the number of "blocks free",
** "blocks used", or "mb free".  Return codes:
** 0 = read file-name
** 1 = couldn't read directory
** 2 = read "blocks free", "blocks used", or "mb free"
** 3 = couldn't find start of file-name
** 4 = couldn't find end of file-name
** 5 = couldn't read file-type
** 6 = premature end of file
*/

void __fastcall__ cbm_closedir (unsigned char lfn);
/* Closes directory by cbm_close(lfn) */



/* End of cbm.h */
#endif
