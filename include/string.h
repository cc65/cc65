/*
 * string.h
 *
 * Ullrich von Bassewitz, 04.06.1998
 *
 */



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
char* __fastcall__ strrchr (const char* s, int c);
size_t __fastcall__ strspn (const char* s1, const char* s2);
char* __fastcall__ strstr (const char* str, const char* substr);
char* strtok (char* s1, const char* s2);
size_t strxfrm (char* s1, const char* s2, size_t count);
void* __fastcall__ memchr (const void* mem, int c, size_t count);
int __fastcall__ memcmp (const void* p1, const void* p2, size_t count);
void* __fastcall__ memcpy (void* dest, const void* src, size_t count);
void* __fastcall__ memmove (void* dest, const void* src, size_t count);
void* __fastcall__ memset (void* s, int c, size_t count);

/* Non standard: */
#ifndef __STRICT_ANSI__
char* strdup (const char* s);				      /* SYSV/BSD */
int __fastcall__ stricmp (const char* s1, const char* s2);    /* DOS/Windows */
int __fastcall__ strcasecmp (const char* s1, const char* s2); /* Same for Unix */
char* __fastcall__ strlwr (char* s);
char* __fastcall__ strlower (char* s);
char* __fastcall__ strupr (char* s);
char* __fastcall__ strupper (char* s);
#endif



/* End of string.h */
#endif



