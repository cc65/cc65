/* bug 1888 - cc65 fails with storage class specifiers after type specifiers */

#include <stdio.h>

int const typedef volatile x_type, * const volatile y_type;

int static failures = 0;

int extern main(void);

int main(void)
{
    volatile static x_type const x = 42, * const volatile y[] = { 1 ? &x : (y_type)0 };
    if (**y != 42) {
        ++failures;
        printf("y = %d, Expected: 42\n", **y);
    }
    return failures;
}
