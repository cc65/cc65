/* Test of bug: https://github.com/cc65/cc65/issues/2060 */

#include <stdio.h>

#define W 320

unsigned long test1(unsigned char* p, unsigned long n)
{
    (void)p;
    return n;
}

unsigned long test0(unsigned char* p, int x, int y, unsigned char b)
{
    (void)b;
    return test1(p, (long)y * W + x);
}

#define TEST(ta,tb) \
    expect = (long)tb * W + ta; \
    result = test0(p,ta,tb,0x56); \
    printf("%4d * %3d + %4d = %08lx",tb,W,ta,result); \
    if (expect != result) { printf(" expected: %08lx\n",expect); ++fail; } \
    else printf("\n");

int main(void)
{
    unsigned char* p = (unsigned char*)0x1234;
    unsigned long expect, result;
    int fail = 0;

    TEST(1,3);
    TEST(50,60);
    TEST(99,88);
    TEST(128,102);
    TEST(129,102);
    TEST(320,102);
    /* Bug 2060 indicated failure when y > 102.
       Because: (y * 320) > 32767
       The promotion of x from int to long had an incorrect high word,
       because it was done before loading x into AX, rather than after.
    */
    TEST(0,103);
    TEST(150,170);
    TEST(300,180);
    /* x < 0 also fails because its high word sign extend is incorrect. */
    TEST(-100,50);
    TEST(-49,99);
    TEST(-300,-180);
    /* This passed despite the bug, because y * 320 coincidentally had the
       same high word.
    */
    TEST(-1,-1);

    return fail;
}
