/* bug #3038: Ternary operator with a constant condition applied the type
** conversion meant for the eliminated operand to the result.
*/

#include <stdio.h>
#include <stdlib.h>

static int failures = 0;

/* Operands that need a type conversion */

long ternary_0_u_x(long x)
{
    unsigned u = 42000U;
    return 0 ? u : x;
}

long ternary_1_x_u(long x)
{
    unsigned u = 42000U;
    return 1 ? x : u;
}

long ternary_0_x_u(long x)
{
    unsigned u = 42000U;
    return 0 ? x : u;
}

long ternary_1_u_x(long x)
{
    unsigned u = 42000U;
    return 1 ? u : x;
}

unsigned long ternary_0_i_ul(unsigned long ul)
{
    int i = -1;
    return 0 ? i : ul;
}

unsigned long ternary_1_ul_i(unsigned long ul)
{
    int i = -1;
    return 1 ? ul : i;
}

unsigned long ternary_0_ul_i(unsigned long ul)
{
    int i = -1;
    return 0 ? ul : i;
}

/* Operands needing no conversion code */

static int ia = -1234;
static int ib = 5678;
static unsigned ua = 0xFFFEU;
static unsigned ub = 0x1234U;
static signed char sca = -5;
static signed char scb = 100;
static unsigned char uca = 200;
static unsigned char ucb = 7;
static long la = -100000L;
static long lb = 0x12345678L;
static unsigned long ula = 0xFEDCBA98UL;
static unsigned long ulb = 0x00010002UL;

int ternary_0_const(void)
{
    return 0 ? 3 : 4;
}

int ternary_1_const(void)
{
    return 1 ? 3 : 4;
}

#define CHECK(expr, expect) \
    res = (expr); \
    if (res != (expect)) { \
        ++failures; \
        printf("%s: Got %d, expected %d\n", #expr, res, (expect)); \
    }

#define CHECK_L(expr, expect) \
    if ((expr) != (expect)) { \
        ++failures; \
        printf("%s: Got %ld, expected %ld\n", #expr, (long)(expr), (long)(expect)); \
    }

int main(void)
{
    int res;
    long sum;

    CHECK_L(ternary_0_u_x(-42L), -42L);
    CHECK_L(ternary_1_x_u(-42L), -42L);
    CHECK_L(ternary_0_u_x(0x12345678L), 0x12345678L);
    CHECK_L(ternary_1_x_u(0x12345678L), 0x12345678L);
    CHECK_L(ternary_0_x_u(-42L), 42000L);
    CHECK_L(ternary_1_u_x(-42L), 42000L);
    CHECK_L(ternary_0_i_ul(0x12345678UL), 0x12345678UL);
    CHECK_L(ternary_1_ul_i(0x12345678UL), 0x12345678UL);
    CHECK_L(ternary_0_ul_i(0x12345678UL), (unsigned long)-1);

    CHECK(0 ? ia : ib, 5678);
    CHECK(1 ? ia : ib, -1234);
    CHECK(0 ? ua : ub, 0x1234);
    CHECK(1 ? ua : ub, 0xFFFE);
    CHECK(0 ? sca : scb, 100);
    CHECK(1 ? sca : scb, -5);
    CHECK(0 ? uca : ucb, 7);
    CHECK(1 ? uca : ucb, 200);
    CHECK_L(0 ? la : lb, 0x12345678L);
    CHECK_L(1 ? la : lb, -100000L);
    CHECK_L(0 ? ula : ulb, 0x00010002UL);
    CHECK_L(1 ? ula : ulb, 0xFEDCBA98UL);
    CHECK(ternary_0_const(), 4);
    CHECK(ternary_1_const(), 3);

    CHECK(0 ? ia : ub, 0x1234);
    CHECK(1 ? ia : ub, -1234);
    CHECK_L(0 ? la : ulb, 0x00010002UL);
    CHECK_L(1 ? la : ulb, (unsigned long)-100000L);
    CHECK(0 ? sca : uca, 200);
    CHECK(1 ? sca : uca, -5);

    /* With code for another operand already emitted before the ?: */
    sum = (0 ? la : lb) + (1 ? la : lb);
    CHECK_L(sum, 0x12345678L - 100000L);

    printf("failures: %d\n", failures);

    if (failures != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
