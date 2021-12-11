
/*

This tests a couple of expressions which yield constant results. While we cant
really check if the compiler figures out they are constant, we can still check
if they are being compiled/evaluated correctly.

related:

pr #1424 - More compile-time constant expressions regarding object addresses
issue #1196 - Constant expressions in general 

*/

#include <stdio.h>

int fails = 0;

#define TESTEXPR(expr) \
    if (!(expr)) { \
        printf("fail line %d\n", __LINE__); \
        fails++; \
    }

#define TESTEXPRFALSE(expr) \
    if (expr) { \
        printf("fail line %d\n", __LINE__); \
        fails++; \
    }

int a;
volatile int b;
const int c = 1;
#define d 1
enum { e = 1 };
int f() { return 1; }

/* we cant really test these at runtime (because the result is constant, but not
 * compile-time known), so compile only */
void test0(void)
{
    TESTEXPR(a);              /* Pure: Yes; Static: No;   Immutable: No;  Compile-Time-Known: No   */
    TESTEXPR(b);              /* Pure: No?; Static: No;   Immutable: No;  Compile-Time-Known: No   */
    TESTEXPR(&a > &b);        /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: No   */
}

void test1(void)
{
    TESTEXPR(1);              /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: Yes  */
    TESTEXPR(c);              /* Pure: Yes; Static: ???;  Immutable: ???; Compile-Time-Known: ???  */
    TESTEXPR(d);              /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: Yes  */
    TESTEXPR(e);              /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: Yes  */
    TESTEXPR(c == c);         /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: Yes  */
    TESTEXPRFALSE(c != c);
    TESTEXPR(f() == f());     /* Pure: Yes; Static: Yes?; Immutable: Yes; Compile-Time-Known: Yes? */
    TESTEXPRFALSE(f() != f());
    TESTEXPR(&a == &a);       /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: Yes  */
    TESTEXPRFALSE(&a != &a);
    TESTEXPR(&a != 0);        /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: Yes* */
    TESTEXPRFALSE(&a == 0);
/* in a real program we cant rely on these, but in this test we can */
    TESTEXPR(&a);             /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: No   */
    TESTEXPR((int)&a != 0);   /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: No   */
    TESTEXPRFALSE((int)&a == 0);
    TESTEXPR(&a != &b);       /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: ??** */
    TESTEXPRFALSE(&a == &b);
/* this may fail in a real world program, but here we can rely on it anyway */
    TESTEXPR(b == b);         /* Pure: No?; Static: No;   Immutable: No;  Compile-Time-Known: No   */
    TESTEXPRFALSE(b != b);
/* NOT detected by the compiler as constant */
    TESTEXPR(a == a);         /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: Yes  */
    TESTEXPRFALSE(a != a);
    TESTEXPR(f());            /* Pure: Yes; Static: Yes?; Immutable: Yes; Compile-Time-Known: Yes? */
}

/* Taken from #1196 reply */
struct S {
    int a;
    int b;
} s[2];

void test2(void)
{
    TESTEXPR((void*)&s == (void*)&s[0]);
    TESTEXPRFALSE((void*)&s != (void*)&s[0]);
    TESTEXPR(&s[0] < &s[1]);
    TESTEXPR(&s[0].b > &s[0].a);
    TESTEXPR(&s[0].b < &s[1].a);
}

/* we abuse the close function here, close(-1) will return -1 */
extern int close(int fd);

void test3(void)
{
    TESTEXPR(close(-1));            /* Pure: No;  Static: No;   Immutable: No;  Compile-Time-Known: No   */
    TESTEXPR((close(-1), 1))        /* Pure: No;  Static: No;   Immutable: Yes; Compile-Time-Known: Yes  */
/* Error: Scalar expression expected */
//    TESTEXPR((void)close(-1));      /* Pure: No;  Static: No;   Immutable: Yes; Compile-Time-Known: Yes  */
    TESTEXPR(sizeof(close(-1)));    /* Pure: Yes; Static: Yes;  Immutable: Yes; Compile-Time-Known: Yes  */
    /* NOT detected by the compiler as constant */
    TESTEXPRFALSE(close(-1) * 0);   /* Pure: No;  Static: No;   Immutable: Yes; Compile-Time-Known: Yes  */
}

int main(void)
{
    test1();
    test2();
    test3();
    return fails;
}
