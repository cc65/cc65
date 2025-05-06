/*****************************************************************************/
/*                                                                           */
/*                                  errno.h                                  */
/*                                                                           */
/*                                Error codes                                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2010, Ullrich von Bassewitz                                      */
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



#ifndef _ERRNO_H
#define _ERRNO_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Operating system specific error code */
extern unsigned char __oserror;

#if __CC65_STD__ >= __CC65_STD_CC65__
/* define the name with just one underscore for backwards compatibility */
#define _oserror __oserror
#endif

extern int __errno;
/* System errors go here */

#define errno   __errno
/* errno must be a macro */



/* Possible error codes */
#define ENOENT          1       /* No such file or directory */
#define ENOMEM          2       /* Out of memory */
#define EACCES          3       /* Permission denied */
#define ENODEV          4       /* No such device */
#define EMFILE          5       /* Too many open files */
#define EBUSY           6       /* Device or resource busy */
#define EINVAL          7       /* Invalid argument */
#define ENOSPC          8       /* No space left on device */
#define EEXIST          9       /* File exists */
#define EAGAIN          10      /* Try again */
#define EIO             11      /* I/O error */
#define EINTR           12      /* Interrupted system call */
#define ENOSYS          13      /* Function not implemented */
#define ESPIPE          14      /* Illegal seek */
#define ERANGE          15      /* Range error */
#define EBADF           16      /* Bad file number */
#define ENOEXEC         17      /* Exec format error */
#define EUNKNOWN        18      /* Unknown OS specific error */



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



int __fastcall__ __osmaperrno (unsigned char oserror);
/* Map an operating system specific error code (for example from __oserror)
** into one of the E... codes above. It is user callable.
*/

#if __CC65_STD__ >= __CC65_STD_CC65__
/* define the name with just one underscore for backwards compatibility */
#define _osmaperrno __osmaperrno
#endif

unsigned char __fastcall__ __seterrno (unsigned char code);
/* Set errno to a specific error code and return zero. Used by the library */

#if __CC65_STD__ >= __CC65_STD_CC65__
/* define the name with just one underscore for backwards compatibility */
#define _seterrno __seterrno
#endif

int __fastcall__ __directerrno (unsigned char code);
/* Set errno to a specific error code, clear __oserror and return -1. Used
** by the library.
*/

#if __CC65_STD__ >= __CC65_STD_CC65__
/* define the name with just one underscore for backwards compatibility */
#define _directerrno __directerrno
#endif

int __fastcall__ __mappederrno (unsigned char code);
/* Set __oserror to the given platform specific error code. If it is a real
** error code (not zero) set errno to the corresponding system error code
** and return -1. Otherwise return zero.
** Used by the library.
*/

#if __CC65_STD__ >= __CC65_STD_CC65__
/* define the name with just one underscore for backwards compatibility */
#define _mappederrno __mappederrno
#endif

/* End of errno.h */
#endif



