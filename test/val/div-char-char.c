#include <stdint.h>
#include <stdio.h>
#include "div-common.h"

int res = 0;

/* we check A_8 and B_8 signed */
#define TEST(_n,_a,_b,_r) TEST_AB_8(_n,_a,_b,_r)

#define DO_TEST_A(_n) res += test##_n##a()
#define DO_TEST_B(_n) res += test##_n##b()

/* arbitrary values */
TEST(1,  1,  8,  0)
TEST(2, -1,  8,  0)
TEST(3,  1, -8,  0)
TEST(4, -1, -8,  0)
TEST(5,  8,  1,  8)
TEST(6, -8,  1, -8)
TEST(7,  8, -1, -8)
TEST(8, -8, -1,  8)

TEST(11,  32,  64,  0)
TEST(12, -32,  64,  0)
TEST(13,  32, -64,  0)
TEST(14, -32, -64,  0)
TEST(15,  64,  32,  2)
TEST(16, -64,  32, -2)
TEST(17,  64, -32, -2)
TEST(18, -64, -32,  2)

/* +128 can't be tested for 8-bit signed char */
TEST(101,  127, -128,  0)
TEST(102, -127, -128,  0)
TEST(103, -128, -128,  1)

int main(void)
{
    /* check if the result is correct */
    DO_TEST_A(1);
    DO_TEST_A(2);
    DO_TEST_A(3);
    DO_TEST_A(4);
    DO_TEST_A(5);
    DO_TEST_A(6);
    DO_TEST_A(7);
    DO_TEST_A(8);

    DO_TEST_A(11);
    DO_TEST_A(12);
    DO_TEST_A(13);
    DO_TEST_A(14);
    DO_TEST_A(15);
    DO_TEST_A(16);
    DO_TEST_A(17);
    DO_TEST_A(18);

    DO_TEST_A(101);
    DO_TEST_A(102);
    DO_TEST_A(103);

    /* check if the results are equal */
    DO_TEST_B(1);
    DO_TEST_B(2);
    DO_TEST_B(3);
    DO_TEST_B(4);
    DO_TEST_B(5);
    DO_TEST_B(6);
    DO_TEST_B(7);
    DO_TEST_B(8);

    DO_TEST_B(11);
    DO_TEST_B(12);
    DO_TEST_B(13);
    DO_TEST_B(14);
    DO_TEST_B(15);
    DO_TEST_B(16);
    DO_TEST_B(17);
    DO_TEST_B(18);

    DO_TEST_B(101);
    DO_TEST_B(102);
    DO_TEST_B(103);

    return res;
}
