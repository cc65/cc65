/*
 * stdarg.h
 *
 * Ullrich von Bassewitz, 31.05.1998
 *
 */



#ifndef _STDARG_H
#define _STDARG_H



typedef unsigned char* va_list;

#define va_start(ap, fix)      	ap = (va_list)&fix + *(((va_list)&fix)-1) - __fixargs__
#define va_arg(ap,type)	       	((type)*(ap -= ((sizeof (type) + 1) & ~1)))
#define va_copy(dest, src)	((dest)=(src))
#define va_end(ap)

/* This is only valid *before* the first call to va_arg. It will also work
 * only for int sized parameters.
 */
#define va_fix(ap, offs)	*(ap+(__fixargs__-2*offs))



/* End of stdarg.h */
#endif




