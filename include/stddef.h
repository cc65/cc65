/*
 * stddef.h
 *
 * Ullrich von Bassewitz, 06.06.1998
 *
 */



#ifndef _STDDEF_H
#define _STDDEF_H



/* Standard data types */
typedef int ptrdiff_t;
typedef unsigned size_t;

/* NULL pointer */
#ifdef NULL
#  undef NULL
#endif
#define NULL	0

/* offsetof macro */
#define offsetof(type, member)	(size_t) (&((type*) 0)->member)



/* End of stddef.h */
#endif



