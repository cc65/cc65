/*
 * fcntl.h
 *
 * Ullrich von Bassewitz, 30.05.1998
 *
 */



#ifndef _FCNTL_H
#define _FCNTL_H



/* Flag values for the open() call */
#define O_RDONLY        0x01
#define O_WRONLY        0x02
#define O_RDWR          0x03
#define O_CREAT         0x10
#define O_TRUNC         0x20
#define O_APPEND        0x40



/* Functions */
int open (const char* name, int flags, ...);	/* May take a mode argument */
int close (int fd);
int write (int fd, const void* buf, unsigned count);
int read (int fd, void* buf, unsigned count);
int mkdir (const char* name, ...);		/* May take a mode argument */
int rmdir (const char* name);



/* End of fcntl.h */
#endif



