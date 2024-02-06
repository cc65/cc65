
/* test struct assignment, of structs with a length of 3, which happen to be
   a special case eg when passing/returning structs
   related to bugs #2022, #2079 */

#include <stdio.h>
#include <stdlib.h>

int failures = 0;

struct foo { char a; char b; char c; };
struct foo foo, bar;
void f3(void)
{
    foo.a = 6;
    foo.b = 6;
    foo.c = 6;
    bar.a = 1;
    bar.b = 2;
    bar.c = 3;
    foo = bar;
    printf("%d %d %d, %d %d %d (1,2,3 1,2,3)\n",
           foo.a, foo.b, foo.c,
           bar.a, bar.b, bar.c);
    if ((foo.a != 1) || (foo.b != 2) || (foo.c != 3) ||
        (bar.a != 1) || (bar.b != 2) || (bar.c != 3)) {
        failures++;
    }
    foo.a = 3;
    foo.b = 2;
    foo.c = 1;
    printf("%d %d %d, %d %d %d (3,2,1 1,2,3)\n",
           foo.a, foo.b, foo.c,
           bar.a, bar.b, bar.c);
    if ((foo.a != 3) || (foo.b != 2) || (foo.c != 1) ||
        (bar.a != 1) || (bar.b != 2) || (bar.c != 3)) {
        failures++;
    }
    bar.a = 5;
    bar.b = 6;
    bar.c = 7;
    printf("%d %d %d, %d %d %d (3,2,1 5,6,7)\n",
           foo.a, foo.b, foo.c,
           bar.a, bar.b, bar.c);
    if ((foo.a != 3) || (foo.b != 2) || (foo.c != 1) ||
        (bar.a != 5) || (bar.b != 6) || (bar.c != 7)) {
        failures++;
    }
    bar = foo;
    foo.a = 6;
    foo.b = 6;
    foo.c = 6;
    printf("%d %d %d, %d %d %d (6,6,6 3,2,1)\n",
           foo.a, foo.b, foo.c,
           bar.a, bar.b, bar.c);
    if ((foo.a != 6) || (foo.b != 6) || (foo.c != 6) ||
        (bar.a != 3) || (bar.b != 2) || (bar.c != 1)) {
        failures++;
    }
}

int main(void)
{
    f3();
    return failures;
}
