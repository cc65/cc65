/*
  !!DESCRIPTION!! Signed comparisons of the form:  (variable<=LIT)
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

/*
 This regression test exercises all of the boundary
 conditions in literal less than or equal comparisons. There
 are numerous opportunities to optimize these comparison
 and each one has an astonishing capability of failing
 a boundary condition.
*/

unsigned char success = 0;
unsigned char failures = 0;
unsigned char dummy = 0;
unsigned char result = 0;

int int0 = 0;
int int1 = 0;
signed char char0 = 0;
signed char char1 = 0;

/* *** NOTE ***  This particular test takes quite a while to run
 * ~ 10,000,000 instruction cycles. (2.5 seconds on a 20Mhz PIC).
 * The WDT will reset the CPU if it's enabled. So disable it...
*/

void
done ()
{
  dummy++;
}

void c_char_lte_lit1(unsigned char expected_result)
{
  result = 0;

  if(char0 <= -0x7f)
    result |= 1;

  if(char0 <= -1)
    result |= 2;

  if(char0 <= 0)
    result |= 4;

  if(char0 <= 1)
    result |= 8;

  if(char0 <= 0x7f)
    result |= 0x10;
  
  if(result != expected_result)
    failures++;
}

void char_compare(void)
{
  char0 = 0x7f;
  c_char_lte_lit1(0x10);

  char0 = 0x7e;
  c_char_lte_lit1(0x10);

  char0 = 0x40;
  c_char_lte_lit1(0x10);

  char0 = 0x2;
  c_char_lte_lit1(0x10);

  char0 = 0x1;
  c_char_lte_lit1(0x18);

  char0 = 0;
  c_char_lte_lit1(0x1c);

  char0 = -1;
  c_char_lte_lit1(0x1e);

  char0 = -2;
  c_char_lte_lit1(0x1e);

  char0 = -0x40;
  c_char_lte_lit1(0x1e);

  char0 = -0x7e;
  c_char_lte_lit1(0x1e);

  char0 = -0x7f;
  c_char_lte_lit1(0x1f);

  char0 = 0x80;
  /*  c_char_lte_lit1(0x1f); */

  /* Now test entire range */

  for(char0=2; char0 != 0x7f; char0++)
    c_char_lte_lit1(0x10);

  for(char0=-0x7e; char0 != 0; char0++)
    c_char_lte_lit1(0x1e);
}

void c_int_lte_lit1(unsigned char expected_result)
{
  result = 0;

  if(int0 <= 0)
    result |= 1;

  if(int0 <= 1)
    result |= 2;

  if(int0 <= 0xff)
    result |= 4;

  if(int0 <= 0x100)
    result |= 8;

  if(int0 <= 0x0101)
    result |= 0x10;
  
  if(int0 <= 0x01ff)
    result |= 0x20;
  
  if(int0 <= 0x0200)
    result |= 0x40;

  if(int0 <= 0x0201)
    result |= 0x80;

  if(result != expected_result)
    failures=1;
}

void int_compare1(void)
{
  int0 = -1;
  c_int_lte_lit1(0xff);

  int0 = 0;
  c_int_lte_lit1(0xff);

  int0 = 1;
  c_int_lte_lit1(0xfe);

  int0 = 2;
  c_int_lte_lit1(0xfc);

  int0 = 0xfe;
  c_int_lte_lit1(0xfc);

  int0 = 0xff;
  c_int_lte_lit1(0xfc);

  int0 = 0x100;
  c_int_lte_lit1(0xf8);

  int0 = 0x101;
  c_int_lte_lit1(0xf0);

  int0 = 0x1fe;
  c_int_lte_lit1(0xe0);

  int0 = 0x1ff;
  c_int_lte_lit1(0xe0);

  int0 = 0x200;
  c_int_lte_lit1(0xc0);

  int0 = 0x201;
  c_int_lte_lit1(0x80);

  int0 = 0x7f00;
  c_int_lte_lit1(0x0);

  /* now check contiguous ranges */

  for(int0 = -0x7fff; int0 != 1; int0++)
    c_int_lte_lit1(0xff);

  for(int0 = 2; int0 != 0xff; int0++)
    c_int_lte_lit1(0xfc);

  for(int0 = 0x202; int0 != 0x7fff; int0++)
    c_int_lte_lit1(0);
}

void c_int_lte_lit2(unsigned char expected_result)
{
  result = 0;

  if(int0 <= -0x7fff)
    result |= 1;

  if(int0 <= -0x7f00)
    result |= 2;

  if(int0 <= -0x7eff)
    result |= 4;

  if(int0 <= -0x7e00)
    result |= 8;

  if(int0 <= -0x0101)
    result |= 0x10;
  
  if(int0 <= -0x0100)
    result |= 0x20;
  
  if(int0 <= -0xff)
    result |= 0x40;

  if(int0 <= -1)
    result |= 0x80;

  if(result != expected_result)
    failures=1;
}

void int_compare2(void)
{
  int0 = -0x7fff;
  c_int_lte_lit2(0xff);

  int0 = -0x7f00;
  c_int_lte_lit2(0xfe);

  int0 = -0x7eff;
  c_int_lte_lit2(0xfc);

  int0 = -0x7e00;
  c_int_lte_lit2(0xf8);

  int0 = -0x4567;
  c_int_lte_lit2(0xf0);

  int0 = -0x200;
  c_int_lte_lit2(0xf0);

  int0 = -0x102;
  c_int_lte_lit2(0xf0);

  int0 = -0x101;
  c_int_lte_lit2(0xf0);

  int0 = -0x100;
  c_int_lte_lit2(0xe0);

  int0 = -0xff;
  c_int_lte_lit2(0xc0);

  int0 = -0x02;
  c_int_lte_lit2(0x80);

  int0 = -0x01;
  c_int_lte_lit2(0x80);

  int0 = 0;
  c_int_lte_lit2(0x00);

  int0 = 1;
  c_int_lte_lit2(0x00);

  int0 = 0x7fff;
  c_int_lte_lit2(0x00);

  /* now check contiguous ranges */

  for(int0 = -0x7ffe; int0 != -0x7f00; int0++)
    c_int_lte_lit2(0xfe);

  for(int0 = -0x7dff; int0 != -0x101; int0++)
    c_int_lte_lit2(0xf0);

  for(int0 = 0; int0 != 0x7fff; int0++)
    c_int_lte_lit2(0);
}

int
main (void)
{
  char_compare();
  int_compare1();
  int_compare2();

  success = failures;
  done ();
  printf("failures: %d\n",failures);

  return failures;
}
