/*
 * printf.c
 * 
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <stdarg.h>
#include <stdio.h>



int printf (char* format, ...)
{
    va_list ap;
    va_start (ap, format);

    /* Do formatting and output. Since we know, that va_end is empty, we don't
     * call it here, saving an extra variable and some code.
     */
    return vfprintf (stdout, (char*) va_fix (ap, 1), ap);
}



