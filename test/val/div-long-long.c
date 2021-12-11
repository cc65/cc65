#include <stdint.h>
#include <stdio.h>
#include "div-common.h"

int res = 0;

/* we check A_32 and B_32 signed */
#define TEST(_n,_a,_b,_r) TEST_AB_32(_n,_a,_b,_r)

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

TEST(11,  2048,   512,  4)
TEST(12, -2048,   512, -4)
TEST(13,  2048,  -512, -4)
TEST(14, -2048,  -512,  4)
TEST(15,   512,  2048,  0)
TEST(16,  -512,  2048,  0)
TEST(17,   512, -2048,  0)
TEST(18,  -512, -2048,  0)

/* values that are around min/max of the type(s) */
TEST(101,  127,  128,  0)
TEST(102, -127,  128,  0)
TEST(103,  127, -128,  0)
TEST(104, -127, -128,  0)
TEST(105,  128,  128,  1)
TEST(106,  128, -128, -1)
TEST(107, -128,  128, -1)
TEST(108, -128, -128,  1)

TEST(201,  32767L,  32768L,  0)
TEST(202, -32767L,  32768L,  0)
TEST(203,  32767L, -32768L,  0)
TEST(204,  32767L, -32768L,  0)
TEST(205,  32768L,  32768L,  1)
TEST(206,  32768L, -32768L, -1)
TEST(207, -32768L,  32768L, -1)
TEST(208, -32768L, -32768L,  1)

/* +2147483648 can't be tested for 32-bit signed long */
TEST(401,  2147483647UL, -2147483648UL,  0)
TEST(402, -2147483647UL, -2147483648UL,  0)
TEST(403, -2147483648UL, -2147483648UL,  1)

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
    DO_TEST_A(104);
    DO_TEST_A(105);
    DO_TEST_A(106);
    DO_TEST_A(107);
    DO_TEST_A(108);

    DO_TEST_A(201);
    DO_TEST_A(202);
    DO_TEST_A(203);
    DO_TEST_A(204);
    DO_TEST_A(205);
    DO_TEST_A(206);
    DO_TEST_A(207);
    DO_TEST_A(208);

    DO_TEST_A(401);
    DO_TEST_A(402);
    DO_TEST_A(403);

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
    DO_TEST_B(104);
    DO_TEST_B(105);
    DO_TEST_B(106);
    DO_TEST_B(107);
    DO_TEST_B(108);

    DO_TEST_B(201);
    DO_TEST_B(202);
    DO_TEST_B(203);
    DO_TEST_B(204);
    DO_TEST_B(205);
    DO_TEST_B(206);
    DO_TEST_B(207);
    DO_TEST_B(208);

    DO_TEST_B(401);
    DO_TEST_B(402);
    DO_TEST_B(403);

    return res;
}
