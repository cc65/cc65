/*
  !!DESCRIPTION!! A small test for asrax7.
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
    signed int ai = -32768, ti, refi;
    signed char ac = -128, tc, refc;

    do {
      refi = ai >> 4;
      refi = refi >> 3;

      ti   = ai >> 7;

      if (ti != refi) {
        printf("wrong result on int %d >> 7: %04X, expected %04X\n", ai, ti, refi);
        return 1;
      }
    } while (++ai != -32768);

    do {
      refc = ac >> 4;
      refc = refc >> 3;

      tc   = ac >> 7;

      if (tc != refc) {
        printf("wrong result on char %d >> 7: %04X, expected %04X\n", ac, tc, refc);
        return 1;
      }
    } while (++ac != -128);

    return 0;
}
