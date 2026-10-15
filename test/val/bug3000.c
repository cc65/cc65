/* bug #3000: Invalid optimizer transformations around shrax runtimes.
**  LDX #$FF instructions get erroneously removed due to incorrect
**  in/out register contents tracking in the optimizer.
*/

#include <stdio.h>
#include <stdlib.h>

static int failures = 0;

unsigned u1;

int shry_ff(unsigned char x, unsigned char y)
{
    unsigned a;

    a = (0xffu << 8) | x;
    u1 = a >> y;
    /* LDX #$FF must remain here after shrax */
    return (0xffu << 8) | x;
}

int shry_00(unsigned char x, unsigned char y)
{
    unsigned a;

    a = (0 << 8) | x;
    u1 = a >> y;
    /* LDX #$00 is removed here as unnecessary */
    return (0 << 8) | x;
}

int shr2_ff(unsigned char x)
{
    unsigned a;

    a = (0xffu << 8) | x;
    u1 = a >> 2;
    /* LDX #$FF must remain here after shrax */
    return (0xffu << 8) | x;
}

int shr2_00(unsigned char x)
{
    unsigned a;

    a = (0 << 8) | x;
    u1 = a >> 2;
    /* LDX #$00 is removed here as unnecessary */
    return (0 << 8) | x;
}

#define CHECK(func, expect) \
    res = (func); \
    if (res != (expect)) { \
        ++failures; \
        printf(#func ": Got %04x, expected %04x\n", res, (expect)); \
    }

int main(void)
{
    int res;

    CHECK(shry_ff(5, 3), 0xff05);
    CHECK(shry_00(5, 3), 0x0005);
    CHECK(shr2_ff(5), 0xff05);
    CHECK(shr2_00(5), 0x0005);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;    
}
