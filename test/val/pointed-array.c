/*
** !!DESCRIPTION!! Simple tests of pointer-to-array dereferences
** !!ORIGIN!!      cc65 regression tests
** !!LICENCE!!     Public Domain
** !!AUTHOR!!      2015-06-29, Greg King
*/

#include <stdio.h>

static unsigned char failures = 0;
static size_t Size;

typedef unsigned char array_t[4][4];

static array_t table = {
    {12, 13, 14, 15},
    { 8,  9, 10, 11},
    { 4,  5,  6,  7},
    { 0,  1,  2,  3}
};
static array_t *tablePtr = &table;

static unsigned (*vector)[2];

static unsigned char y = 0, x;

int main(void)
{
    /* The indirection must convert the expression-type (from Pointer into
    ** Array); but, it must not convert the value, because it already points
    ** to the start of the array.
    */
    /* (Note:  I reduce output clutter by using a variable to prevent
    ** compiler warnings about constant comparisons and unreachable code.
    */
    if ((Size = sizeof *tablePtr) != sizeof table) {
        ++failures;
    }
    if (*tablePtr != table) {
        ++failures;
    }

    /* Test fetching. */
    do {
        x = 0;
        do {
            if ((*tablePtr)[y][x] != table[y][x]) {
                ++failures;
                printf("(*tableptr)[%u][%u] (%u) != table[%u][%u] (%u).\n",
                       y, x, (*tablePtr)[y][x],
                       y, x, table[y][x]);
            }
        } while (++x < sizeof table[0]);
    } while (++y < sizeof table / sizeof table[0]);

    vector = (unsigned (*)[])table[1];
    if ((*vector)[1] != 0x0B0A) {
        ++failures;
    }

    /* Test storing. */
    (*tablePtr)[2][1] = 42;
    if (table[2][1] != 42) {
        ++failures;
        printf("table[2][1] == %u (should have changed from 5 to 42).\n",
               table[2][1]);
    }
    x = 3;
    y = 1;
    (*tablePtr)[y][x] = 83;
    if (table[1][3] != 83) {
        ++failures;
        printf("table[y][x] == %u (should have changed from 11 to 83).\n",
               table[1][3]);
    }

    /* Test triple indirection.  It should compile to two indirection
    ** operations.
    */
    --***tablePtr;
    if (**table != 11) {
        ++failures;
        printf("**table == %u (should have changed from 12 to 11).\n",
               table[0][0]);
    }

    if (failures != 0) {
        printf("failures: %u\n", failures);
    }
    return failures;
}
