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

typedef unsigned char       _driveid_t;
typedef unsigned int        _sectnum_t;
typedef unsigned int        _sectsize_t;
typedef struct __dhandle_t *_dhandle_t;

typedef struct {
  unsigned char  head;
  unsigned       track;
  unsigned       sector;
} _dio_phys_pos;


#ifdef __ATARI__
#define _dio_query_sectsize(x) ((_sectsize_t)128)
#else
#define _dio_query_sectsize(x) ((_sectsize_t)256)
#endif

extern _dhandle_t    __fastcall__ _dio_open  (_driveid_t drive_id);
extern unsigned char __fastcall__ _dio_close (_dhandle_t handle);
extern unsigned char __fastcall__ _dio_format(_dhandle_t handle,
                                              unsigned int format);
extern unsigned char __fastcall__ _dio_read(_dhandle_t handle,
                                            _sectnum_t sect_num,
                                            void *buffer);
extern unsigned char __fastcall__ _dio_write(_dhandle_t handle,
                                             _sectnum_t sect_num,
                                             const void *buffer);
extern unsigned char __fastcall__ _dio_write_verify(_dhandle_t handle,
                                                    _sectnum_t sect_num,
                                                    const void *buffer);

extern unsigned char __fastcall__ _dio_phys_to_log(_dhandle_t handle,
                                                   _dio_phys_pos *physpos,   /* input */
                                                   _sectnum_t *sectnum);     /* output */
extern unsigned char __fastcall__ _dio_log_to_phys(_dhandle_t handle,
                                                   _dio_phys_pos *physpos,   /* output */
                                                   _sectnum_t *sectnum);     /* input */

#endif /* #ifndef _DIO_H */
