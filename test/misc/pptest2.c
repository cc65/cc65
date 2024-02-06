
/* preprocessor test #2 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int y = 7;
int z[] = {1, 2, 3};
int res;

int f(int i) {
    printf("f: %d\n", i);
    return i + 1;
}
int t(int i) {
    printf("t: %d\n", i);
    return i + 1;
}
int m(int i, int j) {
    printf("m: %d %d\n", i, j);
    return i + j + 1;
}

#define x       3
#define f(a)    f(x * (a))
#undef  x
#define x       2
#define g       f
#define z       z[0]
#define h       g(~
#define m(a)    a(w)
#define w       0,1
#define t(a)    a
#define p()     int
#define q(x)    x
#define r(x,y)  x ## y
#define str(x)  # x

int main(void)
{
    res = f(y+1) + f(f(z)) % t(t(g) (0) + t)(1);
    printf("res: %d expected: 19\n", res);
    if (res != 19) {
        return EXIT_FAILURE;
    }

    res = g(x+(3,4)-w) | h 5) & m(f)^m(m);
    printf("res: %d expected: 3\n", res);
    if (res != 3) {
        return EXIT_FAILURE;
    }

    p() i[q()] = { q(1), r(2,3), r(4,), r(,5), r(,) };
    printf("i[]:      %d %d %d %d\n", i[0], i[1], i[2], i[3]);
    printf("expected: %d %d %d %d\n", 1, 23, 4, 5);
    if ((i[0] != 1) || (i[1] != 23) || (i[2] != 4) || (i[3] != 5)) {
        return EXIT_FAILURE;
    }

    char c[2][6] = { str(hello), str() };
    printf ("c[0]: %s expected: %s\n", c[0], "hello");
    printf ("c[1]: %s expected: %s\n", c[1], "");
    if (strcmp(c[0], "hello") != 0) {
        return EXIT_FAILURE;
    }
    if (strcmp(c[1], "") != 0) {
        return EXIT_FAILURE;
    }

    printf("all fine\n");
    return EXIT_SUCCESS;
}
