/*
 * vcprintf.c
 *
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <stdarg.h>
#include <conio.h>
#include "_printf.h"



static void out (struct outdesc* d, char* buf, unsigned count)
/* Routine used for writing */
{
    /* Fast screen output */
    d->ccount += count;
    while (count) {
    	cputc (*buf);
    	++buf;
        --count;
    }
}



int vcprintf (const char* format, va_list ap)
{
    struct outdesc d;

    /* Setup descriptor */
    d.fout = out;

    /* Do formatting and output */
    _printf (&d, format, ap);

    /* Return bytes written */
    return d.ccount;
}



