/*
  !!DESCRIPTION!! Make sure that the fields of anonymous structs/unions can be reached properly.
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Greg King
*/

#include <stddef.h>
#include <stdio.h>

static unsigned char fails = 0;

typedef struct {
    short s1;
    struct {
        char c1;
        int i1;
        long l1;
    };
    char c2;
} s1_t;

typedef struct {
    char c2;
    union {
        int i1;
        char c1;
        long l1;
    };
    short s1;
} s2_t;

typedef union {
    short s1;
    struct {
        int i1;
        long l1;
        char c1;
    };
    char c2;
} u1_t;

typedef union {
    short s1;
    union {
        long l1;
        char c1;
        int i1;
    };
    char c2;
} u2_t;

typedef struct {
    union {
        short s1;
        struct {
            int i1;
            long l1;
            char c1;
        };
        char c2;
    };
    short s2;
} s3_t;

static s1_t s1;
static s2_t s2;
static u1_t u1;
static u2_t u2;

static long l2;
static int i2;

/* We use "variables" in the comparisons, so that we can avoid "constant
** comparison" and "Unreachable code" warnings (the second one currently
** can't be suppressed).
*/

static size_t const zero = 0;
static size_t const one = 1;
static size_t const three = 3;
static size_t const five = 5;
static size_t const six = 6;
static size_t const seven = 7;
static size_t const nine = 9;

int main(void)
{
    /* Can cc65 see the names of members of anonymous structs/unions? */

    l2 = s1.l1;
    l2 = s2.l1;
    l2 = u1.l1;
    l2 = u2.l1;

    i2 = s1.c1;
    i2 = s1.c2;
    i2 = s2.c1;
    i2 = s2.c2;
    i2 = u1.c1;
    i2 = u1.c2;
    i2 = u2.c1;
    i2 = u2.c2;

    /* Does cc65 use the correct offsets of
    ** the members of anonymous structs/unions?
    */

    if (offsetof(s1_t, i1) != three) {
        printf("The offset of s1.i1 is %u; it should be 3.\n", offsetof(s1_t, i1));
        ++fails;
    }
    if (offsetof(s2_t, l1) != one) {
        printf("The offset of s2.l1 is %u; it should be 1.\n", offsetof(s2_t, l1));
        ++fails;
    }
    if (offsetof(u1_t, c1) != six) {
        printf("The offset of u1.c1 is %u; it should be 6.\n", offsetof(u1_t, c1));
        ++fails;
    }
    if (offsetof(u2_t, i1) != zero) {
        printf("The offset of u2.i1 is %u; it should be 0.\n", offsetof(u2_t, i1));
        ++fails;
    }

    /* Does cc65 use the correct offset of a member
    ** that's later than an anonymous struct/union?
    */

    if (offsetof(s1_t, c2) != nine) {
        printf("The offset of s1.c2 is %u; it should be 9.\n", offsetof(s1_t, c2));
        ++fails;
    }
    if (offsetof(s2_t, s1) != five) {
        printf("The offset of s2.s1 is %u; it should be 5.\n", offsetof(s2_t, s1));
        ++fails;
    }
    if (offsetof(u1_t, c2) != zero) {
        printf("The offset of u1.c2 is %u; it should be 0.\n", offsetof(u1_t, c2));
        ++fails;
    }
    if (offsetof(u2_t, c2) != zero) {
        printf("The offset of u2.c2 is %u; it should be 0.\n", offsetof(u2_t, c2));
        ++fails;
    }
    if (offsetof(s3_t, s2) != seven) {
        printf("The offset of s3.s2 is %u; it should be 7.\n", offsetof(s3_t, s2));
        ++fails;
    }

    return fails;
}
