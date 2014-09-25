/*
  !!DESCRIPTION!! Substraction Test
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

int int0 = 0;
int int1 = 0;

signed char char0 = 0;
signed char char1 = 0;
signed char char2 = 0;

void done()
{
  dummy++;
}

void sub_int1(void)
{
  if(int0 != 5)
    failures++;

  if(int1 != 4)
    failures++;

  int0 = int0 - int1;

  if(int0 != 1)
    failures++;

  int0 = 4 - int0;
  if(int0 != 3)
    failures++;

  int0 = int0 - int1;

  if(int0 != -1)
    failures++;

  int0 = int0 - 0xff;

  if(int0 != -0x100)
    failures++;

  int0 = 0xff - int0;

  if(int0 != 0x1ff)
    failures++;
}

void sub_char_int(void)
{
  int0 = int0 - char0;

  if(int0 != 3)
    failures++;

  if(int0 < char0)
    failures++;

  int0 = int0 - char0;

  if(int0 != 1)
    failures++;

  if(int0 > char0)
    failures++;

  int0 = int0 - char0;
  if(int0 != -1)
    failures++;

  if(int0>0)
    failures++;
}

void assign_char2int(void)
{
  int0 = char0;
  if(int0 != 0x7f)
    failures++;

 /* printf("%2x %2x %2x %d\n",0x7f,int0,char0,failures); */

  int1 = char1;
  if(int1 != -5)
    failures++;

 /* printf("%2x,%d %2x,%d %2x,%d %d\n",-5,-5,(int)int1,(int)int1,(int)char1,(int)char1,failures); */
}

void sub_compound_char(void)
{
  char0 = char1 - 5;
  if(char0 != 4)
    failures++;

  if((char1 - char0 - 5) != 0)
    failures++;
}

void sub_compound_int(void)
{
  int0 = int1 - 5;
  if(int0 != 4)
    failures++;

  if((int1 - int0 - 5) != 0)
    failures++;
}

int main(void)
{
  int0 = 5;
  int1 = 4;

  sub_int1();
  printf("failures: %d\n",failures);

  int0 = 5;
  int1 = 4;
  char0 = 2;

  sub_char_int();
  printf("failures: %d\n",failures);

  char0 = 0x7f;
  char1 = -5;
  assign_char2int();
  printf("failures: %d\n",failures);

  char1 = 9;
  sub_compound_char();
  printf("failures: %d\n",failures);

  int1 = 9;
  sub_compound_int();

  success = failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
