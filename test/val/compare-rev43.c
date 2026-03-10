/* Exercise compare operator edge cases in forward and reverse forms;
**   unsigned int: autos, unsigned statics.
** 
** License: Public Domain
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
*/

#include <stdio.h>
#include <stdlib.h>

#define TEST_NAME  "compare-rev43"

#include "compare-rev.h"

static unsigned stat_u_0 = 0U;
static unsigned stat_u_1 = 1U;
static unsigned stat_u_255 = 255U;
static unsigned stat_u_256 = 256U;
static unsigned stat_u_257 = 257U;
static unsigned stat_u_32767 = 32767U;
static unsigned stat_u_32768 = 32768U;
static unsigned stat_u_65535 = 65535U;

/* Compared values are unsigned statics */
#define ZERO    stat_u_0
#define P1      stat_u_1
#define P255    stat_u_255
#define P256    stat_u_256
#define P257    stat_u_257
#define P32767  stat_u_32767
#define P32768  stat_u_32768
#define P65535  stat_u_65535

/* unsigned int to unsigned statics compares */
static void uint_loc_to_ustat_11(void)
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
}

static void uint_loc_to_ustat_13(void)
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
}

static void uint_loc_to_ustat_15(void)
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
}

static void uint_loc_to_ustat_17(void)
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
}

static void uint_loc_to_ustat_19(void)
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
}

static void uint_loc_to_ustat_1B(void)
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
}

static void uint_loc_to_ustat_1D(void)
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
}

static void uint_loc_to_ustat_1F(void)
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
}

static void uint_loc_to_ustat_21(void)
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
}

static void uint_loc_to_ustat_23(void)
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
}

int main(void)
{
    marker = 0x01;

    uint_loc_to_ustat_11();
    uint_loc_to_ustat_13();
    uint_loc_to_ustat_15();
    uint_loc_to_ustat_17();
    uint_loc_to_ustat_19();
    uint_loc_to_ustat_1B();
    uint_loc_to_ustat_1D();
    uint_loc_to_ustat_1F();
    uint_loc_to_ustat_21();
    uint_loc_to_ustat_23();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
