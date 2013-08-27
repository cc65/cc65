#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{
    int I;

    printf ("argc:     %d\n", argc);
    for (I = 0; I < argc; ++I) {
        printf ("argv[%2d]: \"%s\"\n", I, argv[I]);
    }

    printf ("\n");
    getchar ();
    return EXIT_SUCCESS;
}



