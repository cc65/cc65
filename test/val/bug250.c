/* bug #250 - Array size compile-time optimization stops halfway */

#include <stdlib.h>

#define LZO_MAX(a,b)        ((a) >= (b) ? (a) : (b))
unsigned char c[2*4];
unsigned char b[2*LZO_MAX(8,sizeof(int))];  // this will not compile

int main(void)
{
    /* FIXME: add some runtime check */
    return EXIT_SUCCESS;
}
