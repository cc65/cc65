
/* issue #1462 - Bit-fields are still broken */
/* even the = operation is buggy in certain ways */

#include <stdio.h>

typedef struct {
    signed int a : 3;
    signed int b : 3;
    signed int c : 3;
} T;

int failures = 0;

T *f(T *t)
{
    t->a = 0;
    t->c = 0;
    return t;
}

void test(void)
{
    T a = { 7, 0, 7 };
    T *p = &a;

    a.b = f(p)->a;

    if (a.a != 0) {
        ++failures;
    }
    printf("%d\n", a.a);

    if (p->b != 0) {
        ++failures;
    }
    printf("%d\n", p->b);

    if ((&a)->c != 0) {
        ++failures;
    }
    printf("%d\n", (&a)->c);

    printf("Failures: %d\n", failures);
}

int main(void)
{
    test();
    return failures;
}
