/*
 * vprintf.c
 *
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <stdarg.h>
#include <stdio.h>
#include "_printf.h"



int vprintf (const char* format, va_list ap)
{
    return vfprintf (stdout, format, ap);
}



