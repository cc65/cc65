/* C99 inline */

#include <stdlib.h>

inline static int f(int x, ...)
{
    return x * 2;
}

extern inline int g(int x);

int main(void)
{
    return f(g(7)) == 42 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int g(int x)
{
    return x * 3;
}
