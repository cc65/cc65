
// bug #2172 - Invalid code generated for switch statement

#include <stdlib.h>
#include <stdio.h>

// cc65 -o bug2172.s -Cl -Oirs -T -t c64 bug2172.c
int func(int expr)
{
    switch (expr) {
        int i;
        case 0:
            i = 17;
            return i;
        default:
            i = 16;
            return i;
    }
}

int err = 0;

int main(void)
{
    int i = 0;
    int n = 42;
    for (i = -3; i < 0; i++) {
        n = func(i);
        if ((i < -3) || (i >= 0)) {
            goto stackerr;
        }
        printf("i:%d expect:16 got:%d\n", i, n);
        if (n != 16) {
            err++;
        }
    }
    n = func(0);
    printf("i:%d expect:17 got:%d\n", 0, n);
    if (n != 17) {
        err++;
    }
    for (i = 1; i < 4; i++) {
        n = func(i);
        if ((i < 1) || (i >= 4)) {
            goto stackerr;
        }
        printf("i:%d expect:16 got:%d\n", i, n);
        if (n != 16) {
            err++;
        }
    }
    return err;
stackerr:
    fputs("stack messed up?\n", stdout);
    return -1;
}
