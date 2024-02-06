/*
  Copyright 2021-2022, The cc65 Authors

  This software is provided "as-is", without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications; and, to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated, but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
  Test of operations in unevaluated context resulted from 'sizeof' and
  short-circuited code-paths in AND, OR and conditional operations.

  See also:
  https://github.com/cc65/cc65/issues/1768#issuecomment-1175221466
*/

#include <stdio.h>

static int failures;

#define TEST(EXPR)\
    {\
        int acc = 0;\
        acc += sizeof((EXPR), 0);\
        acc += (0 && (EXPR));\
        acc += (1 || (EXPR));\
        acc += (0 ? (EXPR) : 0);\
        acc += (1 ? 0 : (EXPR));\
        if (acc == 0) {\
            printf("acc = %d\n", acc);\
            ++failures;\
        }\
    }

/* Division by zero/modulo with zero */
void test_1(void)
{
    int i;
    int j;
    TEST((i / 0) | (j % 0))
}

/* Division by zero/modulo with zero */
void test_2(void)
{
    int i;
    int j;
    TEST((i /= 0) | (j %= 0))
}

/* Shift by too wide counts */
void test_3(void)
{
    int i;
    int j;
    TEST((i << 32) | (j >> 32))
}

/* Shift by too wide counts */
void test_4(void)
{
    int i;
    int j;
    TEST((i <<= 32) | (j >>= 32))
}

/* Shift by negative counts */
void test_5(void)
{
    int i;
    int j;
    TEST((i << -1) | (j >> -1))
}

/* Shift by negative counts */
void test_6(void)
{
    int i;
    int j;
    TEST((i <<= -1) | (j >>= -1))
}

/* Shift bit-fields */
void test_7(void)
{
    struct S {
        long i : 24;    /* Will be promoted to 32-bit integer in calculation */
        long j : 8;     /* Will be promoted to 16-bit integer in calculation */
    } s;
    long k;

    s.i = 1;
    printf("%u\n", sizeof(s.i << 24));
    s.i = 2;
    k = s.i << 16;
    if (k != 0x00020000L) {
        printf("k = %ld, expected: %ld\n", k, 0x00020000L);
    }
    TEST(s.j >> 16)
}

/* Shift bit-fields */
void test_8(void)
{
    struct S {
        long i : 24;    /* Will be promoted to 32-bit integer in calculation */
        long j : 8;     /* Will be promoted to 16-bit integer in calculation */
    } s;
    long k;

    s.i = 3;
    printf("%u\n", sizeof(s.i << 24));
    s.i = 4;
    k = s.i <<= 16;
    if (k != 0x00040000L) {
        printf("k = %ld, expected: %ld\n", k, 0x00040000L);
    }
    TEST(s.j >>= 8)
}

/* Do all tests */
int main(void)
{
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    test_7();
    test_8();

    printf("Failures: %d\n", failures);
    return failures;
}
