#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char S1[] = {
    'h', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0', 'A'
};

static const char S2[] = {
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\0', 'B'
};




int main (void)
{
    char I;
    for (I = 0; I < 20; ++I) {
        printf ("%02d: %d\n", I, strncmp (S1, S2, I));
    }
    return 0;
}

