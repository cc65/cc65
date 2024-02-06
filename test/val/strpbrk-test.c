#include <stdio.h>
#include <string.h>

static const char fox[] = "The quick brown fox jumped over the lazy dogs.";

int fails = 0;

int main (void)
{
    printf ("Testing strpbrk():\n");
    if (strpbrk (fox, "qwerty") != &fox[2]) {
        printf ("\nThe first 'e' wasn't found.\n");
        fails++;
    }
    if (strpbrk (fox, "QWERTY") != &fox[0]) {
        printf ("The 'T' wasn't found.\n");
        fails++;
    }
    if (strpbrk (fox, "asdfg") != &fox[16]) {
        printf ("The 'f' wasn't found.\n");
        fails++;
    }
    if (strpbrk (fox, "nxv,zmb") != &fox[10]) {
        printf ("The 'b'  wasn't found.\n");
        fails++;
    }
    if (strpbrk (fox, "!@#$%^&*()-+=[];:',/?<>.") != &fox[45]) {
        printf ("The '.' wasn't found.\n");
        fails++;
    }

    printf ("\nFinished. fails = %d\n", fails);
    return fails;
}
