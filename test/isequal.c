
// minimal tool to compare two text files

#include <stdlib.h>
#include <stdio.h>

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
    if (c == 0x0d) {
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

int main(int argc, char *argv[])
{
    FILE *f1, *f2;
    if (argc < 3) {
        return EXIT_FAILURE;
    }
    f1 = fopen(argv[1], "rb");
    f2 = fopen(argv[2], "rb");
    if ((f1 == NULL) || (f2 == NULL)) {
        return EXIT_FAILURE;
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
