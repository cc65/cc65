/*****************************************************************************/
/*                                                                           */
/*                                 string.h                                  */
/*                                                                           */
/*                              String handling                              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2014, Ullrich von Bassewitz                                      */
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



#ifndef _STRING_H
#define _STRING_H



#include <stddef.h>



char* __fastcall__ strcat (char* dest, const char* src);
char* __fastcall__ strchr (const char* s, int c);
int __fastcall__ strcmp (const char* s1, const char* s2);
int __fastcall__ strcoll (const char* s1, const char* s2);
char* __fastcall__ strcpy (char* dest, const char* src);
size_t __fastcall__ strcspn (const char* s1, const char* s2);
char* __fastcall__ strerror (int errcode);
size_t __fastcall__ strlen (const char* s);
char* __fastcall__ strncat (char* s1, const char* s2, size_t count);
int __fastcall__ strncmp (const char* s1, const char* s2, size_t count);
char* __fastcall__ strncpy (char* dest, const char* src, size_t count);
char* __fastcall__ strpbrk (const char* str, const char* set);
char* __fastcall__ strrchr (const char* s, int c);
size_t __fastcall__ strspn (const char* s1, const char* s2);
char* __fastcall__ strstr (const char* str, const char* substr);
char* __fastcall__ strtok (char* s1, const char* s2);
size_t __fastcall__ strxfrm (char* s1, const char* s2, size_t count);
void* __fastcall__ memchr (const void* mem, int c, size_t count);
int __fastcall__ memcmp (const void* p1, const void* p2, size_t count);
void* __fastcall__ memcpy (void* dest, const void* src, size_t count);
void* __fastcall__ memmove (void* dest, const void* src, size_t count);
void* __fastcall__ memset (void* s, int c, size_t count);

/* The following is an internal function, the compiler will replace memset
** with it if the fill value is zero. Never use this one directly!
*/
void* __fastcall__ _bzero (void* ptr, size_t n);

/* Non standard: */
#if __CC65_STD__ == __CC65_STD_CC65__
void __fastcall__ bzero (void* ptr, size_t n);                /* BSD */
char* __fastcall__ strdup (const char* s);                    /* SYSV/BSD */
int __fastcall__ stricmp (const char* s1, const char* s2);    /* DOS/Windows */
int __fastcall__ strcasecmp (const char* s1, const char* s2); /* Same for Unix */
int __fastcall__ strnicmp (const char* s1, const char* s2, size_t count);     /* DOS/Windows */
int __fastcall__ strncasecmp (const char* s1, const char* s2, size_t count);  /* Same for Unix */
char* __fastcall__ strlwr (char* s);
char* __fastcall__ strlower (char* s);
char* __fastcall__ strupr (char* s);
char* __fastcall__ strupper (char* s);
char* __fastcall__ strqtok (char* s1, const char* s2);
#endif

const char* __fastcall__ _stroserror (unsigned char errcode);
/* Map an operating system error number to an error message. */



/* End of string.h */
#endif
