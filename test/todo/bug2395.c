
/* bug #927: format specifiers related to leading zeros do not work as expected */

/* expected output:
0023
0023
-0023
-023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char a, b;
unsigned char c = 199;
unsigned char d = 100;

/* Bitwise operators with a boolean expression fail when optimized */
int main(void) {
    int fails = 0;

    a = c ^ (d != 0);
    b = c ^ 1;

    printf("%u ^ (%u != 0) => %u\n", c, d, a);
    if (a != b) {
      printf("XOR error: a %d instead of %d\n", a, b);
      fails++;
    }

    a = c | (d != 0);
    b = c | 1;

    printf("%u | (%u != 0) => %u\n", c, d, a);
    if (a != b) {
      printf("OR error: a %d instead of %d\n", a, b);
      fails++;
    }

    a = c & (d != 0);
    b = c & 1;

    printf("%u & (%u != 0) => %u\n", c, d, a);
    if (a != b) {
      printf("AND error: a %d instead of %d\n", a, b);
      fails++;
    }
    printf("%d errors\n", fails);

    /* Force exit failure, otherwise the -g version, which works,
     * breaks the build
     */
    return 1;
}
