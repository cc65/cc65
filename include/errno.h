/*****************************************************************************/
/*                                                                           */
/*				    errno.h				     */
/*                                                                           */
/*				  Error codes				     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#ifndef _ERRNO_H
#define _ERRNO_H



/* Operating system specific error codes */
extern unsigned char _oserror;

/* Mapper function, don't call directly */
void _maperrno (void);

/* This one is called under the hood. User callable. */
int __fastcall__ _osmaperrno (unsigned char oserror);

/* System error codes go here */
extern int _errno;

/* errno must be a macro, here the mapper is called */
#define errno  	(_maperrno(), _errno)



/* Possible error codes */
#define	ENOENT		1	/* No such file or directory */
#define ENOMEM		2	/* Out of memory */
#define EACCES		3	/* Permission denied */
#define ENODEV	       	4	/* No such device */
#define EMFILE		5	/* Too many open files */
#define EBUSY		6	/* Device or resource busy */
#define EINVAL		7	/* Invalid argument */
#define ENOSPC		8	/* No space left on device */
#define EEXIST		9	/* File exists */
#define EAGAIN		10	/* Try again */
#define EIO		11	/* I/O error */
#define EINTR		12	/* Interrupted system call */
#define ENOSYS		13	/* Function not implemented */
#define ESPIPE		14	/* Illegal seek */
#define ERANGE          15      /* Range error */
#define EUNKNOWN       	16	/* Unknown OS specific error */



#endif



