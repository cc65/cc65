/*
  !!DESCRIPTION!!
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
unsigned int aint0 = 0;
unsigned int aint1 = 0;
unsigned char achar0 = 0;
unsigned char achar1 = 0;

void done()
{
  dummy++;
}

void switch1(void)
{
  switch(achar0) {
  case 0:
    achar0 = 9;
    break;
  case 1:
    achar0 = 18;
    break;

  default:
    achar0 = 0;
  }
}

void switch2(void)
{
  switch(achar1) {
  case 0:    achar0 = 9;    break;
  case 1:    achar0 = 8;    break;
  case 2:    achar0 = 7;    break;
  case 3:    achar0 = 6;    break;
  case 4:    achar0 = 5;    break;
  case 5:    achar0 = 4;    break;
  case 6:    achar0 = 3;    break;
  case 7:    achar0 = 2;    break;
  case 8:    achar0 = 1;    break;
  case 9:    achar0 = 0;    break;
  case 10:    achar0 = 9;    break;
  case 11:    achar0 = 8;    break;
  case 12:    achar0 = 7;    break;
  default:    achar0 = 0xff;    break;
  }
}

int main(void)
{
  achar0 = 0;
  switch1();
  if(achar0 != 9)
    failures++;

  switch1();
  if(achar0 != 0)
    failures++;

  achar0++;

  switch1();
  if(achar0 != 18)
    failures++;

  for(achar1=0; achar1<10;achar1++){
    switch2();
    if(achar0 != (9-achar1))
      failures++;

  }

  success=failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
