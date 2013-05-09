#include <stdio.h>
#include <stdlib.h>

extern const char text[];       /* In text.s */

int main (void)
{
    printf ("%s\n", text);
    return EXIT_SUCCESS;
}


