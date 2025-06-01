/* Bug #2302 - Parameters of function types not parsed correctly */

#include <stdio.h>

typedef int A;
int zoo(A ());                  /* OK: int zoo(int (*)()) */
int zoo(A (()));                /* OK: int zoo(int ((*)())) aka. int zoo(int (*)()) */
int zoo(A (A));                 /* OK: int zoo(int (*)(int)) */
int zoo(A ((A)));               /* OK: int zoo(int ((*)(int))) aka. int zoo(int (*)(int)) */
int zoo(A A(A));                /* OK: int zoo(int (*A)(int)) */
int zoo(A (*)(A));              /* OK: int zoo(int (*)(int)) */
int zoo(A (*A)(A));             /* OK: int zoo(int (*A)(int)) */
int zoo(A ((*A))(A));           /* OK: int zoo(int (*A)(int)) */
int zoo(A ((((*((fp))))(A A)))) /* OK: int zoo(int (*fp)(int A)) */
{
    return fp(42);
}

int bar(int a)
{
    return a ^ 42;
}

int main(void)
{
    int a = zoo((int (*)())bar);
    if (a != 0)
    {
        printf("failed: a = %d\n", a);
    }
    return a;
}
