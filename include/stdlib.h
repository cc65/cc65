/*
 * stdlib.h
 *
 * Ullrich von Bassewitz, 02.06.1998
 *
 */



#ifndef _STDLIB_H
#define _STDLIB_H



#include <stddef.h>



/* Standard exit codes */
#define EXIT_SUCCESS	0
#define EXIT_FAILURE	1



/* Memory management */
void* __fastcall__ malloc (size_t size);
void* calloc (size_t count, size_t size);
void* realloc (void* block, size_t size);
void __fastcall__ free (void* block);
#ifndef __STRICT_ANSI__
void __fastcall__ _hadd (void* mem, size_t size);	/* Non-standard */
#endif

/* Random numbers */
#define	RAND_MAX	0x7FFF
int rand (void);
void __fastcall__ srand (unsigned seed);

/* Other standard stuff */
void abort (void);
int __fastcall__ abs (int val);
long __fastcall__ labs (long val);
int __fastcall__ atoi (char* s);
long __fastcall__ atol (char* s);
int __fastcall__ atexit (void (*exitfunc) (void));
void* bsearch (const void* key, const void* base, size_t n,
	       size_t size, int (*cmp) (const void*, const void*));
void exit (int ret);
char* __fastcall__ getenv (const char* name);
void qsort (void* base, size_t count, size_t size,
	    int (*compare) (const void*, const void*));

/* Non-ANSI functions */
#ifndef __STRICT_ANSI__
void __fastcall__ _swap (void* p, void* q, size_t size);
char* __fastcall__ itoa (int val, char* buf, int radix);
char* __fastcall__ utoa (unsigned val, char* buf, int radix);
char* __fastcall__ ltoa (long val, char* buf, int radix);
char* __fastcall__ ultoa (unsigned long val, char* buf, int radix);
#endif



/* End of stdlib.h */
#endif



