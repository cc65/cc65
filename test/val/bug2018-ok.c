/* Bug #2018 - Compiler has problems with const struct fields */

#include <stdio.h>

unsigned failures;

struct X {
    const int;                  /* Useless empty declaration */
    const void;                 /* Useless empty declaration */
    const struct U;             /* Useless(?) declaration */
    const struct V { int a; };  /* Useless(?) declaration */

    const struct {              /* Qualifier ignored in cc65 */
        int a;
    };
    const union {               /* Qualifier ignored in cc65 */
        int b;
    };
};

union Y {
    const int;                  /* Useless empty declaration */
    const void;                 /* Useless empty declaration */
    const union W;              /* Useless(?) declaration */
    const union T { int a; };   /* Useless(?) declaration */

    const struct {              /* Qualifier ignored in cc65 */
        int a;
    };
    const union {               /* Qualifier ignored in cc65 */
        int b;
    };
};

struct X f(struct X a)
{
    struct X x = { 42 };
    return --a.a ? a : x;
}

union Y g(union Y a)
{
    union Y y = { 42 };
    return --a.a ? a : y;
}

int main(void)
{
    struct X x = { 1 };
    union Y y = { 1 };

    x = f(x);                   /* Allowed in cc65 since X is not read only */
    y = g(y);                   /* Allowed in cc65 since Y is not read only */

    if (x.a != 42)
    {
        ++failures;
    }

    if (y.a != 42)
    {
        ++failures;
    }

    if (failures > 0)
    {
        printf("failures: %u\n", failures);
    }

    return failures;
}
