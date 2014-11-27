/*
  !!DESCRIPTION!! optimizer bug
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Oliver Schmidt
*/

#include <stdio.h>

/*
After spending a "little" time I finally succeeded in isolating an
(maybe THE) optimizer bug causing Contiki to fail.

From my user perspective it is very interesting that the bug shows up
with compiler option -O but does _not_ show up with -Oi.
*/

unsigned htons(unsigned val)
{
	return (((unsigned) (val)) << 8) | (((unsigned) (val)) >> 8);
}

int main(void)
{
    printf("%x -> %x\n", 0x1234, htons(0x1234) & 0xffff);

    return 0;
}
