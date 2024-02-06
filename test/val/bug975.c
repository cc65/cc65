/* bug #975 - Forward array reference fails to compile */

#include <stdlib.h>

// this works
static const unsigned char array2[3];
int test2(void) {
    return array2[0];
}
static const unsigned char array2[] = { 0, 1, 2 };

// this should work, but does not compile
static const unsigned char array[];
int main() {
    if (test2() != 0) return EXIT_FAILURE;
    return array[0];
}
static const unsigned char array[] = { 0, 1, 2 };
