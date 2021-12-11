/*
  !!DESCRIPTION!! equality problem
  !!ORIGIN!!      Testsuite
  !!LICENCE!!     Public Domain
*/

/*
    Different results, depending on whether constant is on left or right side.

    The optimizer sometimes makes code that executes the right-side expression
    as eight bits; but then, tests it against the left-side zero as 16 bits.
    The high-byte is garbage; therefore, that test might, or might not, work.
    It depends on the platform and the amount of optimization.

    http://www.cc65.org/mailarchive/2014-10/11680.html
    http://www.cc65.org/mailarchive/2014-10/11682.html
    http://www.cc65.org/mailarchive/2014-10/11683.html
*/

#include <stdio.h>

static unsigned char fails = 4;
static unsigned char bad[3], good[3];

int main(void)
{
    unsigned char joy_state = 0x7e;
    unsigned a, b;

    /* NOTE: It fails in only the printf() statements, the other stuff
             below works! */
    printf("bad: %u, ", 0 == (joy_state & 1) );
    printf("good: %u\n", (joy_state & 1) == 0 );

    sprintf(bad, "%u", 0 == (joy_state & 1) );
    sprintf(good, "%u", (joy_state & 1) == 0 );

    printf("bad: %u, ", bad[0] - '0' );
    printf("good: %u\n", good[0] - '0' );

    fails -= bad[0] - '0';
    fails -= good[0] - '0';

    if (0 == (joy_state & 1)) fails--;
    if ((joy_state & 1) == 0) fails--;

    printf("failures: %u\n", fails );

    /* The above printf() returns a value with a zero high-byte.
    ** Therefore, the next (broken) statement works (by accident).
    */
    a = 0 == (joy_state & 1);
    b = (joy_state & 1) == 0;

    printf("a: %u, ", a );
    printf("b: %u\n", b );

    return fails;
}
