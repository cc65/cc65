/* bug #1075 Internal compiler error */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long rhs;

int test(void)
{
    /* the whole lhs is errorneously treated as an absolute address (integer 
       constant) neglecting its dereference */
    return *(char *)0xD77C + rhs;
}

int res;

int main(void)
{
    memset(*(char *)0xD76C, 11, 0x80);
    rhs = 0x10;
    *(char *)(0xD77C + rhs) = 13;
    *(char *)0xD77C = 23;
    *(char *)0xD78C = 42;
    res = test();
    printf("res: %d\n", res);
    if (res != (23 + 0x10)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
