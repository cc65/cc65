/* Bug #1408: Signed char type comparisons with unsigned numeric constants */

#include <stdio.h>

static int failures = 0;
static signed char x = -1;

int main(void)
{
    if (!(x > -2u)) {
        printf("x > -2u should be true\n");
        ++failures;
    }
    if (!(x > 0u)) {
        printf("x > 0u should be true\n");
        ++failures;
    }
    if (!(x > 255u)) {
        printf("x > 255u should be true\n");
        ++failures;
    }

    if (!(-2u < x)) {
        printf("-2u < x should be true\n");
        ++failures;
    }
    if (!(0u < x)) {
        printf("0u < x should be true\n");
        ++failures;
    }
    if (!(255u < x)) {
        printf("255u < x should be true\n");
        ++failures;
    }

    if (failures != 0) {
        printf("Failures: %d\n", failures);
    }

    return failures;
}
