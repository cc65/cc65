/* bug #1941 - Shift by literal 0 bits are broken */

#include <stdlib.h>
#include <stdint.h>

uint8_t foo=1U;
uint8_t bar=3U; // You need it to reproduce the issue

int main(void)
{
    return(foo == foo << 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

