
/* bug #1504 - Some compilation failures */

#include <stdio.h>

int main(void)
{
    int i = 0, *p = &i;
    switch (i) case 0: case 1: i = 21;  /* Should be OK but fails */
    p++[0] += 21;                       /* Should be OK but fails */
    printf("%d\n", i);
    return i != 42;
}
