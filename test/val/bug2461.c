/* related to bug #2461 */

/* Note: The values for MASK1, MASK2, the return values of GarbleAX and the 
 * arguments for CALC() are carefully chosen to elicit the bug.
 */

#include <stdio.h>

#define MASK1           0x000FU
#define MASK2           0x00FFU
#define CALC(num, op)   (((num) & (~MASK1)) op ((num) & MASK2))

static unsigned Failures = 0;
static unsigned TestCount = 0;

unsigned GarbleAX(void)
{
    static const unsigned Garbage[] = {
        0x1234, 0x0000, 0x1234, 0x1234
    };
    return Garbage[TestCount - 1];
}

unsigned WrongAdd(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALC(num, +);
}

unsigned WrongAnd(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALC(num, &);
}

unsigned WrongOr(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALC(num, |);
}

unsigned WrongXor(unsigned num)
{
    unsigned ret=GarbleAX();
    return CALC(num, ^);
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
    Test(WrongAdd, 0x4715, CALC(0x4715, +));
    Test(WrongAnd, 0x4715, CALC(0x4715, &));
    Test(WrongOr,  0x4715, CALC(0x4715, |));
    Test(WrongXor, 0x4715, CALC(0x4715, ^));
    printf("Failures: %u\n", Failures);
    return Failures;
}
