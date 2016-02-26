/*
** _afailed.c
**
** 1998-06-06, Ullrich von Bassewitz
** 2015-03-13, Greg King
*/



#include <stdio.h>
#include <stdlib.h>



void __fastcall__ _afailed (char* file, unsigned line)
{
    fprintf (stderr, "ASSERTION FAILED IN %s(%u)\n", file, line);
    exit (2);
}



