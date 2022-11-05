
/* #1853 - Regression on inline assembly expression evaluation */

int main(void)
{
/*
compiles with e.g. Git 2f4e2a3 to the expected

    lda     1
    lda     1 + 1
    rts

However, with the current HEAD, it compiles to

    lda     1
    lda
*/
    __asm__("lda 1");
    __asm__("lda 1 + 1");
    return 0;
}
