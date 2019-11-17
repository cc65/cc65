/*
** abort.c
**
** Maciej 'YTM/Elysium' Witkowiak 15.7.2001
*/

#include <stdlib.h>
#include <geos.h>
#include <cc65.h>

void abort (void)
{
    ExitTurbo();
    DlgBoxOk(CBOLDON "ABNORMAL PROGRAM", "TERMINATION." CPLAINTEXT);
    exit(EXIT_ABORT);
}
