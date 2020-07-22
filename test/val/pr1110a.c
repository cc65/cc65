
/* pr #1110 - the part of the redefinition that should compile */

static const unsigned char array[3];                /* OK */
static const unsigned char array[] = { 0, 1, 2 };   /* OK - complete definition*/
static const unsigned char array[3];                /* OK */
static const unsigned char array[];                 /* OK */

#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    printf("%u %u %u\n", array[0], array[1], array[2]);
    if ((array[0] != 0) || (array[1] != 1) || (array[2] != 2)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
