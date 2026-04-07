/* related to bug #2461 */

/* Note: The values for MASK1, MASK2, the return values of GarbleAX and the 
 * arguments for CALC() are carefully chosen to elicit the bug.
 * CALCLX() errors appear with cc65 -Osi optimizations.
 */

#include <stdio.h>

#define MASK1           0x000FU
#define MASK2           0x00FFU
#define CALCLA(num, op) (((num) & (~MASK1)) op ((num) & MASK2))
/* + 0x100 here invokes g_inc(), case CF_INT: if (val <= 0x300), when
**  CodeSizeFactor >= 200; then g_inc() produces a single "inx"
*/
#define CALCLX(num, op) (((num) + 0x100) op ((num) & MASK2))
#define CALCRX(num, op) (((num) & MASK2) op ((num) << 8))

#define CALCX(num, op)  (((num) + 0x100) op ((num) & (~MASK1)))


static unsigned Failures = 0;
static unsigned TestCount = 0;

unsigned GarbleAX(void)
{
    static const unsigned Garbage[] = {
        0x1234, 0x1234, 0x0000, 0x1234, 0x1234, /* Lhs A: Add, Sub, And, Or, Xor */
        0x0057, 0x0057, 0x0037, 0x0057,         /* Lhs A: Eq, Neq, Gte, Lte */

        0x1234, 0x1234, 0x2003, 0x1002, 0x5678, /* Lhs X: Add, Sub, And, Or, Xor */
        0x0101, 0xFF00, 0xFF00, 0xFF00,         /* Lhs X: Eq, Neq, Gte, Lte */

        0x1234, 0x1234, 0xFFFF, 0xFFFF, 0xFFFF, /* Rhs X: Add, Sub, And, Or, Xor */
    };
    return Garbage[TestCount - 1];
}

unsigned LhsAAdd(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, +);
}

unsigned LhsASub(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, -);
}

unsigned LhsAAnd(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, &);
}

unsigned LhsAOr(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, |);
}

unsigned LhsAXor(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, ^);
}


unsigned LhsAEq(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, ==);
}

unsigned LhsANeq(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, !=);
}

unsigned LhsAGte(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, >=);
}

unsigned LhsALte(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLA(num, <=);
}


unsigned LhsXAdd(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCX(num, +);
}

unsigned LhsXSub(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCX(num, -);
}

unsigned LhsXAnd(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCX(num, &);
}

unsigned LhsXOr(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLX(num, |);
}

unsigned LhsXXor(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLX(num, ^);
}


unsigned LhsXEq(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLX(num, ==);
}

unsigned LhsXNeq(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCX(num, !=);
}

unsigned LhsXGte(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLX(num, >=);
}

unsigned LhsXLte(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCLX(num, <=);
}


unsigned RhsXAdd(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCRX(num, +);
}

unsigned RhsXSub(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCRX(num, -);
}

unsigned RhsXAnd(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCRX(num, &);
}

unsigned RhsXOr(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCRX(num, |);
}

unsigned RhsXXor(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALCRX(num, ^);
}

void Test(unsigned (*F)(unsigned), unsigned Num, unsigned Ref)
{
    unsigned Res;
    ++TestCount;
    Res = F(Num);
    if (Res != Ref) {
        printf("Test %u failed: got %04X, expected %04X\n", TestCount, Res, Ref);
        ++Failures;
    }
}

int main(void)
{
    /* Test 1+ */
    Test(LhsAAdd, 0x4715, CALCLA(0x4715, +));
    Test(LhsASub, 0x4715, CALCLA(0x4715, -));
    Test(LhsAAnd, 0x4715, CALCLA(0x4715, &));
    Test(LhsAOr,  0x4715, CALCLA(0x4715, |));
    Test(LhsAXor, 0x4715, CALCLA(0x4715, ^));

    /* Test 6+ */
    Test(LhsAEq, 0x4750, CALCLA(0x4750, ==));
    Test(LhsANeq, 0x4750, CALCLA(0x4750, !=));
    Test(LhsAGte, 0x4750, CALCLA(0x4750, >=));
    Test(LhsALte, 0x4750, CALCLA(0x4750, <=));

    /* Test 10+ */
    Test(LhsXAdd, 0x3F15, CALCX(0x3F15, +));
    Test(LhsXSub, 0x3F15, CALCX(0x3F15, -));
    Test(LhsXAnd, 0x3F15, CALCX(0x3F15, &));
    Test(LhsXOr,  0x3F15, CALCLX(0x3F15, |));
    Test(LhsXXor, 0x3F15, CALCLX(0x3F15, ^));

    /* Test 15+ */
    Test(LhsXEq, 0xFF50, CALCLX(0xFF50, ==));
    Test(LhsXNeq, 0xFF50, CALCX(0xFF50, !=));
    Test(LhsXGte, 0xFF50, CALCLX(0xFF50, >=));
    Test(LhsXLte, 0xFF50, CALCLX(0xFF50, <=));

    /* Test 19+ */
    Test(RhsXAdd, 0x3F15, CALCRX(0x3F15, +));
    Test(RhsXSub, 0x3F15, CALCRX(0x3F15, -));
    Test(RhsXAnd, 0x3F15, CALCRX(0x3F15, &));
    Test(RhsXOr,  0x3F15, CALCRX(0x3F15, |));
    Test(RhsXXor, 0x3F15, CALCRX(0x3F15, ^));

    printf("Failures: %u\n", Failures);
    return Failures;
}
