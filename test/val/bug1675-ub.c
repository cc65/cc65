/* #1675 - Some UB cases of bit-shifts */

#include <stdio.h>

int unexpected = 0;

void Test_UB(void)
{
    {
        /* UB per standard, lhs expected in cc65: (int)-32768 */
        if (!((0x4000 << 1) < 0)) {
            ++unexpected;
            printf("Expected: (0x4000 << 1) < 0, got lhs: %ld\n", (long)(0x4000 << 1));
        }
    }

    {
        /* UB per standard, lhs expected in cc65: (long)-2147483648L */
        if (!((0x40000000 << 1) < 0)) {
            ++unexpected;
            printf("Expected: (0x40000000 << 1) < 0, got lhs: %ld\n", (long)(0x40000000 << 1));
        }
    }

    {
        /* UB per standard, lhs expected in cc65: (int)-32768 */
        if (!(((unsigned char)0x80 << 8) < 0)) {
            ++unexpected;
            printf("Expected: ((unsigned char)0x80 << 8) < 0, got lhs: %ld\n", (long)((unsigned char)0x80 << 8));
        }
    }

    {
        /* UB per standard, lhs expected in cc65: (int)-32768 */
        if (!(((short)0x4000L << 1) < 0)) {
            ++unexpected;
            printf("Expected: ((short)0x4000L << 1) < 0, got lhs: %ld\n", (long)((short)0x4000L << 1));
        }
    }

    {
        const signed short x = 0x4000;
        /* UB per standard, lhs expected in cc65: (int)-32768 */
        if (!((x << 1) < 0)) {
            ++unexpected;
            printf("Expected: (x << 1) < 0, got lhs: %ld\n", (long)(x << 1));
        }
    }
}

int main(void)
{
    Test_UB();

    if (unexpected != 0) {
        printf("Unexpected: %d\n", unexpected);
    }

    return unexpected;
}
