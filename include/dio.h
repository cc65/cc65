/*****************************************************************************/
/*                                                                           */
/*				     dio.h			       	     */
/*                                                                           */
/*		         Low-Level diskette I/O functions		     */
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

typedef unsigned char _driveid_t;
typedef unsigned int  _sectnum_t;
typedef unsigned int  _sectsize_t;


#ifdef __ATARI__
#define _dio_query_sectsize(x) ((_sectsize_t)128)
#else
#define _dio_query_sectsize(x) ((_sectsize_t)256)
#endif

extern unsigned char __fastcall__ _dio_open        (_driveid_t drive_id);
extern unsigned char __fastcall__ _dio_close       (_driveid_t drive_id);
extern unsigned char __fastcall__ _dio_format      (_driveid_t drive_id, unsigned int format);
extern unsigned char __fastcall__ _dio_read        (_driveid_t drive_id, _sectnum_t sect_num, void *buffer);
extern unsigned char __fastcall__ _dio_write       (_driveid_t drive_id, _sectnum_t sect_num, void *buffer);
extern unsigned char __fastcall__ _dio_write_verify(_driveid_t drive_id, _sectnum_t sect_num, void *buffer);


extern _sectnum_t __fastcall__ _dio_chs_to_snum(unsigned int cyl, unsigned int head, unsigned int sector);
extern void       __fastcall__ _dio_snum_to_chs(_sectnum_t sect_num,
                                                unsigned int *cyl,
                                                unsigned int *head,
                                                unsigned int *sector);

#endif /* #ifndef _DIO_H */
