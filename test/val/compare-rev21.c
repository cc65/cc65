/* Exercise compare operator edge cases in forward and reverse forms;
**   unsigned char: literals, autos.
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

#define TEST_NAME  "compare-rev21"

#include "compare-rev.h"

/* Compared values are signed int literals */
#define N256    -256
#define N255    -255
#define N1      -1
#define ZERO    0
#define P1      1
#define P255    255
#define P256    256
#define P257    257
#define P32767  32767

/* unsigned char to signed literal compares */
static void uchar_loc_to_slit_11(void)
{
    unsigned char v;

    v = 0;
    marker = 0x1100; /* +8 for each macro */

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

static void uchar_loc_to_slit_13(void)
{
    unsigned char v;

    v = 1;
    marker = 0x1300; /* +8 for each macro */

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

static void uchar_loc_to_slit_15(void)
{
    unsigned char v;

    v = 2;
    marker = 0x1500; /* +8 for each macro */

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

static void uchar_loc_to_slit_17(void)
{
    unsigned char v;

    v = 254;
    marker = 0x1700; /* +8 for each macro */

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

static void uchar_loc_to_slit_19(void)
{
    unsigned char v;

    v = 255;
    marker = 0x1900; /* +8 for each macro */

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

int main(void)
{
    marker = 0x01;

    uchar_loc_to_slit_11();
    uchar_loc_to_slit_13();
    uchar_loc_to_slit_15();
    uchar_loc_to_slit_17();
    uchar_loc_to_slit_19();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
