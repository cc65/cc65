/*****************************************************************************/
/*                                                                           */
/*                                  fcntl.h                                  */
/*                                                                           */
/*                            File control operations                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#ifndef _FCNTL_H
#define _FCNTL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Flag values for the open() call */
#define O_RDONLY        0x01
#define O_WRONLY        0x02
#define O_RDWR          0x03
#define O_CREAT         0x10
#define O_TRUNC         0x20
#define O_APPEND        0x40
#define O_EXCL          0x80



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* Functions */
int open (const char* name, int flags, ...);    /* May take a mode argument */
int __fastcall__ close (int fd);
int __fastcall__ creat (const char* name, unsigned mode);



/* End of fcntl.h */
#endif



