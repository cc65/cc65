/*
 * putchar.c
 *
 * Ullrich von Bassewitz, 11.12.1998
 */



#include <stdio.h>

/* This is usually declared as a macro */
#undef putchar



int putchar (int c)
{
    return fputc (c, stdout);
}



