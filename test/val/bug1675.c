/* #1675 - Some corner cases of bit-shifts */

#include <stdio.h>

int failures = 0;

void Test_Defined(void)
{
    {
        /* Well-defined per standard, lhs expected in cc65: (int)-256 */
        if (!(((signed char)0x80 << 1) < 0)) {
            ++failures;
            printf("Expected: ((signed char)0x80 << 1) < 0, got lhs: %ld\n", (long)((signed char)0x80 << 1));
        }
    }

    {
        /* Implementation-defined per standard, lhs expected in cc65: (int)-128 */
        if (!(((signed char)0x80 >> 1 << 1) < 0)) {
            ++failures;
            printf("Expected: ((signed char)0x80 >> 1 << 1) < 0, got lhs: %ld\n", (long)((signed char)0x80 >> 1 << 1));
        }
    }

    {
        int x = 0;
        /* Well-defined per standard, lhs expected in cc65: (int)1 */
        if (!((1 << (x++, 0)) == 1)) {
            ++failures;
            x = 0;
            printf("Expected: (1 << (x++, 0)) == 1, got lhs: %ld\n", (long)(1 << (x++, 0)));
        }

        /* Well-defined per standard, lhs expected in cc65: (int)1 */
        if (!(x == 1)) {
            ++failures;
            printf("Expected: (1 << (x++, 0)) == 1 && x == 1, got x: %d\n", x);
        }
    }

    {
        int x = 0, y = 0x100;
        /* Well-defined per standard, lhs expected in cc65: (int)128 */
        if (!((y >> (x++, 0) >> 1) == 0x80)) {
            ++failures;
            x = 0;
            printf("Expected: (y >> (x++, 0) >> 1) == 0x80, got lhs: %ld\n", (long)(y >> (x++, 0) >> 1));
        }

        /* Well-defined per standard, lhs expected in cc65: (int)1 */
        if (!(x == 1)) {
            ++failures;
            printf("Expected: (y >> (x++, 0) >> 1) == 0x80 && x == 1, got x: %d\n", x);
        }
    }

    {
        int x = 0, y = 0x100;
        /* Well-defined per standard, lhs expected in cc65: (int)1 */
        if (!((y >> (x++, 8)) == 1)) {
            ++failures;
            x = 0;
            printf("Expected: (y >> (x++, 8)) == 1, got lhs: %ld\n", (long)(y >> (x++, 8)));
        }

        /* Well-defined per standard, lhs expected in cc65: (int)1 */
        if (!(x == 1)) {
            ++failures;
            printf("Expected: (y >> (x++, 8)) == 1 && x == 1, got x: %d\n", x);
        }
    }

    {
        const signed char x = 0x80;
        /* Well-defined per standard, lhs expected in cc65: (int)-256 */
        if (!((x << 1) < 0)) {
            ++failures;
            printf("Expected: (x << 1) < 0, got lhs: %ld\n", (long)(x << 1));
        }
    }

    {
        const signed char x = 0x40;
        /* Well-defined per standard, lhs expected in cc65: (int)128 */
        if (!((x << 1) >= 0)) {
            ++failures;
            printf("Expected: (x << 1) >= 0, got lhs: %ld\n", (long)(x << 1));
        }
    }
}

int main(void)
{
    Test_Defined();

    if (failures != 0) {
        printf("Failures: %d\n", failures);
    }

    return failures;
}
