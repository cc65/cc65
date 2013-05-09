/*****************************************************************************/
/*                                                                           */
/*                                 unistd.h                                  */
/*                                                                           */
/*                  Unix compatibility header file for cc65                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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



#ifndef _UNISTD_H
#define _UNISTD_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Predefined file handles */
#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2

/* WE need size_t */
#ifndef _HAVE_size_t
#define _HAVE_size_t
typedef unsigned size_t;
#endif

/* We need off_t if sys/types is not included */
#ifndef _HAVE_off_t
#define _HAVE_off_t
typedef long int off_t;
#endif

/* Stuff for getopt */
extern char *optarg;
extern int optind, opterr, optopt;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* Files */
int __fastcall__ write (int fd, const void* buf, unsigned count);
int __fastcall__ read (int fd, void* buf, unsigned count);
off_t __fastcall__ lseek (int fd, off_t offset, int whence);
int __fastcall__ unlink (const char* name);     /* Same as remove() */

/* Directories */
int __fastcall__ chdir (const char* name);
char* __fastcall__ getcwd (char* buf, size_t size);
int mkdir (const char* name, ...);              /* May take a mode argument */
int __fastcall__ rmdir (const char* name);

/* Others */
unsigned __fastcall__ sleep (unsigned seconds);
int __fastcall__ getopt (int argc, char* const* argv, const char* optstring);

/* Non standard: */
#if __CC65_STD__ == __CC65_STD_CC65__
int __fastcall__ exec (const char* progname, const char* cmdline);
#endif



/* End of unistd.h */
#endif



