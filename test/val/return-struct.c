/* bug #264 - cc65 fails to warn about a function returning struct */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint16_t u16;
typedef uint8_t u8;

typedef struct {
    u16 quot;
    u16 rem;
} udiv_t;

udiv_t div3(u16 in) {

    udiv_t u;
    u16 q = 0;

    while (in >= 300) {
        in -= 300;
        q += 100;
    }

    while (in >= 30) {
        in -= 30;
        q += 10;
    }

    while (in >= 3) {
        in -= 3;
        ++q;
    }

    u.quot = q;
    u.rem = in;

    return u;
}

int res = 0;

int main(void) {

    u16 i;
    div_t d;
    udiv_t u;

    for (i = 1024; i; i--) {
        d = div(i, 3);
        u = div3(i);

        if (d.quot != u.quot || d.rem != u.rem) {
            printf("Mismatch at %u/3, div %u %u, div3 %u %u\n", i,
                d.quot, d.rem, u.quot, u.rem);
            res++;
        }
    }

    return res;
}

