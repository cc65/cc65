/*****************************************************************************/
/*                                                                           */
/*                                   dio.h                                   */
/*                                                                           */
/*                       Low-Level diskette I/O functions                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2005 Christian Groessler <cpg@aladdin.de>                             */
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



#ifndef _DIO_H
#define _DIO_H



/* Please note: All functions in this file will set _oserror *and* return its
 * value. The only exception is dio_open, which will return NULL, but _oserror
 * will be set. All function will also set _oserror in case of successful
 * execution, effectively clearing it.
 */



/*****************************************************************************/
/*                                   Data				     */
/*****************************************************************************/



typedef unsigned char       driveid_t;
typedef unsigned int        sectnum_t;
typedef unsigned int        sectsize_t;
typedef struct __dhandle_t *dhandle_t;

typedef struct {
  unsigned char  head;
  unsigned       track;
  unsigned       sector;
} dio_phys_pos;



/*****************************************************************************/
/*                                   Code				     */
/*****************************************************************************/



sectsize_t __fastcall__ dio_query_sectsize(dhandle_t handle);
/* queries sector size, currently hardcoded */

sectnum_t __fastcall__ dio_query_sectcount(dhandle_t handle);
/* Return the sector count for a disk. */

dhandle_t __fastcall__ dio_open(driveid_t drive_id);
/* open drive for subsequent dio access */

unsigned char __fastcall__ dio_close(dhandle_t handle);
/* close drive, returns oserror (0 for success) */

unsigned char __fastcall__ dio_read(dhandle_t handle,
                                    sectnum_t sect_num,
                                    void *buffer);
/* read sector <sect_num> from drive <handle> to memory at <buffer> */
/* the number of bytes transferred depends on the sector size */
/* returns oserror (0 for success) */

unsigned char __fastcall__ dio_write(dhandle_t handle,
                                     sectnum_t sect_num,
                                     const void *buffer);
/* write memory at <buffer> to sector <sect_num> on drive <handle>, no verify */
/* the number of bytes transferred depends on the sector size */
/* returns oserror (0 for success) */

unsigned char __fastcall__ dio_write_verify(dhandle_t handle,
                                            sectnum_t sect_num,
                                            const void *buffer);
/* write memory at <buffer> to sector <sect_num> on drive <handle>, verify after write */
/* the number of bytes transferred depends on the sector size */
/* returns oserror (0 for success) */


unsigned char __fastcall__ dio_phys_to_log(dhandle_t handle,
                                           const dio_phys_pos *physpos, /* input */
                                           sectnum_t *sectnum);         /* output */
/* convert physical sector address (head/track/sector) to logical sector number */
/* returns oserror (0 for success) */

unsigned char __fastcall__ dio_log_to_phys(dhandle_t handle,
                                           const sectnum_t *sectnum, /* input */
                                           dio_phys_pos *physpos);   /* output */
/* convert logical sector number to physical sector address (head/track/sector) */
/* returns oserror (0 for success) */

#endif /* #ifndef _DIO_H */
