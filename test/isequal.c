
// minimal tool to compare two text files

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ARRAYSIZE(_x) (sizeof _x / sizeof _x[0])

typedef int wildcardfunc(FILE * f2);

struct wildcardtype {
    char         * name;
    wildcardfunc * func;
};


static int wildcard_path(FILE * f1);
static int wildcard_integer(FILE * f1);

struct wildcardtype wildcards[] = {
    { "PATH", wildcard_path },
    { "INTEGER", wildcard_integer }
};

static wildcardfunc * currentwildcardfunc = 0;

static int binary = 0;
static int empty = 0;
static int skiplines_left = 0;
static int skiplines_right = 0;
static int use_wildcards = 0;
static char * filename_left = 0;
static char * filename_right = 0;

/* LOOKAHEADBUFFERSIZE must be a power of 2, because the wrap-around for the
   indices requires this!
*/
#define LOOKAHEADBUFFERSIZE 0x80
#define WILDCARDCHAR_OPEN     '<'
#define WILDCARDCHAR_INTERNAL '#'
#define WILDCARDCHAR_CLOSE    '>'

#define WILDCARDCHAR_OPEN_CLOSE_COUNT 3
#define WILDCARDNAME_MAXLENGTH 20

static char lookaheadbuffer[LOOKAHEADBUFFERSIZE] = { 0 };
static int  lookaheadindexread = 0;
static int  lookaheadindexwrite = 0;

static int  wildcardendchar;

wildcardfunc * findwildcardfunc(char * wildcardname)
{
    wildcardfunc * func = NULL;

    unsigned int i;

    for (i = 0; i < ARRAYSIZE(wildcards); ++i) {
        if (strcmp(wildcards[i].name, wildcardname) == 0) {
            func = wildcards[i].func;
        }
    }

    return func;
}

static int handleargparameter(int offset, char * parameter)
{
    long number = -1;
    char * endptr = NULL;

    if (parameter[offset++] != '=') {
        return -1;
    }

    number = strtol(parameter + offset, &endptr, 10);

    if (endptr == NULL || *endptr != 0) {
        return -1;

    }
    return number;
}

static int handleparameter(int argc, char *argv[])
{
    static const char opt_binary[]    = "--binary";
    static const char opt_empty[]     = "--empty";
    static const char opt_skipleft[]  = "--skipleft";
    static const char opt_skipright[] = "--skipright";
    static const char opt_skip[]      = "--skip";
    static const char opt_wildcards[] = "--wildcards";

    static const char len_skipleft    = sizeof opt_skipleft - 1;
    static const char len_skipright   = sizeof opt_skipright - 1;
    static const char len_skip        = sizeof opt_skip - 1;

    int argindex = 1;

    if (argc < 2) {
        return -1;
    }

    while (argindex < argc && argv[argindex] && argv[argindex][0] == '-') {
        if (strcmp(argv[argindex], opt_binary) == 0) {
            if (empty || skiplines_left || skiplines_right) {
                fprintf(stderr, "--empty cannot go with other options.\n");
                exit(1);
            }
            binary = 1;
        }
        else if (strcmp(argv[argindex], opt_empty) == 0) {
            if (binary || skiplines_left || skiplines_right || use_wildcards) {
                fprintf(stderr, "--binary cannot go with other options.\n");
                exit(1);
            }
            empty = 1;
        }
        else if (strncmp(argv[argindex], opt_skipleft, len_skipleft) == 0) {
            if (binary || empty) {
                fprintf(stderr, "%s cannot go with other options.\n", opt_skipleft);
                exit(1);
            }
            skiplines_left = handleargparameter(len_skipleft, argv[argindex]);
            if (skiplines_left < 0) {
                fprintf(stderr, "%s: you must specify the number of lines\n", opt_skipleft);
            }
        }
        else if (strncmp(argv[argindex], opt_skipright, len_skipright) == 0) {
            if (binary || empty) {
                fprintf(stderr, "%s cannot go with other options.\n", opt_skipright);
                exit(1);
            }
            skiplines_right = handleargparameter(len_skipright, argv[argindex]);
            if (skiplines_right < 0) {
                fprintf(stderr, "%s: you must specify the number of lines\n", opt_skipright);
            }
        }
        else if (strncmp(argv[argindex], opt_skip, len_skip) == 0) {
            if (binary || empty) {
                fprintf(stderr, "%s cannot go with other options.\n", opt_skipright);
                exit(1);
            }
            skiplines_left = skiplines_right = handleargparameter(len_skip, argv[argindex]);
            if (skiplines_left < 0) {
                fprintf(stderr, "%s: you must specify the number of lines\n", opt_skip);
            }
        }
        else if (strcmp(argv[argindex], opt_wildcards) == 0) {
            if (binary) {
                fprintf(stderr, "--wildcards cannot go with --binary.\n");
                exit(1);
            }
            use_wildcards = 1;
        }
        ++argindex;
    }

    if (argc + empty - argindex != 2) {
        fprintf(stderr, "filenames are missing!\n");
        return -1;
    }

    filename_left  = argv[argindex++];
    filename_right = argv[argindex++];

    return 0;
}

