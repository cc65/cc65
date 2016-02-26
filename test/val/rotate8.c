/*
  !!DESCRIPTION!! Optimized-shift signed ints right by a constant; and, assign to chars.
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Greg King
*/

#include <stdio.h>

static unsigned char failures = 0;
static unsigned char n = 0;

/* This number must be read from a variable because
** we want this program, not cc65, to do the shift.
*/
static const signed int aint0 = 0xAAC0;

static signed char achar0, achar1;

static void check(void)
{
    if ((unsigned char)achar0 != (unsigned char)achar1)
        ++failures;
}

static void shift_right_0(void)
{
    achar0 = aint0 >> 0;
    check();
}

static void shift_right_1(void)
{
    achar0 = aint0 >> 1;
    check();
}

static void shift_right_2(void)
{
    achar0 = aint0 >> 2;
    check();
}

static void shift_right_3(void)
{
    achar0 = aint0 >> 3;
    check();
}

static void shift_right_4(void)
{
    achar0 = aint0 >> 4;
    check();
}

static void shift_right_5(void)
{
    achar0 = aint0 >> 5;
    check();
}

static void shift_right_6(void)
{
    achar0 = aint0 >> 6;
    check();
}

static void shift_right_7(void)
{
    achar0 = aint0 >> 7;
    check();
}

static void shift_right_8(void)
{
    achar0 = aint0 >> 8;
    check();
}

static void shift_right_9(void)
{
    achar0 = aint0 >> 9;
    check();
}

static void shift_right_10(void)
{
    achar0 = aint0 >> 10;
    check();
}

static void shift_right_11(void)
{
    achar0 = aint0 >> 11;
    check();
}

static void shift_right_12(void)
{
    achar0 = aint0 >> 12;
    check();
}

static void shift_right_13(void)
{
    achar0 = aint0 >> 13;
    check();
}

static void shift_right_14(void)
{
    achar0 = aint0 >> 14;
    check();
}

static void shift_right_15(void)
{
    achar0 = aint0 >> 15;
    check();
}

const struct {
    signed char achar;
    void (*func)(void);
} tests[] = {
    {0xC0, shift_right_0},
    {0x60, shift_right_1},
    {0xB0, shift_right_2},
    {0x58, shift_right_3},
    {0xAC, shift_right_4},
    {0x56, shift_right_5},
    {0xAB, shift_right_6},
    {0x55, shift_right_7},
    {0xAA, shift_right_8},
    {0xD5, shift_right_9},
    {0xEA, shift_right_10},
    {0xF5, shift_right_11},
    {0xFA, shift_right_12},
    {0xFD, shift_right_13},
    {0xFE, shift_right_14},
    {0xFF, shift_right_15}
};

int main(void)
{
    do {
        achar1 = tests[n].achar;
        tests[n].func();
    } while (++n < sizeof tests / sizeof tests[0]);

    if (failures) {
        printf("rotate8: failures: %u (of %u).\n",
               failures, sizeof tests / sizeof tests[0]);
    }
    return failures;
}
