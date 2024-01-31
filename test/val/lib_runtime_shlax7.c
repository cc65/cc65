/*
  !!DESCRIPTION!! A small test for shlax7.
  !!ORIGIN!!
  !!LICENCE!!
  !!AUTHOR!!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

int main (void)
{
    unsigned int ai = 0, ti, refi;
    unsigned char ac = 0, tc, refc;

    do {
      refi = ai << 4;
      refi = refi << 3;

      ti   = ai << 7;

      if (ti != refi) {
        printf("wrong result on int %u << 7: %04X, expected %04X\n", ai, ti, refi);
        return 1;
      }
    } while (++ai != 0);

    do {
      refc = ac << 4;
      refc = refc << 3;

      tc   = ac << 7;

      if (tc != refc) {
        printf("wrong result on char %u << 7: %04X, expected %04X\n", ac, tc, refc);
        return 1;
      }
    } while (++ac != 0);

    return 0;
}
