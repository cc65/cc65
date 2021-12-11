/* bug #170 - Wrong implicit conversion of integers */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    uint8_t c = 2;
    uint32_t u = 2;
    int16_t a = -2;
    int32_t l = -2;

    /* Generated code should use tosmulax but uses tosumulax */
    int16_t r = c * a;
    /* Generated code should use tosmuleax but uses tosumuleax */
    int32_t lr = u * l;

    int32_t n = -95;
    uint16_t d = 3;
    int16_t r1 = n / d; // produces 21813 instead of -31

    int16_t r2 = n / (int32_t) d; // workaround

    printf("r: %d (-4)\n", r);
#ifdef REFERENCE
    printf("lr: %d (-4)\n", lr);
#else
    printf("lr: %ld (-4)\n", lr);
#endif
    printf("r1: %d (-31)\n", r1);
    printf("r2: %d (-31)\n", r2);

    if (r != -4) { return EXIT_FAILURE; }
    if (lr != -4) { return EXIT_FAILURE; }
    if (r1 != -31) { return EXIT_FAILURE; }
    if (r2 != -31) { return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}
