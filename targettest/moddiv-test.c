#include <stdlib.h>
#include <stdio.h>

int main (void)
{
    int a, b;
    int div, mod;

    printf ("Please note that this program does an\n"
            "exhaustive test for the division and\n"
            "modulo operation and therefore runs for\n"
            "almost ever. On my box, it's nearly two\n"
            "days in warp mode of VICE.\n\n");

    a = 0;
    do {
        b = 1;
        do {
            div = a / b;
            mod = a % b;
            if (div * b + mod != a) {
                printf ("Found problems:\n"
                        "  Result of %u / %u is %u\n"
                        "  Result of %u %% %u is %u\n",
                        a, b, div, a, b, mod);
                return EXIT_FAILURE;
            }
            ++b;
        } while (b != 0);
        if ((a & 0xFF) == 0) {
            printf ("%5u ", a);
        }
        ++a;
    } while (a != 0);
    return EXIT_SUCCESS;
}
