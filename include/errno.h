/*
 * errno.h
 *
 * Ullrich von Bassewitz, 18.08.1998
 *
 */



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
#define EUNKNOWN       	15	/* Unknown OS specific error */



#endif



