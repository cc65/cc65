/* PR #1220 - test constant ternary, AND and OR */

#include <stdio.h>

/* test AND/OR, results as integers */
#define CONTEXT_A(x)    do {\
                        s = 0, flags = 0, t = (x),\
                        printf("%3d %2X: %d\n", s, flags, t);\
                        } while (0)

/* test AND/OR in ternary context */
#define CONTEXT_B(x)    do {\
                        s = 0, flags = 0, t = (x ? 42 : -42),\
                        printf("%3d %2X: %d\n", s, flags, t);\
                        } while (0)

int s, t;
unsigned flags;

int f(int x)
/* The call to this function should be and only be skipped strictly according to
** the short-circuit evaluation rule.
*/
{
    flags |= (x != 0) << s;
    ++s;
    return x;
}

#define _A f(a)
#define _B f(b)
#define _C f(c)
#define _D f(d)
#define _T (f(0), 256)
#define _F (f(256), 0)

void f0()
/* constant short-circuit */
{
    printf("f0()\n");

    CONTEXT_A(_T && _T && _T);
    CONTEXT_A(_F && _F && _F);

    CONTEXT_A(_T || _T || _T);
    CONTEXT_A(_F || _F || _F);

    CONTEXT_A(_T && _T || _T && _T);
    CONTEXT_A(_F && _F || _F && _F);
    CONTEXT_A(_T && _F || _T && _F);
    CONTEXT_A(_F && _T || _F && _T);

    CONTEXT_A((_T && _T) || (_T && _T));
    CONTEXT_A((_F && _F) || (_F && _F));
    CONTEXT_A((_T && _F) || (_T && _F));
    CONTEXT_A((_F && _T) || (_F && _T));

    CONTEXT_A((_T || _T) && (_T || _T));
    CONTEXT_A((_F || _F) && (_F || _F));
    CONTEXT_A((_T || _F) && (_T || _F));
    CONTEXT_A((_F || _T) && (_F || _T));

    printf("\n");
}

void f1(int a, int b, int c)
/* AND */
{
    printf("f1(%d, %d, %d)\n", a, b, c);

    CONTEXT_A(_A && _B && _C);

    CONTEXT_A(_T && _B && _C);
    CONTEXT_A(_A && _T && _C);
    CONTEXT_A(_A && _B && _T);

    CONTEXT_A(_F && _B && _C);
    CONTEXT_A(_A && _F && _C);
    CONTEXT_A(_A && _B && _F);

    CONTEXT_A(_T && _T && _C);
    CONTEXT_A(_A && _T && _T);
    CONTEXT_A(_T && _B && _T);

    printf("\n");
}

void f2(int a, int b, int c)
/* OR */
{
    printf("f2(%d, %d, %d)\n", a, b, c);

    CONTEXT_A(_A || _B || _C);

    CONTEXT_A(_T || _B || _C);
    CONTEXT_A(_A || _T || _C);
    CONTEXT_A(_A || _B || _T);

    CONTEXT_A(_F || _B || _C);
    CONTEXT_A(_A || _F || _C);
    CONTEXT_A(_A || _B || _F);

    CONTEXT_A(_F || _F || _C);
    CONTEXT_A(_A || _F || _F);
    CONTEXT_A(_F || _B || _F);

    printf("\n");
}

void f3(int a, int b, int c, int d)
/* AND and OR */
{
    printf("f3(%d, %d, %d, %d)\n", a, b, c, d);

    CONTEXT_A(_A && _B || _C && _D);
    CONTEXT_A(_T && _T || _C && _D);
    CONTEXT_A(_A && _B || _T && _T);

    CONTEXT_A(_T && _B || _C && _D);
    CONTEXT_A(_A && _T || _C && _D);
    CONTEXT_A(_A && _B || _T && _D);
    CONTEXT_A(_A && _B || _C && _T);

    CONTEXT_A(_F && _B || _C && _D);
    CONTEXT_A(_A && _F || _C && _D);
    CONTEXT_A(_A && _B || _F && _D);
    CONTEXT_A(_A && _B || _C && _F);

    printf("\n");
}

