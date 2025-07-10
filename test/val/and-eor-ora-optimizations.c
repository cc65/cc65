/* Test some new optimization passes */

#include <stdio.h>

static unsigned failures = 0;

int func0()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (a & 0x00FF) & (b & 0xFF00);
}

int func1()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (0x00FF & a) & (0xFF00 & b);
}

int func2()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (a | 0x00FF) & (b | 0xFF00);
}

int func3()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (0x00FF | a) & (0xFF00 | b);
}

int func4()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (a | 0x00FF) | (b | 0xFF00);
}

int func5()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (0x00FF | a) | (0xFF00 | b);
}

int func6()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (a ^ 0x00FF) & (b & 0xFF00);
}

int func7()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (0x00FF ^ a) & (0xFF00 & b);
}

int func8()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (a | 0x00FF) | (b ^ 0xFF00);
}

int func9()
{
    unsigned a = 0x1234, b = 0x55AA;
    return (0x00FF | a) | (0xFF00 ^ b);
}

void onetest(unsigned count, int (*f1)(void), int (*f2)(void), int result)
{
    int r1 = f1();
    int r2 = f2();
    if (r1 != result || r2 != result) {
        printf("Test %u failed! Expected 0x%04X but got 0x%04X/0x%04X\n",
               count, result, r1, r2);
        ++failures;
    }
}

int main()
{
    onetest(1, func0, func1, 0x0000);
    onetest(2, func2, func3, 0x12AA);
    onetest(3, func4, func5, 0xFFFF);
    onetest(4, func6, func7, 0x1000);
    onetest(5, func8, func9, 0xBAFF);
    return failures;
}
