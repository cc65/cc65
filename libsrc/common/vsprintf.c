/*
 * vsprintf.c
 *
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <stdarg.h>
#include <stdio.h>
#include "_printf.h"



static void out (struct outdesc* d, char* buf, unsigned count)
/* Routine used for writing */
{
    /* String - be shure to check the size */
    while (count-- && d->ccount < d->uns) {
	((char*) d->ptr) [d->ccount] = *buf;
	++buf;
	++d->ccount;
    }
}



int vsprintf (char* buf, char* format, va_list ap)
{
    struct outdesc d;

    /* Setup descriptor */
    d.fout = out;
    d.ptr  = buf;
    d.uns  = 0x7FFF;

    /* Do formatting and output */
    _printf (&d, format, ap);

    /* Return bytes written */
    return d.ccount;
}



