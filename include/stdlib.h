/*****************************************************************************/
/*                                                                           */
/*                                 stdlib.h                                  */
/*                                                                           */
/*                             General utilities                             */
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



#ifndef _STDLIB_H
#define _STDLIB_H



/* size_t is needed */
#ifndef _HAVE_size_t
typedef unsigned size_t;
#define _HAVE_size_t
#endif

/* NULL pointer */
#ifndef NULL
#define NULL ((void *) 0)
#endif

/* Standard exit codes */
#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1

#if __CC65_STD__ == __CC65_STD_CC65__

/* Those non-standard cc65 exit constants definitions are in addition
** to the EXIT_SUCCESS and EXIT_FAILURE constants, which should not be
** redefined
*/
#define EXIT_ASSERT 2
#define EXIT_ABORT 3

#endif


/* Return type of the div function */
typedef struct {
    int rem;
    int quot;
} div_t;

/* Return type of the ldiv function (which currently doesn't exist) */
typedef struct {
    long rem;
    long quot;
} ldiv_t;

/* Memory management */
void* __fastcall__ malloc (size_t size);
void* __fastcall__ calloc (size_t count, size_t size);
void* __fastcall__ realloc (void* block, size_t size);
void __fastcall__ free (void* block);

/* Non standard memory management functions */

#if __CC65_STD__ == __CC65_STD_CC65__
int __fastcall__ posix_memalign (void** memptr, size_t alignment, size_t size);
/* Allocate a block of memory with the given "size", which is aligned to a
** memory address that is a multiple of "alignment".  "alignment" MUST NOT be
** zero, and MUST be a power of two; otherwise, this function will return
** EINVAL.  The function returns ENOMEM if not enough memory is available
** to satisfy the request.  "memptr" must point to a variable; that variable
** will return the address of the allocated memory.  Use free() to release that
** allocated block.
*/
#endif

void __fastcall__ __heapadd (void* mem, size_t size);
/* Add a block to the heap */
#if __CC65_STD__ == __CC65_STD_CC65__
/* define old name with one underscore for backwards compatibility */
#define _heapadd __heapadd
#endif

size_t __fastcall__ __heapblocksize (const void* block);
/* Return the size of an allocated block */
#if __CC65_STD__ == __CC65_STD_CC65__
/* define old name with one underscore for backwards compatibility */
#define _heapblocksize __heapblocksize
#endif

size_t __heapmemavail (void);
/* Return the total free heap space */
#if __CC65_STD__ == __CC65_STD_CC65__
/* define old name with one underscore for backwards compatibility */
#define _heapmemavail __heapmemavail
#endif

size_t __heapmaxavail (void);
/* Return the size of the largest free block on the heap */
#if __CC65_STD__ == __CC65_STD_CC65__
/* define old name with one underscore for backwards compatibility */
#define _heapmaxavail __heapmaxavail
#endif


/* Random numbers */
#define RAND_MAX        0x7FFF
int rand (void);
void __fastcall__ srand (unsigned seed);
void __randomize (void);         /* Non-standard */
#if __CC65_STD__ == __CC65_STD_CC65__
/* define old name with one underscore for backwards compatibility */
#define _randomize __randomize
#endif

/* Other standard stuff */
void abort (void) __attribute__ ((noreturn));
int __fastcall__ abs (int val);
long __fastcall__ labs (long val);
int __fastcall__ atoi (const char* s);
long __fastcall__ atol (const char* s);
int __fastcall__ atexit (void (*exitfunc) (void));
void* __fastcall__ bsearch (const void* key, const void* base, size_t n,
                            size_t size, int __fastcall__ (* cmp) (const void*, const void*));
div_t __fastcall__ div (int numer, int denom);
void __fastcall__ exit (int ret) __attribute__ ((noreturn));
char* __fastcall__ getenv (const char* name);
void __fastcall__ qsort (void* base, size_t count, size_t size,
                         int __fastcall__ (* compare) (const void*, const void*));
long __fastcall__ strtol (const char* nptr, char** endptr, int base);
unsigned long __fastcall__ strtoul (const char* nptr, char** endptr, int base);
int __fastcall__ system (const char* s);

/* Non-ANSI functions */
void __fastcall__ __swap (void* p, void* q, size_t size);
#if __CC65_STD__ == __CC65_STD_CC65__
/* define old name with one underscore for backwards compatibility */
#define _swap __swap
#endif
#if __CC65_STD__ == __CC65_STD_CC65__
char* __fastcall__ itoa (int val, char* buf, int radix);
char* __fastcall__ utoa (unsigned val, char* buf, int radix);
char* __fastcall__ ltoa (long val, char* buf, int radix);
char* __fastcall__ ultoa (unsigned long val, char* buf, int radix);
int __fastcall__ putenv (char* s);
#endif



/* End of stdlib.h */
#endif
