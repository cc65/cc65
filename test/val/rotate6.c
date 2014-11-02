/*
  !!DESCRIPTION!! Shift bytes left and right by a variable.
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
signed short aint0 = 0;
signed short aint1 = 0;

#else
signed int aint0 = 0;
signed int aint1 = 0;

#endif

#else
signed int aint0 = 0;
signed int aint1 = 0;

#endif
signed char achar0 = 0;
signed char achar1 = 0;
signed char achar2 = 0;
signed char achar3 = 0;

void done()
{
  dummy++;
}

void shift_right_var(void)
{
  achar0 >>= achar1;
}

void shift_left_var(void)
{
  achar0 <<= achar1;
}

void shift_int_left_var(void)
{
  aint0 <<= achar1;
}

void shift_int_right_var(void)
{
  aint0 >>= achar1;
}

void shift_int_right_var2(void)
{
  aint0 = aint1 >> achar1;
}

void shift_int_left_var2(void)
{
  aint0 = aint1 << achar1;
}

int
main (void)
{
  char i;

  achar0 = 1;
  achar1 = 1;
  shift_left_var();

  if(achar0 !=2)
    failures++;
  printf("failures: %d\n",failures);

  achar0 = 1;
  achar1 = 1;
  achar2 = 1;
  for(i=0; i<7; i++) {
    shift_left_var();
    achar2 <<= 1;

    if(achar2 != achar0)
      failures++;
  }
  printf("failures: %d\n",failures);

  achar1 = 4;
  achar0 = 0xf0;
  shift_right_var();
  if(((unsigned char)achar0) != 0xff)
    failures++;
  printf("failures: %d\n",failures);

  aint0 = 1;
  aint1 = 1;
  achar1 = 1;

  for(i=0; i<15; i++) {
    shift_int_left_var();
    aint1 <<= 1;
    if(aint0 != aint1)
      failures++;
  }
  printf("failures: %d\n",failures);

  aint0 = 0x4000;
  aint1 = 0x4000;

  for(i=0; i<15; i++) {
    shift_int_right_var();
    aint1 >>= 1;
    if(aint0 != aint1)
      failures++;
  }
  printf("failures: %d\n",failures);

  aint0 = -0x4000;
  aint1 = -0x4000;

  for(i=0; i<15; i++) {
    shift_int_right_var();
    aint1 >>= 1;
    if(aint0 != aint1)
      failures++;
  }
  printf("failures: %d\n",failures);

  aint1 = 0xf000;
  achar1 = 10;
  shift_int_right_var2();

  if(((unsigned short)aint0) != 0xfffc)
    failures++;
  printf("failures: %d\n",failures);

  aint1 = aint0;
  shift_int_left_var2();

  if(((unsigned short)aint0) != 0xf000)
    failures++;

  success=failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
