/*
 * abort.c
 *
 * Maciej 'YTM/Elysium' Witkowiak 15.7.2001
 */



#include <stdlib.h>
#include <geos.h>

void abort (void)
{
    DlgBoxOk(CBOLDON "ABNORMAL PROGRAM", "TERMINATION." CPLAINTEXT);
    exit(3);
}
