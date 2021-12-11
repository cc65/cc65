
/* issue #897 - __asm__()-referenced code-labels are generated for only branches and jumps */

#include <stdlib.h>
#include <stdio.h>

static unsigned char *srcptr, *dstptr;

#define COPY_LEN    16

void test(void)
{
    asm("lda %v", srcptr);
    asm("sta %g+1", s2b_copy_from);
    asm("lda %v+1", srcptr);
    asm("sta %g+2", s2b_copy_from);

    asm("lda %v", dstptr);
    asm("sta %g+1", s2b_copy_to);
    asm("lda %v+1", dstptr);
    asm("sta %g+2", s2b_copy_to);

    asm("ldy #%b", COPY_LEN-1);
s2b_copy_from:
    asm("lda $FFFF,y");
s2b_copy_to:
    asm("sta $FFFF,y");
    asm("dey");
    asm("bpl %g", s2b_copy_from);
}

unsigned char src[16] = "0123456789abcdef";
unsigned char dest[16];

int failures = 0;

unsigned char i;

int main(void)
{
    srcptr = src;
    dstptr = dest;
    test();
    for (i = 0; i < COPY_LEN; i++) {
        printf("%d %02x %02x\n", i, src[i], dest[i]);
        if (src[i] != dest[i]) {
            failures++;
        }
    }
    printf("failures: %d\n", failures);
    return failures;
}
