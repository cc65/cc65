#include <stdio.h>
#include <stdlib.h>

/* this should fail on all three standards
 */
int main(void) {

#if __CC65_STD__ != __CC65_STD_CC65__
    printf("\e");
#else
#error "this needs to error on CC65 to make it through validation"
#endif
    return EXIT_SUCCESS;
}
