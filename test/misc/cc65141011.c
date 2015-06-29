
/*
  !!DESCRIPTION!! equality problem
  !!ORIGIN!!      Testsuite
  !!LICENCE!!     Public Domain
*/

/*
    different result depending on whether constant is on left or right side

    http://www.cc65.org/mailarchive/2014-10/11680.html
    http://www.cc65.org/mailarchive/2014-10/11683.html
*/

#include <stdlib.h>
#include <stdio.h>

static unsigned char fails = 4;
static unsigned char bad[3], good[3];

int main(int n, char **args)
{
    unsigned char joy_state = 0x7e;
    unsigned a, b;

    /* NOTE: somehow it only fails in the printf statement, the other stuff
             below works! */
    printf("bad: %u\n", 0 == (joy_state & 1) );
    printf("good: %u\n", (joy_state & 1) == 0 );

    sprintf(bad, "%u", 0 == (joy_state & 1) );
    sprintf(good, "%u", (joy_state & 1) == 0 );

    printf("bad: %u\n", bad[0] - '0' );
    printf("good: %u\n", good[0] - '0' );

    fails -= bad[0] - '0';
    fails -= good[0] - '0';

    if (0 == (joy_state & 1)) fails--;
    if ((joy_state & 1) == 0) fails--;

    printf("fails: %u\n", fails );

    a = 0 == (joy_state & 1);
    b = (joy_state & 1) == 0;

    printf("a: %u\n", a );
    printf("b: %u\n", b );

    return fails;
}
