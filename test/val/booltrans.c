/* Optimization bugs with multiple inverse Z branches following one boolean transformer */

#include <stdint.h>
#include <stdio.h>

unsigned failures;

int a;

/* To reveal the bug, the second Z branch must jump over the destination of the first Z branch */

int test_booltrans(int8_t x)
{
    a = x;
    __asm__("lda #$00");
    __asm__("cmp %v", a);
    __asm__("jsr booleq");
    __asm__("jeq %g", L1);
    __asm__("jne %g", L0);
L1:
    return 1;
L0:
    return 0;
}

int test_bnega2(int8_t x)
{
    a = x;
    __asm__("lda %v", a);
    __asm__("jsr bnega");
    __asm__("jeq %g", L1);
    __asm__("jne %g", L0);
L1:
    return 1;
L0:
    return 0;
}

int test_bnegax2(int16_t x)
{
    int a = x;
    __asm__("ldy #%o+1", a);
    __asm__("jsr ldaxysp");
    __asm__("jsr bnegax");
    __asm__("jeq %g", L1);
    __asm__("jne %g", L0);
L1:
    return 1;
L0:
    return 0;
}

void __fastcall__ f(void) {}

int test_bnegax3(int16_t x)
{
    a = x;
    __asm__("lda %v", a);
    __asm__("ldx %v+1", a);
    __asm__("jsr %v", f);
    __asm__("jsr bnegax");
    __asm__("jeq %g", L1);
    __asm__("jne %g", L0);
L1:
    return 1;
L0:
    return 0;
}

int test_bnegax4(int16_t x)
{
    a = x;
    __asm__("lda %v", a);
    __asm__("ldx %v+1", a);
    __asm__("jsr bnegax");
    __asm__("jeq %g", L1);
    __asm__("jne %g", L0);
L1:
    return 1;
L0:
    return 0;
}

int main(void)
{
    a = test_booltrans(0);
    if (a != 0)
    {
        ++failures;
        printf("test_booltrans(0): %d, expected: 0\n", a);
    }

    a = test_booltrans(1);
    if (a != 1)
    {
        ++failures;
        printf("test_booltrans(1): %d, expected: 1\n", a);
    }

    a = test_bnega2(0);
    if (a != 0)
    {
        ++failures;
        printf("test_bnega2(0): %d, expected: 0\n", a);
    }

    a = test_bnega2(1);
    if (a != 1)
    {
        ++failures;
        printf("test_bnega2(1): %d, expected: 1\n", a);
    }

    a = test_bnegax2(0);
    if (a != 0)
    {
        ++failures;
        printf("test_bnegax2(0): %d, expected: 0\n", a);
    }

    a = test_bnegax2(1);
    if (a != 1)
    {
        ++failures;
        printf("test_bnegax2(1): %d, expected: 1\n", a);
    }

    a = test_bnegax3(0);
    if (a != 0)
    {
        ++failures;
        printf("test_bnegax3(0): %d, expected: 0\n", a);
    }

    a = test_bnegax3(1);
    if (a != 1)
    {
        ++failures;
        printf("test_bnegax3(1): %d, expected: 1\n", a);
    }

    a = test_bnegax4(0);
    if (a != 0)
    {
        ++failures;
        printf("test_bnegax4(0): %d, expected: 0\n", a);
    }

    a = test_bnegax4(1);
    if (a != 1)
    {
        ++failures;
        printf("test_bnegax4(1): %d, expected: 1\n", a);
    }

    if (failures > 0)
    {
        printf("failures: %u\n", failures);
    }
    return failures;
}
