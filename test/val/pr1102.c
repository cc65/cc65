
/* pr#1102 - Passing structs/unions <= 4 bytes to/from functions */

#include <stdlib.h>
#include <stdio.h>

typedef struct test1_s {
    char a;
    int b;
    char c;
} test1_t;

test1_t test1(int a, int b, int c)
{
    test1_t res;
    res.a = a;
    res.b = b;
    res.c = c;
    return res;
}

int test2(test1_t s)
{
    printf("a: %d b: %d c: %d\n", s.a, s.b, s.c);
    if ((s.a != 13) || (s.b != 4711) || (s.c != 99)) {
        return 0;
    }
    return 1;
}

typedef struct test2_s {
    char a;
    char b;
    char c;
    char d;
} test2_t;

test2_t test3(test1_t s)
{
    test2_t ret;
    printf("a: %d b: %d c: %d\n", s.a, s.b, s.c);
    ret.a = s.c;
    ret.b = s.b & 0xff;
    ret.c = s.b >> 8;
    ret.d = s.a;
    return ret;
}

int main(void) {
    test1_t t1;
    test2_t t2;

    t1 = test1(12, 1842, 23);
    printf("a: %d b: %d c: %d\n", t1.a, t1.b, t1.c);
    if ((t1.a != 12) || (t1.b != 1842) || (t1.c != 23)) {
        return EXIT_FAILURE;
    }

    t1.a = 13;
    t1.b = 4711;
    t1.c = 99;
    if (test2(t1) != 1) {
        return EXIT_FAILURE;
    }

    t1.a = 66;
    t1.b = 0x7788;
    t1.c = 22;
    t2 = test3(t1);
    printf("a: %d b: %d c: %d d: %d\n", t2.a, t2.b, t2.c, t2.d);
    if ((t2.a != 22) || (t2.b != 0x88) || (t2.c != 0x77) || (t2.d != 66)) {
        return EXIT_FAILURE;
    }

    printf("all fine\n");
    return EXIT_SUCCESS;
}
