
// minimal tool to compare two text files

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int binary = 0;
static int empty = 0;
static int skiplines_left = 0;
static int skiplines_right = 0;
static char * filename_left = 0;
static char * filename_right = 0;

int handleargparameter(int offset, char * parameter)
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

int handleparameter(int argc, char *argv[])
{
    static const char opt_binary[]    = "--binary";
    static const char opt_empty[]     = "--empty";
    static const char opt_skipleft[]  = "--skipleft";
    static const char opt_skipright[] = "--skipright";

    static const char len_skipleft    = sizeof opt_skipleft - 1;
    static const char len_skipright   = sizeof opt_skipright - 1;

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
            if (binary || skiplines_left || skiplines_right) {
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

int getnext(FILE *f)
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

void skiplines(FILE *f, int skipcount)
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
}
