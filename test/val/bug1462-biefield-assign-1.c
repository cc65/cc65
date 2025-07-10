
/* issue #1462 - Bit-fields are still broken */

#include <stdio.h>

typedef struct {
    signed int a : 3;
    signed int b : 3;
    signed int c : 3;
} T;

int failures = 0;

void test()
{
    T a = {2, 5, -1};
    T b = {1, 4, -1};
    T m[1] = {{6, 3, -1}};
    T *p = &a;

    a.c += b.a;
    p->c += b.b;
    m->c += b.c;

    if (a.c != -4) {
        ++failures;
    }
    printf("%d\n", a.c);

    if (p->c != -4) {
        ++failures;
    }
    printf("%d\n", p->c);

    if (m->c != -2) {
        ++failures;
    }
    printf("%d\n", m->c);

    ++a.a;
    p->b++;
    m->c--;

    if (a.a != 3) {
        ++failures;
    }
    printf("%d\n", a.a);

    if (p->b != -2) {
        ++failures;
    }
    printf("%d\n", p->b);

    if (m->c != -3) {
        ++failures;
    }
    printf("%d\n", m->c);

    printf("Failures: %d\n", failures);
}

int main(void)
{
    test();
    return failures;
}

