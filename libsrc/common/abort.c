/*
 * abort.c
 *
 * Ullrich von Bassewitz, 02.06.1998
 */



#include <stdio.h>
#include <stdlib.h>



void abort (void)
{
    fputs ("ABNORMAL PROGRAM TERMINATION\n", stderr);
    exit (3);
}



