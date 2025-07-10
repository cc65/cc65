
/* bug #1094 - Nested struct/union initializers don't compile */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint16_t u16;
typedef uint8_t u8;

struct WW {
    int a : 4;
    struct {
        unsigned int b : 4;
        unsigned int c : 8;
    } x[2];
} wwqq = { 0, {2, 5, {3, 4}}, };

typedef struct {
    u16 quot;
    u16 rem;
} udiv_t;

typedef struct {
    u16 quot;
    u16 rem;
    char m[8];
} big_t;

union U {
    struct {
        signed int a : 3;
        signed int b : 3;
        signed int c : 3;
    };
    int u;
};

union U g = { 5, 3, 1 };

struct S {
    struct {
        unsigned int a : 3;
        unsigned int b : 3;
        unsigned int c : 3;
    };
};

struct S h = { 5, 3, 1 };

union X {
    struct {
        uint16_t a : 3;
        union {
            struct {
                uint16_t b : 3;
                uint16_t c : 3;
            };
            uint16_t d;
        };
    };
    uint16_t e;
} x = { 4, {5, 6} };


udiv_t div3(udiv_t u)
{
    udiv_t v = {};

    u.quot = 341 + u.quot;
    u.rem = 1 + u.rem;

    v = u;

    return v;
}

int main(void)
{
    udiv_t v = { 141, 32 };
    big_t  b = { 141, 32 };

    v = div3(*(udiv_t*)&b);

    printf("%d %d %d\n", (int)wwqq.a, wwqq.x[0].b, wwqq.x[0].c);
    printf("%d %d %d\n", (int)wwqq.a, wwqq.x[1].b, wwqq.x[1].c);
    printf("quot = %u, rem = %u\n", div3(v).quot, div3(v).rem);
    printf("quot = %u, rem = %u\n", v.quot, v.rem);
    printf("quot = %u, rem = %u\n", b.quot, b.rem);
    printf("g.a = %u, g.b = %u, g.c = %d\n", g.a, g.b, g.c);
    x.e = 1467;
    printf("x.a = %d, x.b = %d, x.c = %d\n", x.a, x.b, x.c);
    printf("(long)x.b = %ld, sizeof(x) = %u, sizeof((long)x.a) = %u\n", (long)x.b, sizeof(x), sizeof((long)x.a));
    printf("-x.d = %d, (long)(-x.c + 1) = %ld\n", -x.d, (long)(-x.c + 1));
    printf("h.a = %u, h.b = %u, h.c = %u\n", h.a, h.b, h.c);

    return 0;
}