/* get the next character from FILE and convert commonly used line-endings all
   into the same value (0x0a, as used on *nix systems)

   recognized values/pairs:

   0x0a (LF)                Linux, macOS
   0x0d, 0x0a (CR, LF)      Windows, MSDOS, OS/2
   0x0d (CR)                classic MacOS
*/

static int getnext(FILE *f)
{
    int c = fgetc(f);
    if (!binary && c == 0x0d) {
        if (!feof(f)) {
            int n = fgetc(f);
            if (n != 0x0a) {
                ungetc(n, f);
            }
            clearerr(f); /* clears EOF when we did not push back */
        }
        return 0x0a;
    }
    return c;
}

static void skiplines(FILE *f, int skipcount)
{
   int c;

   while (skipcount > 0) {
       c = getnext(f);
       if (feof(f)) {
           return;
       }

       if (c == 0x0a) {
           --skipcount;
       }
   }
}

static int comparefiles(FILE *f1, FILE *f2)
{
    for(;;) {
        if (feof(f1) && feof(f2)) {
            return EXIT_SUCCESS;
        } else if (feof(f1) || feof(f2)) {
            return EXIT_FAILURE;
        }
        if (getnext(f1) != getnext(f2)) {
            return EXIT_FAILURE;
        }
    }
}

static int lookaheadbufferisempty(void)
{
    return lookaheadindexread == lookaheadindexwrite;
}

static char lookaheadbufferread(void)
{
    char ch;

    if (lookaheadbufferisempty()) {
        fprintf(stderr, "### want to take from lookahead buffer, but it is empty --> aborting!\n");
        exit(EXIT_FAILURE);
    }

    ch = lookaheadbuffer[lookaheadindexread];

    /* advance read pointer, with wrap-around */
    lookaheadindexread = (lookaheadindexread + 1) & (sizeof lookaheadbuffer - 1);

    return ch;
}

static void lookaheadbufferwrite(char ch)
{
    lookaheadbuffer[lookaheadindexwrite] = ch;

    /* advance write pointer, with wrap-around */
    lookaheadindexwrite = (lookaheadindexwrite + 1) & (sizeof lookaheadbuffer - 1);

    if (lookaheadbufferisempty()) {
        fprintf(stderr, "### lookahead buffer ovrrun, aborting!\n");
        exit(EXIT_FAILURE);
    }
}

