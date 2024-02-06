/*
  !!DESCRIPTION!! Addition tests - mostly int's
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

unsigned char failures=0;

char char0 = 0;
char char1 = 0;
char char2 = 0;

/*
  this test assumes:
  sizeof(char) == 1
  sizeof(int)  == 2
  sizeof(long) == 4
*/

#ifdef REFERENCE

/*
   make sure the reference output uses types with
   proper size
*/

#include <stdint.h>

#ifdef SIZEOF_INT_16BIT
int16_t int0 = 0;
int16_t int1 = 0;
#else
int32_t int0 = 0;
int32_t int1 = 0;
#endif
int32_t long0 = 0;
int32_t long1 = 0;
uint32_t ulong0 = 0;
uint32_t ulong1 = 0;

#else

int int0 = 0;
int int1 = 0;
long long0 = 0;
long long1 = 0;
unsigned long ulong0 = 0;
unsigned long ulong1 = 0;

#endif

#ifdef SUPPORT_BIT_TYPES

bit bit0 = 0;
bit bit1 = 0;
bit bit2 = 0;
bit bit3 = 0;
bit bit4 = 0;
bit bit5 = 0;
bit bit6 = 0;
bit bit7 = 0;
bit bit8 = 0;
bit bit9 = 0;
bit bit10 = 0;
bit bit11 = 0;

#endif

void done(char *name)
{
  printf("%s done - failures: %d\n", name, failures);

  if(failures)
    exit(failures);
}

void add_char2char(void)
{
  if(char0 != 4)
    failures++;
  if(char1 != 5)
    failures++;

  char0 = char0 + char1;

  if(char0 != 9)
    failures++;

  char0 += 127;

#if (!defined(REFCC) && defined(UNSIGNED_CHARS)) || (defined(REFCC) && defined(REFCC_UNSIGNED_CHARS))
  if(char0 < 0)
    failures++;

  if(char0 != (127+9))
    failures++;
#else
  if(char0 > 0)
    failures++;

  if(char0 != -0x78)
    failures++;
#endif

  done("add_char2char");
}

void add_compound_char(void)
{
  char0 = char1+5;

  if(char0 != 9)
    failures++;

  if((char0+char1) != 13)
    failures++;

  done("add_compound_char");
}

void add_int2int(void)
{
  if(int0 != 4)
    failures++;
  if(int1 != 5)
    failures++;

  int0 += int1;
  if(int0 != 9)
    failures++;

  int0 += 0x7fff;
  if(int0 != -0x7ff8)
    failures++;

  done("add_int2int");
}

void add_compound_int(void)
{
  int0 = int1+5;

  if(int0 != 9)
    failures++;

  if((int0+int1) != 13)
    failures++;

  done("add_compound_int");
}

void add_lit2long(void)
{
  if(long0 != 0)
    failures++;

  long0++;

  if(long0 != 1)
    failures++;

  long0 = long0 + 0xff;

  if(long0 != 0x100)
    failures++;

  long0 = long0 + 0x100;
  if(long0 != 0x200)
    failures++;

  long0 = long0 + 0xfe00;
  if(long0 != 0x10000)
    failures++;

  long0 = long0 + 0xff0000;
  if(long0 != 0x1000000)
    failures++;

  long0 = long0 + 0x7e000000;
  if(long0 != 0x7f000000)
    failures++;

  /* wrap around zero */
  long0 = long0 + 0x2000000;
  if(long0 != -0x7f000000)
    failures++;

  long0 = long0 + 0x7f000000;
  if(long0 != 0)
    failures++;

  done("add_lit2long");
}

void add_lit2ulong(void)
{
  if(ulong0 != 0)
    failures++;

  ulong0++;

  if(ulong0 != 1)
    failures++;

  ulong0 = ulong0 + 0xff;

  if(ulong0 != 0x100)
    failures++;

  ulong0 = ulong0 + 0x100;
  if(ulong0 != 0x200)
    failures++;

  ulong0 = ulong0 + 0xfe00;
  if(ulong0 != 0x10000)
    failures++;

  ulong0 = ulong0 + 0xff0000;
  if(ulong0 != 0x1000000)
    failures++;

  ulong0 = ulong0 + 0x7e000000;
  if(ulong0 != 0x7f000000)
    failures++;

  ulong0 = ulong0 + 0x2000000;
  if(ulong0 != 0x81000000)
    failures++;

  /* wrap around zero */
  ulong0 = ulong0 + 0x7f000000;
  if(ulong0)
    failures++;

  done("add_lit2ulong");
}

int main(void)
{
  char0=4;
  char1 = char0 + 1;
  add_char2char();

  char1=4;
  add_compound_char();

  int0 = 4;
  int1 = int0 + 1;
  add_int2int();

  int1=4;
  add_compound_int();

  add_lit2long();
  add_lit2ulong();

  /* if not exit() */
  return 0;
}
