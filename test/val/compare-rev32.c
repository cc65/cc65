/* Exercise compare operator edge cases in forward and reverse forms;
**   signed int: autos, signed statics.
** 
** License: Public Domain
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
*/

#include <stdio.h>
#include <stdlib.h>

#define TEST_NAME  "compare-rev32"

#include "compare-rev.h"

static int stat_i_m32768 = -32767-1;
static int stat_i_m32767 = -32767;
static int stat_i_m257 = -257;
static int stat_i_m256 = -256;
static int stat_i_m255 = -255;
static int stat_i_m1 = -1;
static int stat_i_0 = 0;
static int stat_i_1 = 1;
static int stat_i_255 = 255;
static int stat_i_256 = 256;
static int stat_i_257 = 257;
static int stat_i_32767 = 32767;

/* Compared values are signed int statics */
#define N32768  stat_i_m32768
#define N32767  stat_i_m32767
#define N257    stat_i_m257
#define N256    stat_i_m256
#define N255    stat_i_m255
#define N1      stat_i_m1
#define ZERO    stat_i_0
#define P1      stat_i_1
#define P255    stat_i_255
#define P256    stat_i_256
#define P257    stat_i_257
#define P32767  stat_i_32767

/* signed int to signed int statics compares */
static void sint_loc_to_sstat_11(void)
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

static void sint_loc_to_sstat_13(void)
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

static void sint_loc_to_sstat_15(void)
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

static void sint_loc_to_sstat_17(void)
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

static void sint_loc_to_sstat_19(void)
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

static void sint_loc_to_sstat_1B(void)
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

static void sint_loc_to_sstat_1D(void)
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

static void sint_loc_to_sstat_1F(void)
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

static void sint_loc_to_sstat_21(void)
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

static void sint_loc_to_sstat_23(void)
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

static void sint_loc_to_sstat_25(void)
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

static void sint_loc_to_sstat_27(void)
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

static void sint_loc_to_sstat_29(void)
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

static void sint_loc_to_sstat_2B(void)
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

static void sint_loc_to_sstat_2D(void)
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

    sint_loc_to_sstat_11();
    sint_loc_to_sstat_13();
    sint_loc_to_sstat_15();
    sint_loc_to_sstat_17();
    sint_loc_to_sstat_19();
    sint_loc_to_sstat_1B();
    sint_loc_to_sstat_1D();
    sint_loc_to_sstat_1F();
    sint_loc_to_sstat_21();
    sint_loc_to_sstat_23();
    sint_loc_to_sstat_25();
    sint_loc_to_sstat_27();
    sint_loc_to_sstat_29();
    sint_loc_to_sstat_2B();
    sint_loc_to_sstat_2D();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