static int processwildcardchar(FILE *f1)
{
    int countwildcardchar = 1;
    int foundwildcard = 0;
    int ch;

    static char wildcardname[WILDCARDNAME_MAXLENGTH + 1];
    unsigned int wildcardnamenextindex;

    while ((ch = getnext(f1)) == WILDCARDCHAR_OPEN) {
        ++countwildcardchar;
        if (feof(f1)) {
            break;
        }
    }

    if (countwildcardchar == WILDCARDCHAR_OPEN_CLOSE_COUNT) {
        if (ch == WILDCARDCHAR_INTERNAL) {
            /* we found a wildcard! */
            foundwildcard = 1;
        }
    }
    else {
        if (ch == WILDCARDCHAR_OPEN) {
            ch = -1;
        }
    }

    if (foundwildcard) {
        char c;

        /* we found a wildcard init sequence; now check which wildcard it is */

        wildcardnamenextindex = 0;
        wildcardname[WILDCARDNAME_MAXLENGTH] = 0;

        c = getnext(f1);
        while (c != WILDCARDCHAR_INTERNAL && c != 0x0a && !feof(f1)) {
            wildcardname[wildcardnamenextindex] = c;
            if (++wildcardnamenextindex >= WILDCARDNAME_MAXLENGTH) {
                wildcardname[WILDCARDNAME_MAXLENGTH] = 0;
                fprintf(stderr, "wildcard '%s' is too long!\n", wildcardname);
                exit(EXIT_FAILURE);
            }
            c = getnext(f1);
        }

        wildcardname[wildcardnamenextindex] = 0;

        if (c == WILDCARDCHAR_INTERNAL) {
            int countwildcardcharclose = 0;

            fprintf(stderr, "Found wildcard '%s'\n", wildcardname);

            while ((c = getnext(f1)) == WILDCARDCHAR_CLOSE && !feof(f1)) {
                ++countwildcardcharclose;
            }

            wildcardendchar = c; /* remember next char */

            currentwildcardfunc = findwildcardfunc(wildcardname);
            if (currentwildcardfunc == NULL) {
                fprintf(stderr, "Wildcard '%s' is unknown!\n", wildcardname);
                exit(EXIT_FAILURE);
            }
        }
        else {
            fprintf(stderr, "Thought I found wildcard '%s', but it does not end.\n", wildcardname);
        }
    }

    if (!foundwildcard) {
        int i;

        /* no wildcard; restore the chars */
        for (i = 0; i < countwildcardchar; ++i) {
            lookaheadbufferwrite(WILDCARDCHAR_OPEN);
        }

        if (ch >= 0) {
            lookaheadbufferwrite(ch);
        }
    }

    return foundwildcard;
}

static int wildcard_path(FILE * f2)
{
    int isstillwildcard = 1;

    int ch = getnext(f2);

    if ((ch == wildcardendchar) || ch < ' ' || ch > 126) {
        /* this is not a path char anymore, abort the wildcard processing */
        isstillwildcard = 0;

        if (ch != wildcardendchar) {
            exit(EXIT_FAILURE);
        }
    }

    return isstillwildcard;
}

static int wildcard_integer(FILE * f2)
{
    int isstillwildcard = 1;
    int ch = getnext(f2);

    if ((ch == wildcardendchar) || ch < '0' || ch > '9') {
        /* this is not a digit anymore, abort the wildcard processing */
        isstillwildcard = 0;

        if (ch != wildcardendchar) {
            exit(EXIT_FAILURE);
        }
    }

    return isstillwildcard;
}

static int comparefileswithwildcards(FILE *f1, FILE *f2)
{
    static int iswildcard = 0;

    for(;;) {
        int c1;

        if (lookaheadbufferisempty() && !iswildcard && feof(f1) && feof(f2)) {
            return EXIT_SUCCESS;
        } else if (((lookaheadbufferisempty() && !iswildcard && feof(f1))) || feof(f2)) {
            return EXIT_FAILURE;
        }

        if (iswildcard) {
            /* f1 has a wildcard; process that */
            iswildcard = currentwildcardfunc(f2);
        }
        else {
            /* f1 does not have a wildcard; process the next char (unless it starts a wildcard) */
            if (lookaheadbufferisempty()) {
                c1 = getnext(f1);
                if (c1 == WILDCARDCHAR_OPEN) {
                    iswildcard = processwildcardchar(f1);
                    continue;
                }
            }
            else {
                c1 = lookaheadbufferread();
            }
            if (c1 != getnext(f2)) {
                return EXIT_FAILURE;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *f1, *f2;

    if (handleparameter(argc, argv) < 0) {
        return EXIT_FAILURE;
    }

    f1 = fopen(filename_left, "rb");
    if (f1 == NULL) {
        return EXIT_FAILURE;
    }

    if (empty) {
        fseek(f1, 0, SEEK_END);
        if (ftell(f1) != 0) {
            return EXIT_FAILURE;
        }
        else {
            return EXIT_SUCCESS;
        }
    }
    else {
        if (skiplines_left) {
            skiplines(f1, skiplines_left);
        }

        f2 = fopen(filename_right, "rb");
        if (f2 == NULL) {
            return EXIT_FAILURE;
        }
        if (skiplines_right) {
            skiplines(f2, skiplines_right);
        }
        if (use_wildcards) {
            return comparefileswithwildcards(f1, f2);
        }
        else {
            return comparefiles(f1, f2);
        }
    }
}
