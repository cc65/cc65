/* Exercise compare operator array expression cases in forward and reverse forms;
**   signed int: autos, static arrays.
** Note: will cause some compiler warnings:
** -- Result of comparison is constant
** -- 'x_yyy' is defined but never used   [not all defined values used]
** 
** License: Public Domain
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
*/

#include <stdio.h>
#include <stdlib.h>

#define TEST_NAME  "compare-rev34"

#include "compare-rev.h"

/* The indexes of compared values are kept the same across different types */
static int stat_i[16] = {
    -32767-1, -32767, -257, -256, -255, -1, 0, 1, 255, 256, 257, 32766, 32767, 0, 0, 0
};

static unsigned stat_u[16] = {
    0, 0, 0, 0, 0, -1U, 0U, 1U, 255U, 256U, 257U, 32766U, 32767U, 32768U, 65534U, 65535U
};

static unsigned char stat_uc[16] = {
    0, 0, 0, 0, 0, 0, 0, 1, 255, 0, 0, 0, 0, 0, 0, 0
};

/* The indexes of compared values */
static unsigned char x_m32768 = 0;
static unsigned char x_m32767 = 1;
static unsigned char x_m257 = 2;
static unsigned char x_m256 = 3;
static unsigned char x_m255 = 4;
static unsigned char x_m1 = 5;
static unsigned char x_0 = 6;
static unsigned char x_1 = 7;
static unsigned char x_255 = 8;
static unsigned char x_256 = 9;
static unsigned char x_257 = 10;
static unsigned char x_32766 = 11;
static unsigned char x_32767 = 12;
static unsigned char x_32768 = 13;
static unsigned char x_65534 = 14;
static unsigned char x_65535 = 15;

/* Compared values are static arrays. The actual types used in comparisons
** are controlled here by the defines.
*/
#define N32768  stat_i[x_m32768]
#define N32767  stat_i[x_m32767]
#define N257    stat_i[x_m257]
#define N256    stat_i[x_m256]
#define N255    stat_i[x_m255]
#define N1      stat_i[x_m1]
#define ZERO    stat_uc[x_0]
#define P1      stat_uc[x_1]
#define P255    stat_uc[x_255]
#define P256    stat_i[x_256]
#define U257    stat_u[x_257]
#define P32766  stat_i[x_32766]
#define P32767  stat_i[x_32767]
#define U32768  stat_u[x_32768]
#define U65534  stat_u[x_65534]
#define U65535  stat_u[x_65535]

/* signed int to static arrays compares */
/* General form: if (loc >= array[x_val]) */
static void sint_loc_to_statarr_11(void)
{
    signed int v;

    v = -32767-1;
    marker = 0x1100; /* +8 for each macro */

    MUST_BE_LESS_THAN(v, N32767);
    MUST_BE_LESS_THAN(v, N256);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P32767);
}

static void sint_loc_to_statarr_13(void)
{
    signed int v;

    v = -32767;
    marker = 0x1300; /* +8 for each macro */

    MUST_BE_EQUAL(v, N32767);
    MUST_BE_LESS_THAN(v, N256);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P32766);
    /* Special signed to unsigned promotion cases */
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, U32768);
        MUST_BE_LESS_THAN(v, U65535);
    }
}

static void sint_loc_to_statarr_15(void)
{
    signed int v;

    v = -257;
    marker = 0x1500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_LESS_THAN(v, N256);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P32766);
    /* Special signed to unsigned promotion cases */
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, U257);
        MUST_BE_GREATER_THAN(v, U32768);
    }
}

static void sint_loc_to_statarr_17(void)
{
    signed int v;

    v = -256;
    marker = 0x1700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_EQUAL(v, N256);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P32766);
}

static void sint_loc_to_statarr_19(void)
{
    signed int v;

    v = -2;
    marker = 0x1900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_LESS_THAN(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P32766);
}

static void sint_loc_to_statarr_1B(void)
{
    signed int v;

    v = -1;
    marker = 0x1B00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_EQUAL(v, N1);
    MUST_BE_LESS_THAN(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, P32766);
    /* Special signed to unsigned promotion cases */
    if (sizeof(v) == 2) {
        MUST_BE_GREATER_THAN(v, U32768);
        MUST_BE_GREATER_THAN(v, U65534);
    }
}

static void sint_loc_to_statarr_1D(void)
{
    signed int v;

    v = 0;
    marker = 0x1D00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_EQUAL(v, ZERO);
    MUST_BE_LESS_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, U257);
    MUST_BE_LESS_THAN(v, P32766);
}

static void sint_loc_to_statarr_1F(void)
{
    signed int v;

    v = 1;
    marker = 0x1F00; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_EQUAL(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, U257);
    MUST_BE_LESS_THAN(v, P32766);
}

static void sint_loc_to_statarr_21(void)
{
    signed int v;

    v = 2;
    marker = 0x2100; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_LESS_THAN(v, P255);
    MUST_BE_LESS_THAN(v, P256);
    MUST_BE_LESS_THAN(v, U257);
    MUST_BE_LESS_THAN(v, P32766);
}

static void sint_loc_to_statarr_25(void)
{
    signed int v;

    v = 256;
    marker = 0x2500; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_EQUAL(v, P256);
    MUST_BE_LESS_THAN(v, U257);
    MUST_BE_LESS_THAN(v, P32766);
    MUST_BE_LESS_THAN(v, U65534);
    MUST_BE_LESS_THAN(v, U65535);
}

static void sint_loc_to_statarr_27(void)
{
    signed int v;

    v = 257;
    marker = 0x2700; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_EQUAL(v, U257);
    MUST_BE_LESS_THAN(v, P32766);
    MUST_BE_LESS_THAN(v, U65534);
}

static void sint_loc_to_statarr_29(void)
{
    signed int v;

    v = 32766;
    marker = 0x2900; /* +8 for each macro */

    MUST_BE_GREATER_THAN(v, N32767);
    MUST_BE_GREATER_THAN(v, N256);
    MUST_BE_GREATER_THAN(v, N1);
    MUST_BE_GREATER_THAN(v, ZERO);
    MUST_BE_GREATER_THAN(v, P1);
    MUST_BE_GREATER_THAN(v, P255);
    MUST_BE_GREATER_THAN(v, P256);
    MUST_BE_GREATER_THAN(v, U257);
    MUST_BE_EQUAL(v, P32766);
    MUST_BE_LESS_THAN(v, P32767);
    MUST_BE_LESS_THAN(v, U65534);
    MUST_BE_LESS_THAN(v, U65535);
}

int main(void)
{
    marker = 0x01;

    sint_loc_to_statarr_11();
    sint_loc_to_statarr_13();
    sint_loc_to_statarr_15();
    sint_loc_to_statarr_17();
    sint_loc_to_statarr_19();
    sint_loc_to_statarr_1B();
    sint_loc_to_statarr_1D();
    sint_loc_to_statarr_1F();
    sint_loc_to_statarr_21();
    sint_loc_to_statarr_25();
    sint_loc_to_statarr_27();
    sint_loc_to_statarr_29();

    printf(TEST_NAME " failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
