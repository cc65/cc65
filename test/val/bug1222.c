/* bug #1222 - 'sizeof' issues */

#include <stdlib.h>

int a[1];
int b[sizeof ++a[42]]; /* should work as '++a[42]' is actually unevaluated */

int main(void)
{
    return EXIT_SUCCESS;
}

