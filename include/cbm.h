/*****************************************************************************/
/*                                                                           */
/*				     cbm.h			       	     */
/*                                                                           */
/*		        CBM system specific definitions			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
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



#ifndef _CBM_H
#define _CBM_H



/* Check for errors */
#if !defined(__CBM__)
#  error This module may only be used when compiling for CBM machines!
#endif



/* Load the system specific files here, if needed */
#if defined(__C64__) && !defined(_C64_H)
#  include <c64.h>
#elif defined(__VIC20__) && !defined(_VIC20_H)
#  include <vic20.h>
#elif defined(__C128__) && !defined(_C128_H)
#  include <c128.h>
#elif defined(__PLUS4__) && !defined(_PLUS4_H)
#  include <plus4.h>
#elif defined(__C16__) && !defined(_C16_H)
#  include <c16.h>
#elif defined(__CBM510__) && !defined(_CBM510_H)
#  include <cbm510.h>
#elif defined(__CBM610__) && !defined(_CBM610_H)
#  include <cbm610.h>
#elif defined(__PET__) && !defined(_PET_H)
#  include <pet.h>
#endif



/*****************************************************************************/
/*                                 Variables                                 */
/*****************************************************************************/



/* The file stream implementation and the POSIX I/O functions will use the
 * following variables to determine the file type and the disk unit to use.
 */
extern unsigned char _curunit;          /* Default 8 */
extern unsigned char _filetype;         /* Default 'u' */



/*****************************************************************************/
/*                      Characters codes (CBM charset)                       */
/*****************************************************************************/



#define CH_HLINE    		 96
#define CH_VLINE    		125
#define	CH_ULCORNER 		176
#define CH_URCORNER 		174
#define CH_LLCORNER 		173
#define CH_LRCORNER 		189
#define CH_TTEE	    		178
#define CH_RTEE	    		179
#define CH_BTEE	    		177
#define CH_LTEE	    		171
#define CH_CROSS    		123
#define CH_CURS_UP		145
#define CH_CURS_DOWN		 17
#define CH_CURS_LEFT		157
#define CH_CURS_RIGHT		 29
#define CH_PI			126
#define CH_DEL			 20
#define CH_INS			148
#define CH_ENTER                '\n'
#define CH_STOP                   3
#define CH_ESC                   27



/*****************************************************************************/
/*                Definitions for directory reading functions                */
/*****************************************************************************/



/* CBM FILE TYPES */
#define CBM_T_DEL       0
#define CBM_T_SEQ       1
#define CBM_T_PRG       2
#define CBM_T_USR       3
#define CBM_T_REL       4
#define CBM_T_CBM       5       /* 1581 sub-partition */
#define CBM_T_DIR       6       /* IDE64 and CMD sub-directory */
#define CBM_T_RESERVED  7       /* Not used, but kept free for compatibility */
#define CBM_T_VRP       8       /* Vorpal fast-loadable format */
#define CBM_T_OTHER     9       /* Other file-types not yet defined */
#define CBM_T_HEADER   10       /* Disk header / title */

/* CBM FILE ACCESS */
#define CBM_A_RO    1           /* Read only   */
#define CBM_A_RW    3           /* Read, Write */

struct cbm_dirent {
    char          name[17];     /* File name in PETSCII, limited to 16 chars */
    unsigned int  size;         /* Size in 256B blocks */
    unsigned char type;
    unsigned char access;
};



/*****************************************************************************/
/*                               Machine info                                */
/*****************************************************************************/



#define TV_NTSC         0
#define TV_PAL          1
#define TV_OTHER        2

unsigned char __fastcall__ get_tv (void);
/* Return the video mode the machine is using. */



/*****************************************************************************/
/*                           CBM kernal functions                            */
/*****************************************************************************/



/* Constants to use with cbm_open() for opening a file for reading or
 * writing without the need to append ",r" or ",w" to the filename.
 *
 * e.g.: cbm_open(2, 8, CBM_READ, "data,s");
 */
#define CBM_READ    0
#define CBM_WRITE   1

/* Kernel level functions */
void __fastcall__ cbm_k_setlfs (unsigned char LFN, unsigned char DEV,
                                unsigned char SA);
void __fastcall__ cbm_k_setnam (const char* Name);
unsigned int __fastcall__ cbm_k_load(unsigned char flag, unsigned addr);
unsigned char __fastcall__ cbm_k_save(unsigned int start, unsigned int end);
unsigned char __fastcall__ cbm_k_open (void);
void __fastcall__ cbm_k_close (unsigned char FN);
unsigned char __fastcall__ cbm_k_readst (void);
unsigned char __fastcall__ cbm_k_chkin (unsigned char FN);
unsigned char __fastcall__ cbm_k_ckout (unsigned char FN);
unsigned char __fastcall__ cbm_k_basin (void);
void __fastcall__ cbm_k_bsout (unsigned char C);
void __fastcall__ cbm_k_clrch (void);



/*****************************************************************************/
/*                       BASIC-like file I/O functions                       */
/*****************************************************************************/



/* All cbm_* IO functions set _oserror (see errno.h) in case of an
 * error.
 *
 * errorcode	BASIC error
 *	1   =	too many files
 *	2   =	file open
 *	3   =	file not open
 *	4   =	file not found
 *	5   =	device not present
 *	6   =	not input file
 *	7   =	not output file
 *	8   =	missing filename
 *	9   =	illegal device number
 */



unsigned int cbm_load (const char* name, unsigned char device, void* data);
/* Loads file "name" from given device to given address or to the load
 * address of the file if "data" is the null pointer (like load"name",8,1
 * in BASIC).
 * Returns number of bytes that where loaded if loading was successful
 * otherwise 0. "_oserror" contains an errorcode then (see table below).
 */

unsigned char cbm_save (const char* name, unsigned char device,
                        const void* data, unsigned int size);
/* Saves "size" bytes starting at "data" to a file.
 * Returns 0 if saving was successful, otherwise an errorcode (see table
 * below).
 */

unsigned char __fastcall__ cbm_open (unsigned char lfn,
                                     unsigned char device,
                                     unsigned char sec_addr,
                                     const char* name);
/* Opens a file. Works just like the BASIC command.
 * Returns 0 if opening was successful, otherwise an errorcode (see table
 * below).
 */

void __fastcall__ cbm_close (unsigned char lfn);
/* Closes a file */

int __fastcall__ cbm_read (unsigned char lfn, void* buffer, unsigned int size);
/* Reads up to "size" bytes from a file to "buffer".
 * Returns the number of actually read bytes, 0 if there are no bytes left
 * (EOF) or -1 in case of an error. _oserror contains an errorcode then (see
 * table below).
 */

int __fastcall__ cbm_write (unsigned char lfn, void* buffer, unsigned int size);
/* Writes up to "size" bytes from "buffer" to a file.
 * Returns the number of actually written bytes or -1 in case of an error.
 * _oserror contains an errorcode then (see above table).
 */

unsigned char __fastcall__ cbm_opendir (unsigned char lfn, unsigned char device);
/* Opens directory listing.
 * Returns 0 if opening directory was successful,
 * othervise errorcode corresponding to cbm_open()
 */

unsigned char __fastcall__ cbm_readdir (unsigned char lfn, struct cbm_dirent* l_dirent);
/* Reads one directory line into cbm_dirent structure.
 * Returns 0 if reading directory line was successful.
 * Returns 'true' if reading directory failed or no more files to read.
 */

void __fastcall__ cbm_closedir (unsigned char lfn);
/* Closes directory by cbm_close (unsigned char lfn) */



/* End of cbm.h */
#endif


