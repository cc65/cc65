/* bug #1077 - Wrong code: a non-array constant address with a non-constant subscript  */

#include <stdlib.h>
#include <stdio.h>

int test1(void)
{
    int a = 0;
    (&a)[a] = 42;
    return a;
}
int test2(void)
{
    int a = 0;
    int b = 0;
    (&a)[b] = 42;
    return a;
}

int main(void)
{
    int res, ret = EXIT_SUCCESS;
    res = test1();
    printf("%d\n", res);
    if (res != 42) ret = EXIT_FAILURE;
    res = test2();
    printf("%d\n", res);
    if (res != 42) ret = EXIT_FAILURE;

    return ret;
}
