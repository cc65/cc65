/*
  !!DESCRIPTION!! gets test
  !!LICENCE!!     Public domain
*/

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

char buf[512];

#define INFILE   "cf.in"

#ifndef __CC65__
/* Force declaration on host compiler, as gets() is deprecated for
 * being dangerous as hell */
char *gets (char *__s);
#endif

#ifdef NO_OLD_FUNC_DECL
int main(int argc,char **argv)
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
    /* Fake stdin with the reference file */
    if (freopen(INFILE, "rb", stdin) == NULL) {
        return EXIT_FAILURE;
    }

    while (gets(buf) != NULL)
    {
        printf("%s",buf);
    }

    fclose(stdin);
    return 0;
}
