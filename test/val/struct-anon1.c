/*
  !!DESCRIPTION!! Make sure that structs/unions know the sizes of anonymous struct/union members
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Greg King
*/

/*
  see https://github.com/cc65/cc65/issues/641
*/

#include <stdio.h>

static unsigned char fails = 0;

typedef struct {
    short s1;
    struct {
        int i1;
        long l1;
        char c1;
    };
    char c2;
} s1_t;

typedef struct {
    short s1;
    union {
        int i1;
        long l1;
        char c1;
    };
    char c2;
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
        int i1;
        long l1;
        char c1;
    };
    char c2;
} u2_t;

static s1_t s1;
static s2_t s2;
static u1_t u1;
static u2_t u2;

/* We use "variables" in the comparisons, so that we can avoid "constant
** comparison" and "Unreachable code" warnings (the second one currently
** can't be suppressed).
*/

static size_t const four = 4;
static size_t const seven = 7;
static size_t const ten = 10;

int main(void)
{
    /* Check the types' sizes. */

    if (sizeof (s1_t) != ten) {
        printf("s1_t size is %u; it should be 10.\n", sizeof (s1_t));
        ++fails;
    }
    if (sizeof (s2_t) != seven) {
        printf("s2_t size is %u; it should be 7.\n", sizeof (s2_t));
        ++fails;
    }
    if (sizeof (u1_t) != seven) {
        printf("u1_t size is %u; it should be 7.\n", sizeof (u1_t));
        ++fails;
    }
    if (sizeof (u2_t) != four) {
        printf("u2_t size is %u; it should be 4.\n", sizeof (u2_t));
        ++fails;
    }

    /* Check the variables' sizes. */

    if (sizeof s1 != ten) {
        printf("s1 size is %u; it should be 10.\n", sizeof s1);
        ++fails;
    }
    if (sizeof s2 != seven) {
        printf("s2 size is %u; it should be 7.\n", sizeof s2);
        ++fails;
    }
    if (sizeof u1 != seven) {
        printf("u1 size is %u; it should be 7.\n", sizeof u1);
        ++fails;
    }
    if (sizeof u2 != four) {
        printf("u2 size is %u; it should be 4.\n", sizeof u2);
        ++fails;
    }

    return fails;
}
