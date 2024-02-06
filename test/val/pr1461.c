
/* pr#1461 Fixed pointer subtraction in certain very rare cases */

#include <stdlib.h>
#include <stdio.h>

static int err = 0;

static int a[1], *p;
static unsigned int i1, i2;

int test1(void)
{
    p = a - (int)a;
    printf("a: %p - (int)a: 0x%x = p: %p\n", a, (int)a, p);
    printf("i1: 0x%x - i2: 0x%x = p: %p\n", i1, i2, i1 - i2);
    if ((int)p != (i1 - i2)) {
        printf("-> failed\n");
        return 1;
    }
    return 0;
}

int test2(void)
{
    p = p - (int)a;
    printf("p: %p - (int)a: 0x%x = p: %p\n", p, (int)a, p);
    printf("p: %p - i2: 0x%x = p: %p\n", p, i2, 0x1234 - i2);
    if ((int)p != (0x1234 - i2)) {
        printf("-> failed\n");
        return 1;
    }
    return 0;
}

int main(void)
{
    a[0] = 0x4711;
    i1 = (int)a;
    i2 = i1 << 1;

    p = (int*)0x1234;
    printf("p: %p &a[0]: %p a: %p (int)a: 0x%x i1: 0x%x i2: 0x%x\n", p, &a[0], a, (int)a, i1, i2);

    err += test1();

    p = (int*)0x1234;
    printf("p: %p &a[0]: %p a: %p (int)a: 0x%x i1: 0x%x i2: 0x%x\n", p, &a[0], a, (int)a, i1, i2);

    err += test2();

    return err;
}
