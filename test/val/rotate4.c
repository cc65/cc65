/*
  !!DESCRIPTION!! rotate bytes left and right by a constant.
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

unsigned char success=0;
unsigned char failures=0;
unsigned char dummy=0;

#ifdef SUPPORT_BIT_TYPES
bit bit0 = 0;
#endif
#ifdef SIZEOF_INT_16BIT
#if defined(__LINUX__) || defined(LINUX)
unsigned short aint0 = 0;
unsigned short aint1 = 0;

#else
unsigned int aint0 = 0;
unsigned int aint1 = 0;

#endif

#else
unsigned int aint0 = 0;
unsigned int aint1 = 0;

#endif
unsigned char uchar0 = 0;
unsigned char uchar1 = 0;
unsigned char uchar2 = 0;

void done()
{
  dummy++;
}

void check(void)
{
  if(uchar0 != uchar1)
    failures++;
}

void rotate_left_1(void)
{
  uchar0 = (uchar0<<1) | (uchar0>>7);

  check();
}

void rotate_left_2(void)
{
  uchar0 = (uchar0<<2) | (uchar0>>6);

  check();
}

void rotate_left_3(void)
{
  uchar0 <<= 3;

  if(uchar0 != uchar1)
    failures++;
}

void rotate_left_4(void)
{
  uchar0 <<= 4;

  if(uchar0 != uchar1)
    failures++;
}

void rotate_left_5(void)
{
  uchar0 <<= 5;

  if(uchar0 != uchar1)
    failures++;
}

void rotate_left_6(void)
{
  uchar0 <<= 6;

  if(uchar0 != uchar1)
    failures++;
}

void rotate_left_7(void)
{
  uchar0 <<= 7;

  if(uchar0 != uchar1)
    failures++;
}

void rotate_right_1(void)
{
  uchar0 = (uchar0>>1) | (uchar0<<7);

  check();
}

void rotate_right_2(void)
{
  uchar0 = (uchar0>>2) | (uchar0<<6);

  check();
}

void rotate_right_3(void)
{
  uchar0 >>= 3;

  check();
}

void rotate_right_4(void)
{
  uchar0 >>= 4;

  check();
}

void rotate_right_5(void)
{
  uchar0 >>= 5;

  check();
}

void rotate_right_6(void)
{
  uchar0 >>= 6;

  check();
}

void rotate_right_7(void)
{
  uchar0 >>= 7;

  check();
}

int main(void)
{
  /* call with both values zero */
  rotate_left_1();

  uchar0 = 1;
  uchar1 = 2;

  rotate_left_1();

  uchar0 = 0x80;
  uchar1 = 1;

  rotate_left_1();

  uchar1 = 2;
  for(uchar2=0; uchar2<6; uchar2++) {
    rotate_left_1();
    uchar1 <<=1;
  }

  uchar0 = 1;
  uchar1 = 4;
  rotate_left_2();

  uchar0 = 1;
  uchar1 = 8;
  rotate_left_3();

  uchar0 = 1;
  uchar1 = 0x10;
  rotate_left_4();

  uchar0 = 1;
  uchar1 = 0x20;
  rotate_left_5();

  uchar0 = 1;
  uchar1 = 0x40;
  rotate_left_6();

  uchar0 = 1;
  uchar1 = 0x80;
  rotate_left_7();

  uchar0 = 2;
  uchar1 = 1;
  rotate_right_1();

  uchar0 = 1;
  uchar1 = 0x80;
  rotate_right_1();

  uchar0 = 4;
  uchar1 = 1;
  rotate_right_2();

  uchar0 = 8;
  rotate_right_3();

  uchar0 = 0x10;
  rotate_right_4();

  uchar0 = 0x20;
  rotate_right_5();

  uchar0 = 0x40;
  rotate_right_6();

  uchar0 = 0x80;
  rotate_right_7();

  success=failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
