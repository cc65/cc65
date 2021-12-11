
/* preprocessor test #5 */

#define t(x,y,z) x ## y ## z 
int j[] = { t(1,2,3), t(,4,5), t(6,,7), t(8,9,),
            t(10,,), t(,11,), t(,,12), t(,,) };

int e[] = { 123, 45, 67, 89, 10, 11, 12, };

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unsigned char i;

int main(void)
{
    for (i = 0; i < 7; ++i) {
        printf("j: %d expect: %d\n", j[i], e[i]);
        if (j[i] != e[i]) return EXIT_FAILURE;
    }
    printf("all fine\n");
    return EXIT_SUCCESS;
}
