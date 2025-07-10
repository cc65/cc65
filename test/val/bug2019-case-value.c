/* Bug #2019 - Type promotion in switch statements seems to be broken */

#include <limits.h>
#include <stdio.h>

unsigned failures;

int f1(void)
{
    unsigned char c = 0xFF;
    switch (c) {
        case (signed char)0xFF:   break;
        case (unsigned char)0xFF: return 0;
    }
    return -1;
}

int f2(void)
{
    signed char c = SCHAR_MIN;
    switch (c) {
        case (unsigned char)SCHAR_MIN: break;
        case SCHAR_MIN:                return 0;
    }
    return -1;
}

int f3(void)
{
    signed int c = (int)UINT_MAX;
    switch (c) {
        case UINT_MAX: return 0;
    }
    return -1;
}

int f4(void)
{
    unsigned int c = UINT_MAX;
    switch (c) {
        case -1L: return 0;
    }
    return -1;
}

int main(void)
{
    if (f1())
    {
        ++failures;
        printf("f1() failed\n");
    }

    if (f2())
    {
        ++failures;
        printf("f2() failed\n");
    }

    if (f3())
    {
        ++failures;
        printf("f3() failed\n");
    }

    if (f4())
    {
        ++failures;
        printf("f4() failed\n");
    }

    if (failures > 0)
    {
        printf("failures: %u\n", failures);
    }
    return failures;
}
