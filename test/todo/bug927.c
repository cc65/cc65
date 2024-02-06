
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

char b1[10];
char b2[10];
char b3[10];
char b4[10];

int main(void) {
    printf("%.4d\n", 23);
    printf("%04d\n", 23);
    printf("%.4d\n", -23);
    printf("%04d\n\n", -23);

    sprintf(b1, "%.4d", 23);
    sprintf(b2, "%04d", 23);
    sprintf(b3, "%.4d", -23);
    sprintf(b4, "%04d", -23);

    printf("%s\n", b1);
    printf("%s\n", b2);
    printf("%s\n", b3);
    printf("%s\n\n", b4);

    printf("%d\n", strcmp(b1, "0023"));
    printf("%d\n", strcmp(b2, "0023"));
    printf("%d\n", strcmp(b3, "-0023"));
    printf("%d\n", strcmp(b4, "-023"));

    if(strcmp(b1, "0023") != 0) return EXIT_FAILURE;
    if(strcmp(b2, "0023") != 0) return EXIT_FAILURE;
    if(strcmp(b3, "-0023") != 0) return EXIT_FAILURE;
    if(strcmp(b4, "-023") != 0) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
