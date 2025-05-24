#include <stdio.h>
#include <stdlib.h>

/* this should succeed on all three standards
 * yet use only \e on CC65
 */
int main(void) {

#if __CC65_STD__ == __CC65_STD_CC65__
    printf("\e");
#endif

    return EXIT_SUCCESS;
}
