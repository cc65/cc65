/* bug #264 - cc65 fails to warn about a function returning struct */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint32_t u32;
typedef uint16_t u16;

/* this struct is too large, we can only handle max 4 bytes right now */
typedef struct {
    u32 quot;
    u32 rem;
} udiv_t;

udiv_t div3(u32 in) {

    udiv_t u;
    u32 q = 0;

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

    return u;   /* error */
}

int res = 0;

int main(void) {

    u32 i;
    div_t d;
    udiv_t u;

    for (i = 1024; i; i--) {
        d = div((u16)i, 3);
        u = div3(i);

        if (d.quot != u.quot || d.rem != u.rem) {
            printf("Mismatch at %u/3, div %u %u, div3 %u %u\n", i,
                d.quot, d.rem, u.quot, u.rem);
            res++;
        }
    }

    return res;
}
