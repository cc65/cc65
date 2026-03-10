/* Exercise compare operator edge cases in forward and reverse forms;
**   unsigned int: autos, signed statics.
** Note: will cause some compiler warnings:
** -- Result of comparison is constant
** 
** License: Public Domain
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
*/

#include <stdio.h>
#include <stdlib.h>

#define TEST_NAME  "compare-rev42"

#include "compare-rev.h"

static int stat_i_m1 = -1;
static int stat_i_0 = 0;
static int stat_i_1 = 1;
static int stat_i_255 = 255;
static int stat_i_256 = 256;
static int stat_i_257 = 257;
static int stat_i_32767 = 32767;
static unsigned stat_u_32768 = 32768U;
static unsigned stat_u_65535 = 65535U;

/* Compared values are signed int statics, with some exceptions */
#define N1      stat_i_m1
#define ZERO    stat_i_0
#define P1      stat_i_1
#define P255    stat_i_255
#define P256    stat_i_256
#define P257    stat_i_257
#define P32767  stat_i_32767
#define P32768  stat_u_32768
#define P65535  stat_u_65535

/* unsigned int to signed int statics compares */
static void uint_loc_to_sstat_11(void)
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

static void uint_loc_to_sstat_13(void)
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

static void uint_loc_to_sstat_15(void)
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

static void uint_loc_to_sstat_17(void)
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

static void uint_loc_to_sstat_19(void)
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

static void uint_loc_to_sstat_1B(void)
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

static void uint_loc_to_sstat_1D(void)
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

static void uint_loc_to_sstat_1F(void)
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

static void uint_loc_to_sstat_21(void)
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

static void uint_loc_to_sstat_23(void)
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

    uint_loc_to_sstat_11();
    uint_loc_to_sstat_13();
    uint_loc_to_sstat_15();
    uint_loc_to_sstat_17();
    uint_loc_to_sstat_19();
    uint_loc_to_sstat_1B();
    uint_loc_to_sstat_1D();
    uint_loc_to_sstat_1F();
    uint_loc_to_sstat_21();
    uint_loc_to_sstat_23();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
