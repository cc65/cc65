
/* bug #1244 - ICE for enum bit-fields */

#include <stdlib.h>

enum E {
    L = 65535L  /* or U = 65535U */
};

struct S {
    enum E a : 16;
} s;

int main(void)
{
    return EXIT_SUCCESS;
}

