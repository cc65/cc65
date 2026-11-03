/* bug #3038: Ternary operator with a constant condition applied the type
** conversion meant for the eliminated operand to the result.
*/

#include <stdio.h>
#include <stdlib.h>

static int failures = 0;

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

#define CHECK_L(expr, expect) \
    if ((expr) != (expect)) { \
        ++failures; \
        printf("%s: Got %ld, expected %ld\n", #expr, (long)(expr), (long)(expect)); \
    }

int main(void)
{
    CHECK_L(ternary_0_u_x(-42L), -42L);
    CHECK_L(ternary_1_x_u(-42L), -42L);
    CHECK_L(ternary_0_u_x(0x12345678L), 0x12345678L);
    CHECK_L(ternary_1_x_u(0x12345678L), 0x12345678L);
    CHECK_L(ternary_0_x_u(-42L), 42000L);
    CHECK_L(ternary_1_u_x(-42L), 42000L);
    CHECK_L(ternary_0_i_ul(0x12345678UL), 0x12345678UL);
    CHECK_L(ternary_1_ul_i(0x12345678UL), 0x12345678UL);
    CHECK_L(ternary_0_ul_i(0x12345678UL), 0xFFFFFFFFUL);

    printf("failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}