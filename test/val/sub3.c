/*
  !!DESCRIPTION!! Subtraction Test
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

unsigned char failures=0;

int int0 = 5;
unsigned int int1 = 5;

void pre_dec_test(void)
{
  if(int0 != 5)
    failures++;
  if(int1 != 5)
    failures++;

  --int0;
  --int1;

  if(int0 != 4)
    failures++;
  if(int1 != 4)
    failures++;

  --int0;
  --int1;
  --int0;
  --int1;
  --int0;
  --int1;
  --int0;
  --int1;

  if(int0 != 0)
    failures++;
  if(int1 != 0)
    failures++;

  --int0;
  --int1;

  if(int0 != -1)
    failures++;
  if(int1 != 65535U)
    failures++;
}

void post_dec_test(void)
{
  if(int0 != 5)
    failures++;
  if(int1 != 5)
    failures++;

  int0--;
  int1--;

  if(int0 != 4)
    failures++;
  if(int1 != 4)
    failures++;

  int0--;
  int1--;
  int0--;
  int1--;
  int0--;
  int1--;
  int0--;
  int1--;

  if(int0 != 0)
    failures++;
  if(int1 != 0)
    failures++;

  int0--;
  int1--;

  if(int0 != -1)
    failures++;
  if(int1 != 65535U)
    failures++;
}

void pre_dec_assign_test(void)
{
  int a;
  unsigned int b;
  if(int0 != 5)
    failures++;
  if(int1 != 5)
    failures++;

  a = --int0;
  b = --int1;

  if(int0 != 4 || a != int0)
    failures++;
  if(int1 != 4 || b != int1)
    failures++;

  a = --int0;
  b = --int1;
  a = --int0;
  b = --int1;
  a = --int0;
  b = --int1;
  a = --int0;
  b = --int1;

  if(int0 != 0 || a != int0)
    failures++;
  if(int1 != 0 || b != int1)
    failures++;

  a = --int0;
  b = --int1;

  if(int0 != -1 || a != int0)
    failures++;
  if(int1 != 65535U || b != int1)
    failures++;
}

void post_dec_assign_test(void)
{
  int a;
  unsigned int b;
  if(int0 != 5)
    failures++;
  if(int1 != 5)
    failures++;

  a = int0--;
  b = int1--;

  if(int0 != 4 || a != 5)
    failures++;
  if(int1 != 4 || b != 5)
    failures++;

  a = int0--;
  b = int1--;
  a = int0--;
  b = int1--;
  a = int0--;
  b = int1--;
  a = int0--;
  b = int1--;

  if(int0 != 0 || a != 1)
    failures++;
  if(int1 != 0 || b != 1)
    failures++;

  a = int0--;
  b = int1--;

  if(int0 != -1 || a != 0)
    failures++;
  if(int1 != 65535U || b != 0)
    failures++;
}

void dex_tests(void) {
  static unsigned int a, b;

  a = 257;
  b = a - 1;
  if (b != 256) {
    printf("fail 257 => 256\n");
    failures++;
  }

  a = 256;
  b = a - 1;
  if (b != 255) {
    printf("fail 256 => 255\n");
    failures++;
  }

  a = 255;
  b = a - 1;
  if (b != 254) {
    printf("fail 255 => 254\n");
    failures++;
  }
}

int main(void)
{
  int0 = 5;
  int1 = 5;
  pre_dec_test();

  int0 = 5;
  int1 = 5;
  post_dec_test();

  int0 = 5;
  int1 = 5;
  pre_dec_assign_test();

  int0 = 5;
  int1 = 5;
  post_dec_assign_test();

  dex_tests();

  printf("failures: %d\n",failures);

  return failures;
}
