/*
** This is part of a changed public domain getopt implementation that
** had the following text on top:
**
**      I got this off net.sources from Henry Spencer.
**      It is a public domain getopt(3) like in System V.
**      I have made the following modifications:
**
**      A test main program was added, ifdeffed by GETOPT.
**      This main program is a public domain implementation
**      of the getopt(1) program like in System V.  The getopt
**      program can be used to standardize shell option handling.
**              e.g.  cc -DGETOPT getopt.c -o getopt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARGCH    ':'
#define BADCH    '?'
#define ENDARGS  "--"

int main (int argc, char **argv)
{
    char *optstring = argv[1];

    char *argv0 = argv[0];

    int opterr = 0;

    int C;

    char *opi;

    if (argc == 1) {
        fprintf (stderr, "Usage: %s optstring args\n", argv0);
        exit (1);
    }
    argv++;
    argc--;
    argv[0] = argv0;
    while ((C = getopt (argc, argv, optstring)) != EOF) {
        if (C == BADCH)
            opterr++;
        printf ("-%c ", C);
        opi = strchr (optstring, C);
        if (opi && opi[1] == ARGCH)
            if (optarg)
                printf ("\"%s\" ", optarg);
            else
                opterr++;
    }
    printf ("%s", ENDARGS);
    while (optind < argc)
        printf (" \"%s\"", argv[optind++]);
    putchar ('\n');
    return opterr;
}

