/* Exercise compare operator edge cases in forward and reverse forms;
**   signed int: literals, autos.
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

#define TEST_NAME  "compare-rev31"

#include "compare-rev.h"

/* Compared values are signed int literals */
#define N32768  -32767-1
#define N32767  -32767
#define N257    -257
#define N256    -256
#define N255    -255
#define N1      -1
#define ZERO    0
#define P1      1
#define P255    255
#define P256    256
#define P257    257
#define P32767  32767

/* signed int to signed literal compares */
static void sint_loc_to_slit_11(void)
{
    signed int v;

    v = -32767-1;
    marker = 0x1100; /* +8 for each macro */

    MUST_BE_EQUAL(v, N32768);
    MUST_BE_LESS_THAN(v, N32767);
    MUST_BE_LESS_THAN(v, N257);
    MUST_BE_LESS_THAN(v, N256);
    MUST_BE_LESS_THAN(v, N255);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_13(void)
{
    signed int v;

    v = -32767;
    marker = 0x1300; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_EQUAL(v, N32767);
    MUST_BE_LESS_THAN(v, N257);
    MUST_BE_LESS_THAN(v, N256);
    MUST_BE_LESS_THAN(v, N255);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_15(void)
{
    signed int v;

    v = -257;
    marker = 0x1500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_EQUAL(v, N257);
    MUST_BE_LESS_THAN(v, N256);
    MUST_BE_LESS_THAN(v, N255);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_17(void)
{
    signed int v;

    v = -256;
    marker = 0x1700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_EQUAL(v, N256);
    MUST_BE_LESS_THAN(v, N255);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_19(void)
{
    signed int v;

    v = -255;
    marker = 0x1900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_EQUAL(v, N255);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_1B(void)
{
    signed int v;

    v = -2;
    marker = 0x1B00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_1D(void)
{
    signed int v;

    v = -1;
    marker = 0x1D00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_EQUAL(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_1F(void)
{
    signed int v;

    v = 0;
    marker = 0x1F00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_EQUAL(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_21(void)
{
    signed int v;

    v = 1;
    marker = 0x2100; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_EQUAL(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_23(void)
{
    signed int v;

    v = 2;
    marker = 0x2300; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_25(void)
{
    signed int v;

    v = 255;
    marker = 0x2500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_EQUAL(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_27(void)
{
    signed int v;

    v = 256;
    marker = 0x2700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_EQUAL(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_29(void)
{
    signed int v;

    v = 257;
    marker = 0x2900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_EQUAL(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_2B(void)
{
    signed int v;

    v = 32766;
    marker = 0x2B00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_slit_2D(void)
{
    signed int v;

    v = 32767;
    marker = 0x2D00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_EQUAL(v, P32767);
}

int main(void)
{
    marker = 0x01;

    sint_loc_to_slit_11();
    sint_loc_to_slit_13();
    sint_loc_to_slit_15();
    sint_loc_to_slit_17();
    sint_loc_to_slit_19();
    sint_loc_to_slit_1B();
    sint_loc_to_slit_1D();
    sint_loc_to_slit_1F();
    sint_loc_to_slit_21();
    sint_loc_to_slit_23();
    sint_loc_to_slit_25();
    sint_loc_to_slit_27();
    sint_loc_to_slit_29();
    sint_loc_to_slit_2B();
    sint_loc_to_slit_2D();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
