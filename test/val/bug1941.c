/* bug #1941 - Shifts by literal 0 bits are broken */

#include <stdlib.h>
#include <stdint.h>

uint8_t foo=1U;
uint8_t bar=3U; // You need it to reproduce the issue

_Static_assert (sizeof (foo >> 0) == sizeof (int), "Result type of (foo >> 0) should be int");

int main(void)
{
    return(foo == foo << 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
