/* Note: This test is rather fragile since it depends on how the compiler
** handles certain sequences and return codes. It checks for exactly one
** optimizer problem introduced in #2937. If it fails in some future version,
** do not hesitate to remove it.
*/

#include <stdio.h>

static unsigned char test(unsigned v)
{
    (void)v;
    asm("jsr popax");
    asm("lda #0");
    return __A__;
}

int main(void)
{
    if (test(1)) {
        printf("Test failed\n");
        return 1;
    } else {
        printf("Test ok\n");
        return 0;
    }
}

