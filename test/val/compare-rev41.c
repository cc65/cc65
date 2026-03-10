/* Exercise compare operator edge cases in forward and reverse forms;
**   unsigned int: literals, autos.
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

#define TEST_NAME  "compare-rev41"

#include "compare-rev.h"

/* Compared values are signed int literals */
#define N1      -1
#define ZERO    0
#define P1      1
#define P255    255
#define P256    256
#define P257    257
#define P32767  32767
#define P32768  32768
#define P65535  65535

/* unsigned int to signed literal compares */
static void uint_loc_to_slit_11(void)
{
    unsigned int v;

    v = 0U;
    marker = 0x1100; /* +8 for each macro */

    MUST_BE_EQUAL(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_13(void)
{
    unsigned int v;

    v = 1U;
    marker = 0x1300; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_EQUAL(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_15(void)
{
    unsigned int v;

    v = 2U;
    marker = 0x1500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_17(void)
{
    unsigned int v;

    v = 255U;
    marker = 0x1700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_EQUAL(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_19(void)
{
    unsigned int v;

    v = 256U;
    marker = 0x1900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_EQUAL(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_1B(void)
{
    unsigned int v;

    v = 257U;
    marker = 0x1B00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_EQUAL(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_1D(void)
{
    unsigned int v;

    v = 32767U;
    marker = 0x1D00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_EQUAL(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_1F(void)
{
    unsigned int v;

    v = 32768U;
    marker = 0x1F00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_GREATER_THAN(v, P32767);
    MUST_BE_EQUAL(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_21(void)
{
    unsigned int v;

    v = 65534U;
    marker = 0x2100; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_GREATER_THAN(v, P32767);
    MUST_BE_GREATER_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
    MUST_BE_LESS_THAN(v, N1); /* special case: -1 to uint promotion */
}

static void uint_loc_to_slit_23(void)
{
    unsigned int v;

    v = 65535U;
    marker = 0x2300; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_GREATER_THAN(v, P32767);
    MUST_BE_GREATER_THAN(v, P32768);
    MUST_BE_EQUAL(v, P65535);

    if (sizeof(v) == 2) {
        MUST_BE_EQUAL(v, N1); /* special case: -1 to uint promotion */
    }
}

int main(void)
{
    marker = 0x01;

    uint_loc_to_slit_11();
    uint_loc_to_slit_13();
    uint_loc_to_slit_15();
    uint_loc_to_slit_17();
    uint_loc_to_slit_19();
    uint_loc_to_slit_1B();
    uint_loc_to_slit_1D();
    uint_loc_to_slit_1F();
    uint_loc_to_slit_21();
    uint_loc_to_slit_23();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
