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

unsigned int uint0 = 0;
unsigned int uint1 = 0;
unsigned char uchar0 = 0;
unsigned char uchar1 = 0;
unsigned long ulong0 = 0;

void done()
{
  dummy++;
}

/* uchar0 = 0x13; */
void and_compound1(void)
{
  uchar0 = (uchar0 + 1) & 0x0f;
  if(uchar0 != 4)
    failures++;
}

/* uchar1 = 0x42; */
void and_compound2(void)
{
  uchar0 = (uchar1 + 1) & 0x0f;
  if(uchar0 != 3)
    failures++;

  if(uchar1 != 0x42)
    failures++;
}

/* uchar0 = 0x13; */
void or_compound1(void)
{
  uchar0 = (uchar0 + 0xe) | 0x0f;
  if(uchar0 != 0x2f)
    failures++;
}

/* uchar1 = 0x47; */
void or_compound2(void)
{
  uchar0 = (uchar1 + 0xf) | 0x0f;
  if(uchar0 != 0x5f)
    failures++;

  if(uchar1 != 0x47)
    failures++;
}

/* uchar0 = 0x13; */
void xor_compound1(void)
{
  uchar0 = (uchar0 + 1) ^ 0x0f;
  if(uchar0 != 0x1b)
    failures++;
}

/* uchar1 = 0x47; */
void xor_compound2(void)
{
  uchar0 = (uchar1 + 0xf) ^ 0x0f;
  if(uchar0 != 0x59)
    failures++;

  if(uchar1 != 0x47)
    failures++;
}

/* uchar0 = 0x13; */
void neg_compound1(void)
{
  uchar0 = ~(uchar0 + 1);
  if(uchar0 != 0xeb)
    failures++;
}

int main(void)
{
  uchar0 = 0x13;
  and_compound1();

  uchar1 = 0x42;
  and_compound2();

  uchar0 = 0x13;
  or_compound1();

  uchar1 = 0x47;
  or_compound2();

  uchar0 = 0x13;
  xor_compound1();

  uchar1 = 0x47;
  xor_compound2();

  uchar0 = 0x13;
  neg_compound1();

  success = failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
