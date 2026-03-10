/* Exercise compare operator edge cases in forward and reverse forms;
**   signed int: autos, unsigned statics.
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

#define TEST_NAME  "compare-rev33"

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

/* signed int to unsigned statics compares */
static void sint_loc_to_ustat_11(void)
{
    signed int v;

    v = -32767-1;
    marker = 0x1100; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_GREATER_THAN(v, P32767);
    if (sizeof(v) == 2) {
        MUST_BE_EQUAL(v, P32768);
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void sint_loc_to_ustat_13(void)
{
    signed int v;

    v = -32767;
    marker = 0x1300; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, P32767);
        MUST_BE_GREATER_THAN(v, P32768);
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void sint_loc_to_ustat_15(void)
{
    signed int v;

    v = -257;
    marker = 0x1500; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, P257);
        MUST_BE_GREATER_THAN(v, P32767);
        MUST_BE_GREATER_THAN(v, P32768);
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void sint_loc_to_ustat_17(void)
{
    signed int v;

    v = -256;
    marker = 0x1700; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, P256);
        MUST_BE_GREATER_THAN(v, P257);
        MUST_BE_GREATER_THAN(v, P32767);
        MUST_BE_GREATER_THAN(v, P32768);
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void sint_loc_to_ustat_19(void)
{
    signed int v;

    v = -255;
    marker = 0x1900; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, P255);
        MUST_BE_GREATER_THAN(v, P256);
        MUST_BE_GREATER_THAN(v, P257);
        MUST_BE_GREATER_THAN(v, P32767);
        MUST_BE_GREATER_THAN(v, P32768);
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void sint_loc_to_ustat_1B(void)
{
    signed int v;

    v = -2;
    marker = 0x1B00; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, P255);
        MUST_BE_GREATER_THAN(v, P256);
        MUST_BE_GREATER_THAN(v, P257);
        MUST_BE_GREATER_THAN(v, P32767);
        MUST_BE_GREATER_THAN(v, P32768);
        MUST_BE_LESS_THAN(v, P65535);
    }
}

static void sint_loc_to_ustat_1D(void)
{
    signed int v;

    v = -1;
    marker = 0x1D00; /* +8 for each macro */

    /* Special signed to unsigned promotion cases */
    MUST_BE_GREATER_THAN(v, ZERO);
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, P1);
        MUST_BE_GREATER_THAN(v, P255);
        MUST_BE_GREATER_THAN(v, P256);
        MUST_BE_GREATER_THAN(v, P257);
        MUST_BE_GREATER_THAN(v, P32767);
        MUST_BE_GREATER_THAN(v, P32768);
        MUST_BE_EQUAL(v, P65535);
    }
}

static void sint_loc_to_ustat_1F(void)
{
    signed int v;

    v = 0;
    marker = 0x1F00; /* +8 for each macro */

    MUST_BE_EQUAL(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void sint_loc_to_ustat_21(void)
{
    signed int v;

    v = 1;
    marker = 0x2100; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_EQUAL(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void sint_loc_to_ustat_23(void)
{
    signed int v;

    v = 2;
    marker = 0x2300; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void sint_loc_to_ustat_25(void)
{
    signed int v;

    v = 255;
    marker = 0x2500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_EQUAL(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void sint_loc_to_ustat_27(void)
{
    signed int v;

    v = 256;
    marker = 0x2700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_EQUAL(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void sint_loc_to_ustat_29(void)
{
    signed int v;

    v = 257;
    marker = 0x2900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_EQUAL(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void sint_loc_to_ustat_2B(void)
{
    signed int v;

    v = 32766;
    marker = 0x2B00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void sint_loc_to_ustat_2D(void)
{
    signed int v;

    v = 32767;
    marker = 0x2D00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, P257);
    MUST_BE_EQUAL(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

int main(void)
{
    marker = 0x01;

    sint_loc_to_ustat_11();
    sint_loc_to_ustat_13();
    sint_loc_to_ustat_15();
    sint_loc_to_ustat_17();
    sint_loc_to_ustat_19();
    sint_loc_to_ustat_1B();
    sint_loc_to_ustat_1D();
    sint_loc_to_ustat_1F();
    sint_loc_to_ustat_21();
    sint_loc_to_ustat_23();
    sint_loc_to_ustat_25();
    sint_loc_to_ustat_27();
    sint_loc_to_ustat_29();
    sint_loc_to_ustat_2B();
    sint_loc_to_ustat_2D();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
