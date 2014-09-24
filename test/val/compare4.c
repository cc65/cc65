/*
  !!DESCRIPTION!! regression testing program for comparing signed chars and ints
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

/*
 compare4.c
*/

/*#define  COMPARE_OUT_OF_RANGE 1*/

unsigned char success = 0;
unsigned char failures = 0;
unsigned char dummy = 0;

#ifdef SUPPORT_BIT_TYPES
bit bit0 = 0;
#endif
#ifdef SIZEOF_INT_16BIT
#if defined(__LINUX__) || defined(LINUX)
short int0 = 0;
short int1 = 0;

#else
int int0 = 0;
int int1 = 0;

#endif

#else
int int0 = 0;
int int1 = 0;

#endif

signed char char0 = 0;
signed char char1 = 0;

void
done ()
{
  dummy++;
}

/* compare to 0
 assumes
 char0 == 0
 char1 != 0
 int0  == 0
 int1  != 0
*/
void c_0(void)
{
  if(char0 != 0)
    failures++;

  if(char0)
    failures++;

  if(char1 == 0)
    failures++;

  if(!char1)
    failures++;

  if(int0 != 0)
    failures++;

  if(int0)
    failures++;

  if(int1 == 0)
    failures++;

  if(!int1)
    failures++;

  if(char0>0)
    failures++;

  if(int0>0)
    failures++;

  if(char0<0)
    failures++;

  if(int0<0)
    failures++;
}

/* compare to 1
 assumes
 char0 != 1
 char1 == 1
 int0  != 1
 int1  == 1
*/
void c_1(void)
{
  if(char0 == 1)
    failures++;

  if(char1 != 1)
    failures++;

  if(int0 == 1)
    failures++;

  if(int1 != 1)
    failures++;

  if(char0 < 0)
    failures++;

  if(int0 < 0)
    failures++;
}

/* compare to 2
 assumes
 achar0 == 2
 aint0  == 2
*/
void c_2(void)
{
  if(char0 != 2)
    failures++;

  if(int0 != 2)
    failures++;
}

/* compare to 0xff
 assumes
 achar0 == 0xff
 aint0  == 0xff
*/
void c_ff(void)
{
  if((unsigned char)char0 != 0xff)
    failures++;

  if((unsigned short)int0 != 0xff)
    failures++;

  if((unsigned short)int0 == 0xfe)
    failures++;

  if((unsigned short)int0 == 0xff00)
    failures++;
}

/* compare to 0x00a5
 assumes
 char0  == 0xa5
 int0  == 0x00a5
*/
void c_a5(void)
{
  if((unsigned char)char0 != 0xa5)
    failures++;

  if((unsigned short)int0 != 0xa5)
    failures++;

  if((unsigned short)int0 == 0xa4)
    failures++;

  if((unsigned short)int0 == 0xa500)
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

  if((unsigned short)int0 != 0xa500)
    failures++;

  if(int0 != 0x44)
    int0 = 0x28;

  if((unsigned short)int0 == 0xa400)
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
/*
  if(int0 != 0xabcd)
    failures++;
*/
  if((unsigned short)int0 == 0xab00)
    failures++;

  if(int0 == 0x00cd)
    failures++;

  if(int0 == 0x05cd)
    failures++;

  if((unsigned short)int0 == 0xab05)
    failures++;

  if((unsigned short)int0 == 0xab01)
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

/* assumes char0 = -1
 assumes int0 = -1
*/
void c_minus1(void)
{
  if(char0 != -1)
    failures++;

  printf("%d\n",failures);

  if(int0 != -1)
    failures++;

  printf("%d\n",failures);

  if(char0 != int0)
    failures++;

  printf("%d\n",failures);

  if(char0>0)
    failures++;

  printf("%d\n",failures);

  if(int0>0)
    failures++;

  printf("%d\n",failures);
}

void c_c0gtc1(void)
{
  if(char0 < char1)
    failures++;

  printf("%d\n",failures);
}

int main(void)
{
  int1 = 1;
  char1 = 1;
  c_0();
  printf("failures: %d\n",failures);
  c_1();
  printf("failures: %d\n",failures);

  int0 = 2;
  char0 = 2;
  c_2();
  printf("failures: %d\n",failures);

  int0 = 0xff;
  char0 = 0xff;
  c_ff();
  printf("failures: %d\n",failures);

  int0 = 0xa5;
  char0 = 0xa5;
  c_a5();
  printf("failures: %d\n",failures);

  int0 = 0xabcd;
  /*c_abcd();*/

  char0 = 0;
  char1 = 0;
  c_ifelse1();
  printf("failures: %d\n",failures);

  char0 = 1;
  c_ifelse1();
  printf("failures: %d\n",failures);

  char0 = -1;
  int0 = -1;
  c_minus1();
  printf("failures: %d\n",failures);

  char0 = 5;
  char1 = 3;
  c_c0gtc1();
  printf("failures: %d\n",failures);

  char1 = -3;
  c_c0gtc1();

  success = failures;
  done ();
  printf("failures: %d\n",failures);

  return failures;
}
