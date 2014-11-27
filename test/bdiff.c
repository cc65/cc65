
// minimal tool to compare two binaries

#include <stdlib.h>
#include <stdio.h>

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
        if (fgetc(f1) != fgetc(f2)) {
            return EXIT_FAILURE;
        }
    }
}
