/* Exercise compare operator edge cases in forward and reverse forms;
**   unsigned char: autos, unsigned statics.
** 
** License: Public Domain
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
*/

#include <stdio.h>
#include <stdlib.h>

#define TEST_NAME  "compare-rev23"

#include "compare-rev.h"

static unsigned stat_u_0 = 0U;
static unsigned char stat_uc_1 = 1U;
static unsigned stat_u_1 = 1U;
static unsigned char stat_uc_255 = 255U;
static unsigned stat_u_255 = 255U;
static unsigned stat_u_256 = 256U;
static unsigned stat_u_257 = 257U;
static unsigned stat_u_32767 = 32767U;
static unsigned stat_u_32768 = 32768U;
static unsigned stat_u_65535 = 65535U;

/* Compared values are unsigned statics */
#define ZERO    stat_u_0
#define P1_C    stat_uc_1
#define P1      stat_u_1
#define P255_C  stat_uc_255
#define P255    stat_u_255
#define P256    stat_u_256
#define P257    stat_u_257
#define P32767  stat_u_32767
#define P32768  stat_u_32768
#define P65535  stat_u_65535

/* unsigned char to unsigned statics compares */
static void uchar_loc_to_ustat_11(void)
{
    unsigned char v;

    v = 0;
    marker = 0x1100; /* +8 for each macro */

    MUST_BE_EQUAL(v, ZERO);
    MUST_BE_LESS_THAN(v, P1_C);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255_C);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void uchar_loc_to_ustat_13(void)
{
    unsigned char v;

    v = 1;
    marker = 0x1300; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_EQUAL(v, P1_C);
    MUST_BE_EQUAL(v, P1);
    MUST_BE_LESS_THAN(v, P255_C);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void uchar_loc_to_ustat_15(void)
{
    unsigned char v;

    v = 2;
    marker = 0x1500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1_C);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255_C);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void uchar_loc_to_ustat_17(void)
{
    unsigned char v;

    v = 254;
    marker = 0x1700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1_C);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255_C);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

static void uchar_loc_to_ustat_19(void)
{
    unsigned char v;

    v = 255;
    marker = 0x1900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1_C);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_EQUAL(v, P255_C);
    MUST_BE_EQUAL(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, P32768);
    MUST_BE_LESS_THAN(v, P65535);
}

int main(void)
{
    marker = 0x01;

    uchar_loc_to_ustat_11();
    uchar_loc_to_ustat_13();
    uchar_loc_to_ustat_15();
    uchar_loc_to_ustat_17();
    uchar_loc_to_ustat_19();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
