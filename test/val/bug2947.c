
/* bug #2947: OptStackOps() attempts to remove non-removable Rhs X */

#include <stdio.h>
#include <stdlib.h>

unsigned char t8 = 0;
int t16 = -1;
unsigned char r8a, r8b;
int r16;
unsigned char c = 10;
unsigned char d = 1;

static int fail = 0;

void r8a_xor_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Keep X reg at 0 */
        t8 = 0;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8a = c ^ (d >= 0);

    if (r8a != 11) {
        ++fail;
        printf("r8a = c ^ (d >= 0) : fail\n");
    }
}

void r8b_xor_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8b = c ^ (d >= 0);

    if (r8b != 11) {
        ++fail;
        printf("r8b = c ^ (d >= 0) : fail\n");
    }
}

void r8a_or_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Keep X reg at 0 */
        t8 = 0;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8a = c | (d >= 0);

    if (r8a != 11) {
        ++fail;
        printf("r8a = c | (d >= 0) : fail\n");
    }
}

void r8b_or_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8b = c | (d >= 0);

    if (r8b != 11) {
        ++fail;
        printf("r8b = c | (d >= 0) : fail\n");
    }
}

void r8a_add_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Keep X reg at 0 */
        t8 = 0;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8a = c + (d >= 0);

    if (r8a != 11) {
        ++fail;
        printf("r8a = c + (d >= 0) : fail\n");
    }
}

void r8b_add_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8b = c + (d >= 0);

    if (r8b != 11) {
        ++fail;
        printf("r8b = c + (d >= 0) : fail\n");
    }
}

void r8a_sub_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Keep X reg at 0 */
        t8 = 0;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8a = c - (d >= 0);

    if (r8a != 9) {
        ++fail;
        printf("r8a = c - (d >= 0) : fail\n");
    }
}

void r8b_sub_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8b = c - (d >= 0);

    if (r8b != 9) {
        ++fail;
        printf("r8b = c - (d >= 0) : fail\n");
    }
}

void r8a_ge_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Keep X reg at 0 */
        t8 = 0;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8a = c >= (d >= 0);

    if (r8a != 1) {
        ++fail;
        printf("r8a = c >= (d >= 0) : fail\n");
    }
}

void r8b_ge_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8b = c >= (d >= 0);

    if (r8b != 1) {
        ++fail;
        printf("r8b = c >= (d >= 0) : fail\n");
    }
}

void r8a_le_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Keep X reg at 0 */
        t8 = 0;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8a = d <= (d >= 0);

    if (r8a != 1) {
        ++fail;
        printf("r8a = d <= (d >= 0) : fail\n");
    }
}

void r8b_le_test(void)
{
    /* 'if' needed to produce a label below, moved by OptJumpTarget3  */
    if (t8 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r8b = d <= (d >= 0);

    if (r8b != 1) {
        ++fail;
        printf("r8b = d <= (d >= 0) : fail\n");
    }
}

void r16_xor_test(void)
{
    /* 'if' needed to produce a label below */
    if (t16 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r16 = c ^ (d >= 0);

    if (r16 != 11) {
        ++fail;
        printf("r16 = c ^ (d >= 0) : fail\n");
    }
}

void r16_or_test(void)
{
    /* 'if' needed to produce a label below */
    if (t16 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r16 = c | (d >= 0);

    if (r16 != 11) {
        ++fail;
        printf("r16 = c | (d >= 0) : fail\n");
    }
}

void r16_add_test(void)
{
    /* 'if' needed to produce a label below */
    if (t16 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r16 = c + (d >= 0);

    if (r16 != 11) {
        ++fail;
        printf("r16 = c + (d >= 0) : fail\n");
    }
}

void r16_sub_test(void)
{
    /* 'if' needed to produce a label below */
    if (t16 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r16 = c - (d >= 0);

    if (r16 != 9) {
        ++fail;
        printf("r16 = c - (d >= 0) : fail\n");
    }
}

void r16_ge_test(void)
{
    /* 'if' needed to produce a label below */
    if (t16 != 0) {
        /* Clobber X reg */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r16 = c >= (d >= 0);

    if (r16 != 1) {
        ++fail;
        printf("r16 = c >= (d >= 0) : fail\n");
    }
}

void r16_le_test(void)
{
    /* 'if' needed to produce a label below */
    if (t16 != 0) {
        /* Keep X reg at 0 */
        t16 = -1;
    }

    /* d >= 0 is const, A/X=1 (Warning: Result of comparison is always true) */
    r16 = d <= (unsigned int)(d >= 0);

    if (r16 != 1) {
        ++fail;
        printf("r16 = d <= (d >= 0) : fail\n");
    }
}

int main(void)
{
    r8a_xor_test();
    r8b_xor_test();
    r8a_or_test();
    r8b_or_test();

    r8a_add_test();
    r8b_add_test();
    r8a_sub_test();
    r8b_sub_test();

    r8a_ge_test();
    r8b_ge_test();
    r8a_le_test();
    r8b_le_test();

    r16_xor_test();
    r16_or_test();

    r16_add_test();
    r16_sub_test();

    r16_ge_test();
    r16_le_test();

    return fail == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
