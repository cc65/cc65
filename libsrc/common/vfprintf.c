/*
 * vfprintf.c
 *
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <stdarg.h>
#include <stdio.h>
#include "_printf.h"



static void out (struct outdesc* d, const char* buf, unsigned count)
/* Routine used for writing */
{
    /* Write to the file */
    if (fwrite (buf, count, 1, (FILE*) d->ptr) == -1) {
 	d->ccount = -1;
    } else {
 	d->ccount += count;
    }
}



int vfprintf (FILE* f, const char* format, va_list ap)
{
    struct outdesc d;

    /* Setup descriptor */
    d.fout = out;
    d.ptr  = f;

    /* Do formatting and output */
    _printf (&d, format, ap);

    /* Return bytes written */
    return d.ccount;
}




