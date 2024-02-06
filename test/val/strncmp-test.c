#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char S1[] = {
    'h', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0', 'A'
};

static const char S2[] = {
    'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\0', 'B'
};

int fails = 0;

int main (void)
{
    char I;
    int ret;
    for (I = 0; I < 20; ++I) {
        ret = strncmp (S1, S2, I);
        printf ("%02d: %d\n", I, ret);
        if ((ret != 0) && (I < 7)) {
            fails++;
        }
    }
    printf("fails: %d\n", fails);
    return fails;
}
