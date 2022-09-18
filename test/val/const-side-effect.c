/* Check code generation for constant operands with side-effects */

#include <stdio.h>

static int failures = 0;

#define TEST(X, Y, L) \
    if (x != X || y != Y) { \
        printf("Failed: " L "\nExpected: x = " #X ", y = " #Y ", got: x = %d, y = %d\n\n", x, y); \
        ++failures; \
    }

#define TEST_LINE_UNARY(OP, RH, ID) \
    "x = " #OP "(set(&y, " #ID "), " #RH ")"

#define TEST_UNARY(OP, RH, RS, ID) \
    x = -!(RS), y = -!(RS); \
    x = OP (set(&y, ID), RH); \
    TEST(RS, ID, TEST_LINE_UNARY(OP, RH, ID))

#define TEST_LINE_RHS_EFFECT(LH, OP, RH, ID) \
    "x = " #LH " " #OP " (set(&y, " #ID "), " #RH ")"

#define TEST_LINE_LHS_EFFECT(LH, OP, RH, ID) \
    "y = (set(&x, " #ID "), " #LH ") " #OP " " #RH

#define TEST_BINARY(LH, OP, RH, RS, ID) \
    x = -!(RS), y = -!(RS); \
    x = LH OP (set(&y, ID), RH); \
    TEST(RS, ID, TEST_LINE_RHS_EFFECT(LH, OP, RH, ID)) \
    y = -!(RS), x = -!(RS); \
    y = (set(&x, ID), LH) OP RH; \
    TEST(ID, RS, TEST_LINE_LHS_EFFECT(LH, OP, RH, ID)) \
    y = -!(RS); \
    x = (set(&x, LH), x) OP (set(&y, ID), RH); \
    TEST(RS, ID, TEST_LINE_RHS_EFFECT((set(&x, LH), x), OP, RH, ID)) \
    x = -!(RS); \
    y = (set(&x, ID), LH) OP (set(&y, RH), y); \
    TEST(ID, RS, TEST_LINE_LHS_EFFECT(LH, OP, (set(&y, RH), y), ID))

#define TEST_LINE_RHS_EFFECT_WITH_CAST(LT, LH, OP, RT, RH, ID) \
    "x = (" #LT ")" #LH " " #OP " (" #RT ")(set(&y, " #ID "), " #RH ")"

#define TEST_LINE_LHS_EFFECT_WITH_CAST(LT, LH, OP, RT, RH, ID) \
    "y = (" #LT ")(set(&x, " #ID "), " #LH ") " #OP " (" #RT ")" #RH

#define TEST_BINARY_WITH_CAST(LT, LH, OP, RT, RH, RS, ID) \
    x = -!(RS), y = -!(RS); \
    x = (LT)LH OP (RT)(set(&y, ID), RH); \
    TEST(RS, ID, TEST_LINE_RHS_EFFECT_WITH_CAST(LT, LH, OP, RT, RH, ID)) \
    y = -!(RS), x = -!(RS); \
    y = (LT)(set(&x, ID), LH) OP (RT)RH; \
    TEST(ID, RS, TEST_LINE_LHS_EFFECT_WITH_CAST(LT, LH, OP, RT, RH, ID)) \
    y = -!(RS); \
    x = (LT)(set(&x, LH), x) OP (RT)(set(&y, ID), RH); \
    TEST(RS, ID, TEST_LINE_RHS_EFFECT_WITH_CAST(LT, (set(&x, LH), x), OP, RT, RH, ID)) \
    x = -!(RS); \
    y = (LT)(set(&x, ID), LH) OP (RT)(set(&y, RH), y); \
    TEST(ID, RS, TEST_LINE_LHS_EFFECT_WITH_CAST(LT, LH, OP, RT, (set(&y, RH), y), ID))

void set(int *p, int q)
{
    *p = q;
}

int twice(int a)
{
    return a * 2;
}

int (*twicep)(int) = twice;

void test_unary(void)
{
    int x, y;

    TEST_UNARY(+,  42, 42, 1);
    TEST_UNARY(-, -42, 42, 2);
    TEST_UNARY(~, ~42, 42, 3);
    TEST_UNARY(!,  42,  0, 4);
}

void test_binary_arithmetic(void)
{
    int x, y;

    TEST_BINARY(41, +, 1, 42, 1)
    TEST_BINARY(42, +, 0, 42, 1)

    TEST_BINARY(43, -, 1, 42, 2)
    TEST_BINARY(42, -, 0, 42, 2)

    TEST_BINARY(6,   *,  7, 42, 3)
    TEST_BINARY(42,  *,  1, 42, 3)
    TEST_BINARY(-42, *, -1, 42, 3)

    TEST_BINARY(126, /,  3, 42, 4)
    TEST_BINARY(42,  /,  1, 42, 4)
    TEST_BINARY(-42, /, -1, 42, 4)

    TEST_BINARY(85,    %,  43, 42, 5)
    TEST_BINARY(10794, %, 256, 42, 5)

    TEST_BINARY(84,    >>, 1, 42, 6)
    TEST_BINARY(42,    >>, 0, 42, 6)
    TEST_BINARY(10752, >>, 8, 42, 6)
    TEST_BINARY(21504, >>, 9, 42, 6)

    TEST_BINARY(21, <<, 1,    42, 7)
    TEST_BINARY(42, <<, 0,    42, 7)
    TEST_BINARY(42, <<, 8, 10752, 7)

    TEST_BINARY(59, &, 238, 42, 8)
    TEST_BINARY(42, &,   0,  0, 8)
    TEST_BINARY(42, &,  -1, 42, 8)

    TEST_BINARY(34, |, 10, 42, 9)
    TEST_BINARY(42, |,  0, 42, 9)
    TEST_BINARY(34, |, -1, -1, 9)

    TEST_BINARY(59,  ^, 17, 42, 10)
    TEST_BINARY(42,  ^,  0, 42, 10)
    TEST_BINARY(~42, ^, -1, 42, 10)
}

void test_binary_comparison(void)
{
    int x, y;

    TEST_BINARY(42, ==, 42, 1, 11)

    TEST_BINARY(42, !=, 43, 1, 12)
    TEST_BINARY_WITH_CAST(signed char, 42, !=, long, 65536L, 1, 12)
    TEST_BINARY_WITH_CAST(long, 65536L, !=, signed char, 42, 1, 12)

    TEST_BINARY(42, >, 41, 1, 13)
    TEST_BINARY_WITH_CAST(int, 0, >, unsigned, 42, 0, 13)

    TEST_BINARY(42, <, 43, 1, 14)
    TEST_BINARY_WITH_CAST(unsigned, 42, <, int, 0, 0, 14)

    TEST_BINARY(42, >=, 0, 1, 15)
    TEST_BINARY_WITH_CAST(unsigned, 42, >=, int, 0, 1, 15)

    TEST_BINARY(42, <=, 43, 1, 16)
    TEST_BINARY_WITH_CAST(int, 0, <=, unsigned, 42, 1, 16)
}

int main(void)
{
    test_unary();
    test_binary_arithmetic();
    test_binary_comparison();

    if (failures != 0) {
        printf("Failures: %d\n", failures);
    }

    return failures;
}
