/* Exercise compare operator edge cases in forward and reverse forms;
**   signed char: autos, signed statics.
** 
** License: Public Domain
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
*/

#include <stdio.h>
#include <stdlib.h>

#define TEST_NAME  "compare-rev12"

#include "compare-rev.h"

static int stat_i_m32768 = -32767-1;
static int stat_i_m32767 = -32767;
static int stat_i_m257 = -257;
static int stat_i_m256 = -256;
static int stat_i_m255 = -255;
static signed char stat_sc_m128 = -128;
static signed char stat_sc_m127 = -127;
static int stat_i_m1 = -1;
static int stat_i_0 = 0;
static int stat_i_1 = 1;
static signed char stat_sc_127 = 127;
static int stat_i_128 = 128;
static int stat_i_255 = 255;
static int stat_i_256 = 256;
static int stat_i_257 = 257;
static int stat_i_32767 = 32767;

/* Compared values are signed int statics, with a couple exceptions */
#define N32768  stat_i_m32768
#define N32767  stat_i_m32767
#define N257    stat_i_m257
#define N256    stat_i_m256
#define N255    stat_i_m255
#define N128    stat_sc_m128
#define N127    stat_sc_m127
#define N1      stat_i_m1
#define ZERO    stat_i_0
#define P1      stat_i_1
#define P127    stat_sc_127
#define P128    stat_i_128
#define P255    stat_i_255
#define P256    stat_i_256
#define P257    stat_i_257
#define P32767  stat_i_32767

/* signed char to signed int statics compares */
static void schar_loc_to_sstat_11(void)
{
    signed char v;

    v = -128;
    marker = 0x1100; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_EQUAL(v, N128);
    MUST_BE_LESS_THAN(v, N127);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void schar_loc_to_sstat_13(void)
{
    signed char v;

    v = -127;
    marker = 0x1300; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_EQUAL(v, N127);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void schar_loc_to_sstat_15(void)
{
    signed char v;

    v = -2;
    marker = 0x1500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void schar_loc_to_sstat_17(void)
{
    signed char v;

    v = -1;
    marker = 0x1700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_EQUAL(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void schar_loc_to_sstat_19(void)
{
    signed char v;

    v = 0;
    marker = 0x1900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_EQUAL(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void schar_loc_to_sstat_1B(void)
{
    signed char v;

    v = 1;
    marker = 0x1B00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_EQUAL(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void schar_loc_to_sstat_1D(void)
{
    signed char v;

    v = 2;
    marker = 0x1D00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

static void schar_loc_to_sstat_1F(void)
{
    signed char v;

    v = 127;
    marker = 0x1F00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32768);
    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N257);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N255);
    MUST_BE_GREATER_THAN(v, N128);
    MUST_BE_GREATER_THAN(v, N127);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_EQUAL(v, P127);
    MUST_BE_LESS_THAN(v, P128);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P257);
    MUST_BE_LESS_THAN(v, P32767);
}

int main(void)
{
    marker = 0x01;

    schar_loc_to_sstat_11();
    schar_loc_to_sstat_13();
    schar_loc_to_sstat_15();
    schar_loc_to_sstat_17();
    schar_loc_to_sstat_19();
    schar_loc_to_sstat_1B();
    schar_loc_to_sstat_1D();
    schar_loc_to_sstat_1F();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
