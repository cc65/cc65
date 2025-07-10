
/* bug #1989 - OptStackOps Opt_a_toscmpbool bypassed a comparison, discovered in 544a49c */

#include <stdlib.h>

unsigned char i,r,j;

void fail() // for the r=0 case, the == comparison was getting jumped over by OptStackOps
{
    if ((i & 0x1f) == (r ? 0 : 16)) j -=8;
}

void pass()
{
    if ((i & 0x1f) == (unsigned char)(r ? 0 : 16)) j -= 8;
}

void test(unsigned char ti, unsigned char tr, unsigned char tj)
{
    unsigned char rj;
    i = ti;
    r = tr;
    j = tj;
    pass();
    rj = j;
    i = ti;
    r = tr;
    j = tj;
    fail();
    if (j != rj) exit(1);
}

int main(void)
{
    test( 1,0,33);
    test( 0,0,33);
    test( 1,1,33);
    test(16,1,33);
    return 0;
}