void f4(int a, int b, int c, int d)
/* AND as top-level expression inside OR context */
{
    printf("f4(%d, %d, %d, %d)\n", a, b, c, d);

    CONTEXT_A((_A && _B) || (_C && _D));
    CONTEXT_A((_T && _T) || (_C && _D));
    CONTEXT_A((_A && _B) || (_T && _T));

    CONTEXT_A((_T && _B) || (_C && _D));
    CONTEXT_A((_A && _T) || (_C && _D));
    CONTEXT_A((_A && _B) || (_T && _D));
    CONTEXT_A((_A && _B) || (_C && _T));

    CONTEXT_A((_F && _B) || (_C && _D));
    CONTEXT_A((_A && _F) || (_C && _D));
    CONTEXT_A((_A && _B) || (_F && _D));
    CONTEXT_A((_A && _B) || (_C && _F));

    printf("\n");
}

void f5(int a, int b, int c, int d)
/* OR as top-level expression inside AND context */
{
    printf("f5(%d, %d, %d, %d)\n", a, b, c, d);

    CONTEXT_A((_A || _B) && (_C || _D));
    CONTEXT_A((_F || _F) && (_C || _D));
    CONTEXT_A((_A || _B) && (_F || _F));

    CONTEXT_A((_T || _B) && (_C || _D));
    CONTEXT_A((_A || _T) && (_C || _D));
    CONTEXT_A((_A || _B) && (_T || _D));
    CONTEXT_A((_A || _B) && (_C || _T));

    CONTEXT_A((_F || _B) && (_C || _D));
    CONTEXT_A((_A || _F) && (_C || _D));
    CONTEXT_A((_A || _B) && (_F || _D));
    CONTEXT_A((_A || _B) && (_C || _F));

    printf("\n");
}

void f0_B()
/* constant short-circuit */
{
    printf("f0_B()\n");

    CONTEXT_B(_T && _T && _T);
    CONTEXT_B(_F && _F && _F);

    CONTEXT_B(_T || _T || _T);
    CONTEXT_B(_F || _F || _F);

    CONTEXT_B(_T && _T || _T && _T);
    CONTEXT_B(_F && _F || _F && _F);
    CONTEXT_B(_T && _F || _T && _F);
    CONTEXT_B(_F && _T || _F && _T);

    CONTEXT_B((_T && _T) || (_T && _T));
    CONTEXT_B((_F && _F) || (_F && _F));
    CONTEXT_B((_T && _F) || (_T && _F));
    CONTEXT_B((_F && _T) || (_F && _T));

    CONTEXT_B((_T || _T) && (_T || _T));
    CONTEXT_B((_F || _F) && (_F || _F));
    CONTEXT_B((_T || _F) && (_T || _F));
    CONTEXT_B((_F || _T) && (_F || _T));

    printf("\n");
}

void f1_B(int a, int b, int c)
/* AND */
{
    printf("f1_B(%d, %d, %d)\n", a, b, c);

    CONTEXT_B(_A && _B && _C);

    CONTEXT_B(_T && _B && _C);
    CONTEXT_B(_A && _T && _C);
    CONTEXT_B(_A && _B && _T);

    CONTEXT_B(_F && _B && _C);
    CONTEXT_B(_A && _F && _C);
    CONTEXT_B(_A && _B && _F);

    CONTEXT_B(_T && _T && _C);
    CONTEXT_B(_A && _T && _T);
    CONTEXT_B(_T && _B && _T);

    printf("\n");
}

void f2_B(int a, int b, int c)
/* OR */
{
    printf("f2_B(%d, %d, %d)\n", a, b, c);

    CONTEXT_B(_A || _B || _C);

    CONTEXT_B(_T || _B || _C);
    CONTEXT_B(_A || _T || _C);
    CONTEXT_B(_A || _B || _T);

    CONTEXT_B(_F || _B || _C);
    CONTEXT_B(_A || _F || _C);
    CONTEXT_B(_A || _B || _F);

    CONTEXT_B(_F || _F || _C);
    CONTEXT_B(_A || _F || _F);
    CONTEXT_B(_F || _B || _F);

    printf("\n");
}

