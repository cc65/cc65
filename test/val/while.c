/*
  !!DESCRIPTION!! while-condition tests
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>

static unsigned char failures = 0x00;
static unsigned char achar0 = 0;

static void
while1 (void)
{
  unsigned char i = 10;

  do {
    ++achar0;
  } while (--i);

  if (achar0 != 10) {
    failures |= 0x01;
  }
}

static void
while2 (void)
{
  unsigned char i = 10;

  achar0 = 0;
  while (--i) {
    ++achar0;
  }

  if (achar0 != 10 - 1) {
    failures |= 0x02;
  }
}

static void
while3 (void)
{
  achar0 = 0;
  do {
    if (++achar0 == (unsigned char)0) {
      return;
    }
  } while (1);

  failures |= 0x04;
}

static void
while4 (void)
{
  achar0 = 0;
  while (1) {
    if (++achar0 == (unsigned char)0) {
      return;
    }
  }

  failures |= 0x08;
}

static void
while5 (void)
{
  achar0 = 0;
  do {
    ++achar0;
  } while (0);

  if (achar0 != 1) {
    failures |= 0x10;
  }
}

static void
while6 (void)
{
  achar0 = 0;
  while (0) {
    ++achar0;
  }

  if (achar0 != 1 - 1) {
    failures |= 0x20;
  }
}

int
main (void)
{
  while1 ();
  while2 ();
  while3 ();
  while4 ();
  while5 ();
  while6 ();

  if (failures) {
    printf("failures: 0x%02X\n", failures);
  }
  return failures;
}
