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

#if SUPPORT_BIT_TYPES
bit bit0 = 0;
bit bit1 = 0;
bit bit2 = 0;
#endif

unsigned int uint0 = 0;
unsigned int uint1 = 0;
unsigned char uchar0 = 0;
unsigned char uchar1 = 0;
unsigned long ulong0 = 0;
unsigned long ulong1 = 0;

void done()
{
  dummy++;
}

/* uchar0 = 0; */
void or_lit2uchar(void)
{
  if(uchar0)
    failures++;

  uchar0 |= 1;

  if(uchar0 != 1)
    failures++;

  uchar0 |= 2;

  if(uchar0 != 3)
    failures++;

  uchar0 |= 0x0e;

  if(uchar0 != 0x0f)
    failures++;
}

void or_lit2uint(void)
{
  if(uint0)
    failures++;

  uint0 |= 1;
  if(uint0 != 1) 
    failures++;

  uint0 |= 2;
  if(uint0 != 3) 
    failures++;

  uint0 |= 0x100;
  if(uint0 != 0x103) 
    failures++;

  uint0 |= 0x102;
  if(uint0 != 0x103) 
    failures++;

  uint0 |= 0x303;
  if(uint0 != 0x303) 
    failures++;
}

void or_lit2ulong(void)
{
  if(ulong0)
    failures++;

  ulong0 |= 1;
  if(ulong0 != 1) 
    failures++;

  ulong0 |= 2;
  if(ulong0 != 3) 
    failures++;

  ulong0 |= 0x100;
  if(ulong0 != 0x103) 
    failures++;

  ulong0 |= 0x102;
  if(ulong0 != 0x103) 
    failures++;

  ulong0 |= 0x303;
  if(ulong0 != 0x303) 
    failures++;

  ulong0 |= 0x80000000;
  if(ulong0 != 0x80000303) 
    failures++;
}

/*-----------*/
void or_uchar2uchar(void)
{
  uchar0 |= uchar1;

  if(uchar0 != 1)
    failures++;

  uchar1 |= 0x0f;

  uchar0 = uchar1 | 0x10;

  if(uchar0 != 0x1f)
    failures++;
}

void or_uint2uint(void)
{
  uint0 |= uint1;

  if(uint0 != 1)
    failures++;

  uint1 |= 0x0f;

  uint0 = uint1 | 0x10;

  if(uint0 != 0x1f)
    failures++;
}

#if SUPPORT_BIT_TYPES

void or_bits1(void)
{
  bit0 = bit0 | bit1 | bit2;
}

void or_bits2(void)
{
  bit0 = bit1 | bit2;
}
#endif

int  main(void)
{
  or_lit2uchar();
  or_lit2uint();
  or_lit2ulong();

  uchar0=0;
  uchar1=1;
  or_uchar2uchar();

  uint0=0;
  uint1=1;
  or_uint2uint();

#if SUPPORT_BIT_TYPES
  or_bits1();
  if(bit0)
    failures++;

  or_bits2();
  if(bit0)
    failures++;

  bit1=1;
  or_bits1();
  if(!bit0)
    failures++;

  or_bits2();
  if(!bit0)
    failures++;
#endif

  success = failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
