
// optimizer bug that occured after PR #2262, fixed by PR #2295

#include <stdio.h>

unsigned char n;
unsigned long fp1;

int failures = 0;

void test1(void)
{
	asm("lda     _n");
	asm("jeq     %g", L0004);

	asm("lda     #$3F");
	asm("sta     sreg+1");
	asm("lda     #$C0");
	asm("sta     sreg");
	asm("lda     #$00");
	asm("ldx     #$00");
	asm("jmp     %g", L0005);

L0004:
	asm("lda     #$3F");
	asm("sta     sreg+1");
	asm("lda     #$00");
	asm("sta     sreg");
	asm("lda     #$00");
	asm("ldx     #$00");

L0005:
	asm("sta     _fp1");
	asm("stx     _fp1+1");
	asm("ldy     sreg");
	asm("sty     _fp1+2");
	asm("ldy     sreg+1");
	asm("sty     _fp1+3");
}

int main(void)
{
    n = 0;
    test1();
    printf("fp1:%08lx\n", fp1);
    if (fp1 != 0x3f000000) failures++;
    n = 0xff;
    test1();
    printf("fp1:%08lx\n", fp1);
    if (fp1 != 0x3fc00000) failures++;
    printf("failures:%d\n", failures);
    return failures;
}
