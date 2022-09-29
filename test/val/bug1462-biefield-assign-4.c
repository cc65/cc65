
/* issue #1462 - Bit-fields are still broken */
/* When (un-)signedness involves with integral promotion */

#include <stdio.h>
#include <limits.h>

#define SMALL_WIDTH 4
#define LARGE_WIDTH (CHAR_BIT * sizeof (unsigned int))

typedef struct {
    unsigned int a : SMALL_WIDTH;
    unsigned int b : SMALL_WIDTH;
    unsigned int c : SMALL_WIDTH;
    unsigned int d : SMALL_WIDTH;
} T1;

typedef struct {
    unsigned int a : LARGE_WIDTH;
    unsigned int b : LARGE_WIDTH;
    unsigned int c : LARGE_WIDTH;
    unsigned int d : LARGE_WIDTH;
} T2;


int failures1 = 0;
int failures2 = 0;

void test1(void)
{
    T1 a = { 0, 0, 0, 0 };

    printf("\nunsigned int : %d\n", SMALL_WIDTH);
    if (!(~a.a < 0)) {
        ++failures1;
    }
    printf("~a.a < 0 : %d\n", ~a.a < 0);
    if (!(0 > ~a.b)) {
        ++failures1;
    }
    printf("0 > ~a.b : %d\n",0 > ~a.b);
    if (!(a.c > -1)) {
        ++failures1;
    }
    printf("a.c > -1 : %d\n", a.c > -1);
    if (!(-1 < a.d)) {
        ++failures1;
    }
    printf("-1 < a.d : %d\n", -1 < a.d);

    printf("Failures: %d\n", failures1);
}

void test2(void)
{
    T1 b = { 0, 0, 0, 0 };

    printf("\nunsigned int : %d\n", LARGE_WIDTH);
    if (!(~b.a < 0)) {
        ++failures2;
    }
    printf("~b.a < 0 : %d\n", ~b.a < 0);
    if (!(0 > ~b.b)) {
        ++failures2;
    }
    printf("0 > ~b.b : %d\n", 0 > ~b.b);
    if (!(b.c > -1)) {
        ++failures2;
    }
    printf("b.c > -1 : %d\n", b.c > -1);
    if (!(-1 < b.d)) {
        ++failures2;
    }
    printf("-1 < b.d : %d\n", -1 < b.d);

    printf("Failures: %d\n", failures2);
}

int main(void)
{
    test1();
    test2();
    return failures1 + failures2;
}

