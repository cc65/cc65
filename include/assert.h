/*
 * assert.h
 *
 * Ullrich von Bassewitz, 06.06.1998
 *
 */



#ifndef _ASSERT_H
#define _ASSERT_H



#undef assert
#ifdef NDEBUG
#  define assert(expr)
#else
extern void _afailed (const char*, unsigned);
#  define assert(expr)	if ((expr) == 0) _afailed (__FILE__, __LINE__)
#endif



/* End of assert.h */
#endif



