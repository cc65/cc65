/*
** abort.c
**
** Ullrich von Bassewitz, 02.06.1998
*/



#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>


void abort (void)
{
    raise (SIGABRT);
    fputs ("ABNORMAL PROGRAM TERMINATION\n", stderr);
    exit (EXIT_ABORT);
}



