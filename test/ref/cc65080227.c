/*
  !!DESCRIPTION!! 
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!
*/

#include <stdio.h>

long a;  /* must be static life */
long b;  /* must be static life */

int main(void)
{
        a = 0x00112200;    /* must be immediate pattern is (1stBYTE == 4thBYTE) */
        b = a;
        /* b is 0x11112200 ! */

    printf("b (should be 0x00112200): %08lx\n",b);

    return 0;
}

/*
[ command line ]
    cl65 -c -T -l -O test.c

[ part of test.lst ]
    000012r 1               ; b = a;
    000012r 1  AD rr rr         lda     _a+2
    000015r 1  85 rr            sta     sreg
    000017r 1  AE rr rr         ldx     _a+1
    00001Ar 1  AD rr rr         lda     _a
    00001Dr 1  8D rr rr         sta     _b
    000020r 1  8E rr rr         stx     _b+1
    000023r 1  A4 rr            ldy     sreg
    000025r 1  8C rr rr         sty     _b+2
    000028r 1  8C rr rr         sty     _b+3    ; lost 4th BYTE !
*/