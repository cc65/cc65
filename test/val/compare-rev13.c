/* Exercise compare operator edge cases in forward and reverse forms;
**   signed char: autos, usigned statics.
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

#define TEST_NAME  "compare-rev13"

#include "compare-rev.h"

static unsigned stat_u_0 = 0U;
static unsigned stat_u_1 = 1U;
static unsigned char stat_u_127 = 127U;
static unsigned char stat_u_128 = 128U;
static unsigned char stat_u_255 = 255U;
static unsigned stat_u_256 = 256U;
static unsigned stat_u_257 = 257U;
static unsigned stat_u_32767 = 32767U;
static unsigned stat_u_32768 = 32768U;
static unsigned stat_u_65535 = 65535U;

/* Compared values are unsigned statics */
#define ZERO    stat_u_0
#define P1      stat_u_1
#define P127    stat_u_127
#define P128    stat_u_128
#define P255    stat_u_255
#define P256    stat_u_256
#define P257    stat_u_257
#define P32767  stat_u_32767
#define P32768  stat_u_32768
#define P65535  stat_u_65535

/* signed char to unsigned statics compares */
static void schar_loc_to_ustat_11(void)
{
    signed char v;

    v = -128;
    marker = 0x1100; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    /* Normal unsigned char promotion cases */
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_GREATER_THAN(v, P32767);
    MUST_BE_GREATER_THAN(v, P32768);
    if (sizeof(P65535) == 2) {
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void schar_loc_to_ustat_13(void)
{
    signed char v;

    v = -127;
    marker = 0x1300; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    /* Normal unsigned char promotion cases */
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_GREATER_THAN(v, P32767);
    MUST_BE_GREATER_THAN(v, P32768);
    if (sizeof(P65535) == 2) {
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void schar_loc_to_ustat_15(void)
{
    signed char v;

    v = -2;
    marker = 0x1500; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    /* Normal unsigned char promotion cases */
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_GREATER_THAN(v, P32767);
    MUST_BE_GREATER_THAN(v, P32768);
    if (sizeof(P65535) == 2) {
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void schar_loc_to_ustat_17(void)
{
    signed char v;

    v = -1;
    marker = 0x1700; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    /* Normal unsigned char promotion cases */
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_GREATER_THAN(v, P32767);
    MUST_BE_GREATER_THAN(v, P32768);
    if (sizeof(P65535) == 2) {
        MUST_BE_EQUAL(v, P65535); /* special case: -1 to uint promotion */
    }
}

static void schar_loc_to_ustat_19(void)
{
    signed char v;

    v = 0;
    marker = 0x1900; /* +8 for each macro */

    MUST_BE_EQUAL(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void schar_loc_to_ustat_1B(void)
{
    signed char v;

    v = 1;
    marker = 0x1B00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_EQUAL(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void schar_loc_to_ustat_1D(void)
{
    signed char v;

    v = 2;
    marker = 0x1D00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void schar_loc_to_ustat_1F(void)
{
    signed char v;

    v = 127;
    marker = 0x1F00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_EQUAL(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

int main(void)
{
    marker = 0x01;

    schar_loc_to_ustat_11();
    schar_loc_to_ustat_13();
    schar_loc_to_ustat_15();
    schar_loc_to_ustat_17();
    schar_loc_to_ustat_19();
    schar_loc_to_ustat_1B();
    schar_loc_to_ustat_1D();
    schar_loc_to_ustat_1F();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
