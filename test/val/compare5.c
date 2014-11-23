/*
  !!DESCRIPTION!! regression testing program for comparing longs
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

/*
 compare5.c
*/

#define  COMPARE_OUT_OF_RANGE 1

unsigned char success = 0;
unsigned char failures = 0;
unsigned char dummy = 0;

int int0 = 0;
int int1 = 0;
char char0 = 0;
char char1 = 0;
long long0 = 0;
long long1 = 0;
unsigned long ulong0 = 0;
unsigned long ulong1 = 0;

void
done ()
{
  dummy++;
}

/* compare to 0
 assumes
 long0 == 0
 ulong0 == 0
*/
void c_0(void)
{
  if(long0 != 0)
    failures++;

  if(long0 > 0)
    failures++;

  if(ulong0 != 0)
    failures++;

  if(ulong0 > 0)
    failures++;
}

/* compare to 1
 assumes
 long1 == 1
 ulong1 == 1
*/
void c_1(void)
{
  if(long0 == 1)
    failures++;

  if(long1 != 1)
    failures++;

  if(ulong0 == 1)
    failures++;

  if(ulong1 != 1)
    failures++;

  if(long1 < 0)
    failures++;

  if(long1 < 1)
    failures++;

  if(ulong1 < 1)
    failures++;

  if(long1 > 1)
    failures++;

  if(ulong1 > 1)
    failures++;
}

/* compare to 2
 assumes
 long0 == 2
 ulong0  == 2
*/
void c_2(void)
{
  if(long0 != 2)
    failures++;

  if(ulong0 != 2)
    failures++;

  if(long1 == 2)
    failures++;

  if(ulong1 == 2)
    failures++;
}

/* compare to 0xff
 assumes
 achar0 == 0xff
 aint0  == 0xff
*/
void c_ff(void)
{
  if(long0 != 0xff)
    failures++;

  if(ulong0 != 0xff)
    failures++;

  if(long1 == 0xff)
    failures++;

  if(ulong1 == 0xff)
    failures++;
}

/* compare to 0x200
 assumes
 achar0 == 0x200
 aint0  == 0x200
*/
void c_200(void)
{
  if(long0 != 0x200)
    failures++;

  if(ulong0 != 0x200)
    failures++;

  if(long1 == 0x200)
    failures++;

  if(ulong1 == 0x200)
    failures++;
}

/* compare to 0x20000
 assumes
 long0  == 0x20000
 ulong0 == 0x20000
 long1  != 0x20000
 ulong1 != 0x20000
*/
void c_20000(void)
{
  if(long0 != 0x20000)
    failures++;

  if(ulong0 != 0x20000)
    failures++;

  if(long1 == 0x20000)
    failures++;

  if(ulong1 == 0x20000)
    failures++;

  if(long0 <= 0x10000)
    failures++;

  if(long0 < 0x10000)
    failures++;

/*  if(long0 < 0x12345)
    failures++;
*/
  if(long0 == 0)
    failures++;
}

/* compare to 0x00a5
 assumes
 char0  == 0xa5
 int0  == 0x00a5
*/
void c_a5(void)
{
  if(char0 != 0xa5)
    failures++;

  if(int0 != 0xa5)
    failures++;

  if(int0 == 0xa4)
    failures++;

  if(int0 == 0xa500)
    failures++;
}

/* compare to 0xa500
 assumes
 char0  == 0xa5
 int0  == 0xa500
*/
void c_a500(void)
{
#ifdef COMPARE_OUT_OF_RANGE
  if(char0 == 0xa500)
    failures++;
#endif

  if(int0 != 0xa500)
    failures++;

  if(int0 == 0xa400)
    failures++;

  if(int0 == 0x00a5)
    failures++;
}

/* compare to 0xabcd
 assumes
 char0  == 0xa5
 int0  == 0xabcd
*/
void c_abcd(void)
{
#ifdef COMPARE_OUT_OF_RANGE
  if(char0 == 0xabcd)
    failures++;
#endif

  if(int0 != 0xabcd)
    failures++;

  if(int0 == 0xab00)
    failures++;

  if(int0 == 0x00cd)
    failures++;

  if(int0 == 0x05cd)
    failures++;

  if(int0 == 0xab05)
    failures++;

  if(int0 == 0xab01)
    failures++;

  if(int0 == 0x01cd)
    failures++;

  if(int0 > 0)
    failures++;

#ifdef COMPARE_OUT_OF_RANGE
  if(int0 == 0x1234abcd)
    failures++;
#endif
}

/* assumes char1 == 0 */
void c_ifelse1(void)
{
  if(char0)
    char0 = char1;
  else
    char0 = 0;

  if(char0)
    failures++;
}

/*
   assumes long0 = -1
   assumes long1 = 1
*/
void c_minus1(void)
{
  printf("long0:%ld long1:%ld\n",long0,long1);
  
  printf("(long0 != -1)\n");
  if(long0 != -1)
  {
	failures++;
  }
  printf("(long0 > 0)\n");
  if(long0 > 0)
  {
	failures++;
  }
  printf("(long1 < 0)\n");
  if(long1 < 0)
  {
	failures++;
  }
/*
  if(long1 < 2)
    failures++;
*/
}

/* assumes
 long0 = long1 = ulong0 = ulong1 == 0
*/
void c_long2long_eq(void)
{
  if(long0 != long1)
    failures++;

  if(ulong0 != ulong1)
    failures++;

  if(long0 != ulong1)
    failures++;

  if(long0 > long1)
    failures++;

  if(long0 < long1)
    failures++;

  if(long0 > ulong0)
    failures++;

  if(long0 < ulong0)
    failures++;
}

/* assumes
 long0 = ulong0 == 0
 long1 = ulong1 == 1
*/
void c_long2long_neq(void)
{
  if(long0 == long1)
    failures++;

  if(ulong0 == ulong1)
    failures++;

  if(long1 != ulong1)
    failures++;

  if(long1 < long0)
    failures++;

  if(long1 <= long0)
    failures++;

  if(ulong1 < ulong0)
    failures++;

  if(ulong1 <= ulong0)
    failures++;
}

/*  long0=-100;
  long1=-1000;
*/
void
c_long2neglit(void)
{
  if(long0>0)
    failures++;
  if(long1>0)
    failures++;

  if(long1 > long0)
    failures++;

  if(long1 > 100)
    failures++;

  if(long0 > -50)
    failures++;

  if(long1 < -5000)
    failures++;
}

int
main (void)
{
  c_0();
  printf("c_0: %d\n",failures);

  c_long2long_eq();
  printf("c_long2long_eq: %d\n",failures);

  long1 = 1;
  ulong1 = 1;
  c_1();
  printf("c_1: %d\n",failures);
  c_long2long_neq();
  printf("c_long2long_neq: %d\n",failures);

  long0 = 2;
  ulong0 = 2;
  c_2();
  printf("c_2: %d\n",failures);

  long0 = 0xff;
  ulong0 = 0xff;
  c_ff();
  printf("c_ff: %d\n",failures);

  long0 = 0x200;
  ulong0 = 0x200;
  c_200();
  printf("c_200: %d\n",failures);

  long0 = 0x20000;
  ulong0 = 0x20000;
  c_20000();
  printf("c_20000: %d\n",failures);

  long0 = -1;
  c_minus1();
  printf("c_minus1: %d\n",failures);

  long0=-100;
  long1=-1000;
  c_long2neglit();
  printf("c_long2neglit: %d\n",failures);

  success = failures;
  done ();
  
  printf("failures: %d\n",failures);

  return failures;
}
