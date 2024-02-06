/* bug #1245 - ICE for enums with int initializers */

#include <stdlib.h>

enum E {
    X = 1000,
} e = 3;

int main(void)
{
    return EXIT_SUCCESS;
}