void f3_B(int a, int b, int c, int d)
/* AND and OR */
{
    printf("f3_B(%d, %d, %d, %d)\n", a, b, c, d);

    CONTEXT_B(_A && _B || _C && _D);
    CONTEXT_B(_T && _T || _C && _D);
    CONTEXT_B(_A && _B || _T && _T);

    CONTEXT_B(_T && _B || _C && _D);
    CONTEXT_B(_A && _T || _C && _D);
    CONTEXT_B(_A && _B || _T && _D);
    CONTEXT_B(_A && _B || _C && _T);

    CONTEXT_B(_F && _B || _C && _D);
    CONTEXT_B(_A && _F || _C && _D);
    CONTEXT_B(_A && _B || _F && _D);
    CONTEXT_B(_A && _B || _C && _F);

    printf("\n");
}

void f4_B(int a, int b, int c, int d)
/* AND as top-level expression inside OR context */
{
    printf("f4_B(%d, %d, %d, %d)\n", a, b, c, d);

    CONTEXT_B((_A && _B) || (_C && _D));
    CONTEXT_B((_T && _T) || (_C && _D));
    CONTEXT_B((_A && _B) || (_T && _T));

    CONTEXT_B((_T && _B) || (_C && _D));
    CONTEXT_B((_A && _T) || (_C && _D));
    CONTEXT_B((_A && _B) || (_T && _D));
    CONTEXT_B((_A && _B) || (_C && _T));

    CONTEXT_B((_F && _B) || (_C && _D));
    CONTEXT_B((_A && _F) || (_C && _D));
    CONTEXT_B((_A && _B) || (_F && _D));
    CONTEXT_B((_A && _B) || (_C && _F));

    printf("\n");
}

void f5_B(int a, int b, int c, int d)
/* OR as top-level expression inside AND context */
{
    printf("f5_B(%d, %d, %d, %d)\n", a, b, c, d);

    CONTEXT_B((_A || _B) && (_C || _D));
    CONTEXT_B((_F || _F) && (_C || _D));
    CONTEXT_B((_A || _B) && (_F || _F));

    CONTEXT_B((_T || _B) && (_C || _D));
    CONTEXT_B((_A || _T) && (_C || _D));
    CONTEXT_B((_A || _B) && (_T || _D));
    CONTEXT_B((_A || _B) && (_C || _T));

    CONTEXT_B((_F || _B) && (_C || _D));
    CONTEXT_B((_A || _F) && (_C || _D));
    CONTEXT_B((_A || _B) && (_F || _D));
    CONTEXT_B((_A || _B) && (_C || _F));

    printf("\n");
}

int main()
{
    f0();

    f1(0, 0, 0);
    f2(0, 0, 0);
    f3(0, 0, 0, 0);
    f4(0, 0, 0, 0);
    f5(0, 0, 0, 0);

    f1(1, 1, 1);
    f2(1, 1, 1);
    f3(1, 1, 1, 1);
    f4(1, 1, 1, 1);
    f5(1, 1, 1, 1);

    f3(1, 0, 1, 0);
    f4(1, 0, 1, 0);
    f5(1, 0, 1, 0);
    f3(0, 1, 0, 1);
    f4(0, 1, 0, 1);
    f5(0, 1, 0, 1);

    f0_B();

    f1_B(0, 0, 0);
    f2_B(0, 0, 0);
    f3_B(0, 0, 0, 0);
    f4_B(0, 0, 0, 0);
    f5_B(0, 0, 0, 0);

    f1_B(1, 1, 1);
    f2_B(1, 1, 1);
    f3_B(1, 1, 1, 1);
    f4_B(1, 1, 1, 1);
    f5_B(1, 1, 1, 1);

    f3_B(1, 0, 1, 0);
    f4_B(1, 0, 1, 0);
    f5_B(1, 0, 1, 0);
    f3_B(0, 1, 0, 1);
    f4_B(0, 1, 0, 1);
    f5_B(0, 1, 0, 1);

    return 0;
}
