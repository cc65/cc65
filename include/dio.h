/*****************************************************************************/
/*                                                                           */
/*                                   dio.h                                   */
/*                                                                           */
/*                       Low-Level diskette I/O functions                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Christian Groessler <cpg@aladdin.de>                             */
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

typedef unsigned char       driveid_t;
typedef unsigned int        sectnum_t;
typedef unsigned int        sectsize_t;
typedef struct __dhandle_t *dhandle_t;

typedef struct {
  unsigned char  head;
  unsigned       track;
  unsigned       sector;
} dio_phys_pos;


#ifdef __ATARI__
#define dio_query_sectsize(x) ((_sectsize_t)128)
#else
#define dio_query_sectsize(x) ((_sectsize_t)256)
#endif
/* queries sector size, currently hardcoded */

extern dhandle_t     __fastcall__ dio_open(driveid_t drive_id);
/* open drive for subsequent dio access */

extern unsigned char __fastcall__ dio_close(dhandle_t handle);
/* close drive */

extern unsigned char __fastcall__ dio_read(dhandle_t handle,
                                           sectnum_t sect_num,
                                           void *buffer);
/* read sector <sect_num> from drive <handle> to memory at <buffer> */
/* the number of bytes transferred depends on the sector size */

extern unsigned char __fastcall__ dio_write(dhandle_t handle,
                                            sectnum_t sect_num,
                                            const void *buffer);
/* write memory at <buffer> to sector <sect_num> on drive <handle>, no verify */
/* the number of bytes transferred depends on the sector size */

extern unsigned char __fastcall__ dio_write_verify(dhandle_t handle,
                                                   sectnum_t sect_num,
                                                   const void *buffer);
/* write memory at <buffer> to sector <sect_num> on drive <handle>, verify after write */
/* the number of bytes transferred depends on the sector size */


extern unsigned char __fastcall__ dio_phys_to_log(dhandle_t handle,
                                                  const dio_phys_pos *physpos, /* input */
                                                  sectnum_t *sectnum);        /* output */
/* convert physical sector address (head/track/sector) to logical sector number */

extern unsigned char __fastcall__ dio_log_to_phys(dhandle_t handle,
                                                  const sectnum_t *sectnum,    /* input */
                                                  dio_phys_pos *physpos);     /* output */
/* convert logical sector number to physical sector address (head/track/sector) */

#endif /* #ifndef _DIO_H */
