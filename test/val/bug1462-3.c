
/* issue #1462 - Bit-fields are still broken */
/* More tests on "op= expression result value" that a naive fix might fail with */

#include <stdio.h>

typedef struct {
    signed   int a : 3;
    unsigned int b : 3;
    signed   int c : 3;
    unsigned int d : 3;
} T1;

typedef struct {
    signed int a : 3;
    signed int b : 3;
    signed int c : 3;
    signed int d : 3;
} T2;


int failures1 = 0;
int failures2 = 0;

void test1(void)
{
    T1 a = { 3, 3, 3, 3 };
    int i;

    i = a.a -= a.b + a.c;
    if (i != -3 || a.a != -3) {
        ++failures1;
    }
    printf("i = %d, a.a = %d\n", i, a.a);

    a.b = i = a.b / -1;
    if (i != -3 || a.b != 5) {
        ++failures1;
    }
    printf("i = %d, a.b = %d\n", i, a.b);

    i = a.c = 0;
    if (i != 0 || a.c != 0) {
        ++failures1;
    }
    printf("i = %d, a.c = %d\n", i, a.c);

    i = a.d /= -1;
    if (i != 5 || a.d != 5) {
        ++failures1;
    }
    printf("i = %d, a.d = %d\n", i, a.d);

    printf("Failures: %d\n", failures1);
}

void test2(void)
{
    T2 b = { 3, 3, 4, 4 };
    int i;

    i = b.a++;
    if (i != 3 || b.a != -4) {
        ++failures2;
    }
    printf("i = %d, b.a = %d\n", i, b.a);

    i = ++b.b;
    if (i != -4 || b.b != -4) {
        ++failures2;
    }
    printf("i = %d, b.b = %d\n", i, b.b);

    i = b.c--;
    if (i != -4 || b.c != 3) {
        ++failures2;
    }
    printf("i = %d, b.c = %d\n", i, b.c);

    i = --b.d;
    if (i != 3 || b.d != 3) {
        ++failures2;
    }
    printf("i = %d, b.d = %d\n", i, b.d);

    printf("Failures: %d\n", failures2);
}

int main(void)
{
    test1();
    test2();
    return failures1 + failures2;
}

