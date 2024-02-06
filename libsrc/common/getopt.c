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
#define EMSG     ""

int opterr = 1;                 /* useless, never set or used */
int optind = 1;                 /* index into parent argv vector */
int optopt;                     /* character checked for validity */

char *optarg;                   /* argument associated with option */

#define tell(s) fputs(*argv,stderr);fputs(s,stderr); \
                fputc(optopt,stderr);fputc('\n',stderr);return(BADCH);

int __fastcall__ getopt (int argc, char* const* argv, const char* optstring)
/* Get option letter from argument vector */
{
    static char *place = EMSG;  /* option letter processing */

    register char *oli;         /* option letter list index */

    if (!*place) {              /* update scanning pointer */
        if (optind >= argc || *(place = argv[optind]) != '-' || !*++place) {
            return (EOF);
        }
        if (*place == '-') {
            /* found "--" */
            ++optind;
            return (EOF);
        }
    }

    /* option letter okay? */
    if ((optopt = (int) *place++) == ARGCH ||
        !(oli = strchr (optstring, optopt))) {
        if (!*place) {
            ++optind;
        }
        tell (": illegal option -- ");
    }
    if (*++oli != ARGCH) {
        /* don't need argument */
        optarg = NULL;
        if (!*place) {
            ++optind;
        }
    } else {
        /* need an argument */
        if (*place) {
            /* no white space */
            optarg = place;
        }
        else if (argc <= ++optind) {   /* no arg */
            place = EMSG;
            tell (": option requires an argument -- ");
        } else {
            /* white space */
            optarg = argv[optind];
        }
        place = EMSG;
        ++optind;
    }
    return (optopt);            /* dump back option letter */
}

