
/* test for bug#1374 */

#include <stdint.h>
#include <stdio.h>

static int res = 0;

int test1(void)
{
    uint8_t x = 0x89;
    uint8_t y = 0xab;
    uint16_t z = (x << 8) | y;
    printf("%x\n", z);
    return (z == 0x89ab) ? 0 : 1;
}

int test1b(void)
{
    uint8_t x = 0x89;
    uint8_t y = 0xab;
    uint16_t z = (x * 256) | y;
    printf("%x\n", z);
    return (z == 0x89ab) ? 0 : 1;
}

int test2(void)
{
    uint16_t x = 0x8900;
    uint8_t y = 0xab;
    uint16_t z = x | y;
    printf("%x\n", z);
    return (z == 0x89ab) ? 0 : 1;
}

int test3(void)
{
    uint16_t x = 0x89;
    uint8_t y = 0xab;
    uint16_t z = (x << 8) | y;
    printf("%x\n", z);
    return (z == 0x89ab) ? 0 : 1;
}

int test3b(void)
{
    uint16_t x = 0x89;
    uint8_t y = 0xab;
    uint16_t z = (x * 256) | y;
    printf("%x\n", z);
    return (z == 0x89ab) ? 0 : 1;
}

int test4(void)
{
    uint8_t x = 0x89;
    uint16_t y = 0xab;
    uint16_t z = (x << 8) | y;
    printf("%x\n", z);
    return (z == 0x89ab) ? 0 : 1;
}

int test4b(void)
{
    uint8_t x = 0x89;
    uint16_t y = 0xab;
    uint16_t z = (x * 256) | y;
    printf("%x\n", z);
    return (z == 0x89ab) ? 0 : 1;
}

int main(void)
{
    res |= test1();
    res |= test2();
    res |= test3();
    res |= test4();
    res |= test1b();
    res |= test3b();
    res |= test4b();
    printf("res: %d\n", res);
    return res;
}
