/* OptCmp1 messed up with labels */

#include <stdio.h>

static int failures = 0;
static unsigned int z = 0xFF23;

int main(void)
{
    register unsigned int x = 0x200;
    register unsigned int y = 0;

    do {
        ++y;
    } while (--x);
    if (y != 0x200) {
        printf("y should be 0x200, not 0x%X.\n", y);
        ++failures;;
    }

    if ((z -= 0x23)) {
        /* Passed -- non-zero z looks like non-zero. */
    } else {
        /* Failed -- only the low byte of z was tested. */
        printf("Test thinks non-zero z is zero.\n");
        ++failures;
    }

    return failures;
}
