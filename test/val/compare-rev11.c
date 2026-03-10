/* Exercise compare operator edge cases in forward and reverse forms;
**   signed char: literals, autos.
** Note: will cause many compiler warnings:
** -- Result of comparison is always (true|false)
** -- Unreachable code
** 
** License: Public Domain
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
*/

#include <stdio.h>
#include <stdlib.h>

#define TEST_NAME  "compare-rev11"

#include "compare-rev.h"

/* Compared values are signed int literals */
#define N129    -129
#define N128    -128
#define N127    -127
#define N1      -1
#define ZERO    0
#define P1      1
#define P127    127
#define P128    128
#define P255    255
#define P256    256

/* signed char to signed literal compares */
static void schar_loc_to_slit_11(void)
{
    signed char v;

    v = -128;
    marker = 0x1100; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N129);
    MUST_BE_EQUAL(v, N128);
    MUST_BE_LESS_THAN(v, N127);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
}

static void schar_loc_to_slit_13(void)
{
    signed char v;

    v = -127;
    marker = 0x1300; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N129);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_EQUAL(v, N127);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
}

static void schar_loc_to_slit_15(void)
{
    signed char v;

    v = -2;
    marker = 0x1500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N129);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
}

static void schar_loc_to_slit_17(void)
{
    signed char v;

    v = -1;
    marker = 0x1700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N129);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_EQUAL(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
}

static void schar_loc_to_slit_19(void)
{
    signed char v;

    v = 0;
    marker = 0x1900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N129);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_EQUAL(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
}

static void schar_loc_to_slit_1B(void)
{
    signed char v;

    v = 1;
    marker = 0x1B00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N129);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_EQUAL(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
}

static void schar_loc_to_slit_1D(void)
{
    signed char v;

    v = 2;
    marker = 0x1D00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N129);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
}

static void schar_loc_to_slit_1F(void)
{
    signed char v;

    v = 127;
    marker = 0x1F00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N129);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_EQUAL(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
}

int main(void)
{
    marker = 0x01;

    schar_loc_to_slit_11();
    schar_loc_to_slit_13();
    schar_loc_to_slit_15();
    schar_loc_to_slit_17();
    schar_loc_to_slit_19();
    schar_loc_to_slit_1B();
    schar_loc_to_slit_1D();
    schar_loc_to_slit_1F();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
