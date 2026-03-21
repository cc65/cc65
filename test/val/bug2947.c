
/* bug #2947: Opt_a_tosbitwise() attempts to remove non-removable Rhs X */

#include <stdio.h>
#include <stdlib.h>

unsigned char a, b = 0;
unsigned char c = 10;
unsigned char d = 1;

int main(void) {
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (b != 0) {
        /* Operation not important; it only affects the removal of one LDX #$00 */
        b = 0;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    a = c ^ (d >= 0);

    return !(a == 11);
}
