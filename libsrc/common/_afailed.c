/*
** _afailed.c
**
** 1998-06-06, Ullrich von Bassewitz
** 2019-11-10, Greg King
*/



#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <cc65.h>


void __fastcall__ _afailed (char* file, unsigned line)
{
    raise (SIGABRT);
    fprintf (stderr, "ASSERTION FAILED IN %s(%u)\n", file, line);
    exit (CC65_EXIT_AFAILED);
}
