/* bug#760 - Error when using macros as pragma arguments */

#include <stdlib.h>

#define BANK "PRG0"

#pragma rodata-name(push, BANK)

int main(void)
{
    return EXIT_SUCCESS;
}
