
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("entering endless loop\n");
    for(;;) {
        ;
    }
    printf("error: should not come here\n");
    return EXIT_FAILURE;
}
