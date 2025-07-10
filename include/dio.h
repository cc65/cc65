/*****************************************************************************/
/*                                                                           */
/*                                   dio.h                                   */
/*                                                                           */
/*                       Low-Level diskette I/O functions                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2005 Christian Groessler <chris@groessler.org>                        */
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



/* Please note: All functions in this file will set __oserror *and* return its
** value. The only exception is dio_open, which will return NULL, but __oserror
** will be set. All function will also set __oserror in case of successful
** execution, effectively clearing it.
*/



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct __dhandle_t *dhandle_t;

typedef struct {
  unsigned char  head;
  unsigned       track;
  unsigned       sector;
} dio_phys_pos;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned __fastcall__ dio_query_sectsize (dhandle_t handle);
/* returns sector size */

unsigned __fastcall__ dio_query_sectcount (dhandle_t handle);
/* returns sector count */

dhandle_t __fastcall__ dio_open (unsigned char device);
/* open device for subsequent dio access */

unsigned char __fastcall__ dio_close (dhandle_t handle);
/* close device, returns oserror (0 for success) */

unsigned char __fastcall__ dio_read (dhandle_t handle,
                                     unsigned sect_num,
                                     void *buffer);
/* read sector <sect_num> from device <handle> to memory at <buffer> */
/* the number of bytes transferred depends on the sector size */
/* returns oserror (0 for success) */

unsigned char __fastcall__ dio_write (dhandle_t handle,
                                      unsigned sect_num,
                                      const void *buffer);
/* write memory at <buffer> to sector <sect_num> on device <handle>, no verify */
/* the number of bytes transferred depends on the sector size */
/* returns oserror (0 for success) */

unsigned char __fastcall__ dio_write_verify (dhandle_t handle,
                                             unsigned sect_num,
                                             const void *buffer);
/* write memory at <buffer> to sector <sect_num> on device <handle>, verify after write */
/* the number of bytes transferred depends on the sector size */
/* returns oserror (0 for success) */

unsigned char __fastcall__ dio_phys_to_log (dhandle_t handle,
                                            const dio_phys_pos *physpos, /* input */
                                            unsigned *sectnum);          /* output */
/* convert physical sector address (head/track/sector) to logical sector number */
/* returns oserror (0 for success) */

unsigned char __fastcall__ dio_log_to_phys (dhandle_t handle,
                                            const unsigned *sectnum, /* input */
                                            dio_phys_pos *physpos);  /* output */
/* convert logical sector number to physical sector address (head/track/sector) */
/* returns oserror (0 for success) */

#endif /* #ifndef _DIO_H */
