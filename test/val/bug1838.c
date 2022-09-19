/* Bug 1838 - function parameters declared as function types rather than function pointers */

#include <stdio.h>
 
static int failures = 0;

typedef int fn_t(int);
 
int main(void)
{
    void foo(fn_t*);
    fn_t bar;

    foo(bar);
    return failures;
}

void foo(int func(int))
{
    int n = func(42);

    if (n != 12) {
        printf("n = %d, expected: 12\n", n);
        ++failures;
    }
}

int bar(int a)
{
    if (a != 42) {
        printf("a = %d, expected: 42\n", a);
        ++failures;
    }
    return 12;
}
