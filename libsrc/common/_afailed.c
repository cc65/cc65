/*
 * _afailed.c
 *
 * Ullrich von Bassewitz, 06.06.1998
 */



#include <stdio.h>
#include <stdlib.h>



void _afailed (char* file, unsigned line)
{
    fprintf (stderr, "ASSERTION FAILED IN %s(%u)\n", file, line);
    exit (2);
}



