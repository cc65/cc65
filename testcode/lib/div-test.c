/* div-test.c
**
** This program tests the division and modulo operators
** and the div() library function.
**
** 2002-10-24, Greg King
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static bool test(int dividend, int divisor) {
        div_t result;

        result = div(dividend, divisor);
        printf("%+d/%+d= %+d, %+d%%%+d= %+d, div()= %+d, %+d\n",
                dividend, divisor, dividend / divisor,
                dividend, divisor, dividend % divisor,
                result.quot, result.rem);
        return result.quot * divisor + result.rem != dividend;
        }

int main(void) {
        bool t;

        printf("\nTest of division and modulus operations:\n\n");
        t =     test(+40, +3) ||
                test(+40, -3) ||
                test(-40, +3) ||
                test(-40, -3);
        if (t)
                printf("\nThe div() function made a wrong result!\n");

        printf("\nTap a key, to exit. ");
        getchar();
        return (int)t;
        }
