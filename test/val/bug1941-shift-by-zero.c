/* Bug 1941 - Bitwise shift char types by 0 count results in out-of-range access */

#include <stdio.h>
#include <stdint.h>

uint8_t foo = 42U;      /* "Low byte" */
uint8_t goo = 1U;       /* "High byte" - you need it to reproduce the issue */
int16_t bar = 256;      /* ...or just do it with this */

_Static_assert (sizeof (foo >> 0) == sizeof (int), "Shift result should be int-promoted");
_Static_assert (sizeof ((int8_t)bar << 0) == sizeof (int), "Shift result should be int-promoted");

unsigned failures;

int main(void)
{
    if (foo >> 0 != foo) {
        ++failures;
        printf("foo failed\n");
    }

    if ((int8_t)bar << 0 != (int8_t)bar) {
        ++failures;
        printf("bar failed\n");
    }

    return failures;
}
