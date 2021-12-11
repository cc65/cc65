/*****************************************************************************/
/*                                                                           */
/*                                  stdio.h                                  */
/*                                                                           */
/*                               Input/output                                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#ifndef _STDIO_H
#define _STDIO_H



#include <stddef.h>
#include <stdarg.h>
#include <limits.h>



/* Types */
typedef struct _FILE FILE;
typedef unsigned long fpos_t;

/* Standard file descriptors */
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

/* Standard defines */
#define _IOFBF          0
#define _IOLBF          1
#define _IONBF          2
#define BUFSIZ          256
#define EOF             -1
#define FOPEN_MAX       8
#define SEEK_CUR        0
#define SEEK_END        1
#define SEEK_SET        2
#define TMP_MAX         256

#define FILENAME_MAX    PATH_MAX
#define L_tmpnam        FILENAME_MAX



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* Functions */
void __fastcall__ clearerr (FILE* f);
int __fastcall__ fclose (FILE* f);
int __fastcall__ feof (FILE* f);
int __fastcall__ ferror (FILE* f);
int __fastcall__ fflush (FILE* f);
int __fastcall__ fgetc (FILE* f);
char* __fastcall__ fgets (char* buf, size_t size, FILE* f);
FILE* __fastcall__ fopen (const char* name, const char* mode);
int fprintf (FILE* f, const char* format, ...);
int __fastcall__ fputc (int c, FILE* f);
int __fastcall__ fputs (const char* s, FILE* f);
size_t __fastcall__ fread (void* buf, size_t size, size_t count, FILE* f);
FILE* __fastcall__ freopen (const char* name, const char* mode, FILE* f);
size_t __fastcall__ fwrite (const void* buf, size_t size, size_t count, FILE* f);
int __fastcall__ fgetpos (FILE* f, fpos_t *pos);
int __fastcall__ fsetpos (FILE* f, const fpos_t* pos);
long __fastcall__ ftell (FILE* f);
int __fastcall__ fseek (FILE* f, long offset, int whence);
void __fastcall__ rewind (FILE *f);
int getchar (void);
char* __fastcall__ gets (char* s);
void __fastcall__ perror (const char* s);
int printf (const char* format, ...);
int __fastcall__ putchar (int c);
int __fastcall__ puts (const char* s);
int __fastcall__ remove (const char* name);
int __fastcall__ rename (const char* oldname, const char* newname);
int snprintf (char* buf, size_t size, const char* format, ...);
int sprintf (char* buf, const char* format, ...);
int __fastcall__ ungetc (int c, FILE* f);
int __fastcall__ vfprintf (FILE* f, const char* format, va_list ap);
int __fastcall__ vprintf (const char* format, va_list ap);
int __fastcall__ vsnprintf (char* buf, size_t size, const char* format, va_list ap);
int __fastcall__ vsprintf (char* buf, const char* format, va_list ap);

int scanf (const char* format, ...);
int fscanf (FILE* f, const char* format, ...);
int sscanf (const char* s, const char* format, ...);
int __fastcall__ vscanf (const char* format, va_list ap);
int __fastcall__ vsscanf (const char* s, const char* format, va_list ap);
int __fastcall__ vfscanf (FILE* f, const char* format, va_list ap);

#if __CC65_STD__ == __CC65_STD_CC65__
FILE* __fastcall__ fdopen (int fd, const char* mode);   /* Unix */
int __fastcall__ fileno (FILE* f);                      /* Unix */
#endif
void __fastcall__ _poserror (const char* msg);          /* cc65 */

/* Masking macros for some functions */
#define getc(f)         fgetc (f)               /* ANSI */
#define putc(c, f)      fputc (c, f)            /* ANSI */



/* End of stdio.h */
#endif
