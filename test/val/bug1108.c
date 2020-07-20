
/* bug #1108 - Problem with static locals? */

#include <stdio.h>

#pragma static-locals (on)

unsigned char x = 0;

static unsigned char PrintVar1(void)
{
    unsigned char cx = x + 1;

    printf("cx:%d x:%d\n", cx, x);
    return cx == 0;
}

static unsigned char PrintVar2(void)
{
    unsigned char cx = x + 1;
    unsigned char cy;

    cy = x + 1;
    printf("cx:%d cy:%d x:%d\n", cx, cy, x);
    return cx != cy;
}

static unsigned char ret = 0;

int main(void)
{
    do {
        ret |= PrintVar1();
        ret |= PrintVar2();
    } while (++x < 10);
    return ret;
}
