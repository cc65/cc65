/*
  !!DESCRIPTION!! Compound comparisons
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

unsigned char success = 0;
unsigned char failures = 0;
unsigned char dummy = 0;

#ifdef SUPPORT_BIT_TYPES
bit bit0 = 0;
#endif
int int0 = 0;
int int1 = 0;
unsigned char uchar0 = 0;
unsigned char uchar1 = 0;
char char0 = 0;
char char1 = 0;
char long0 = 0;
char long1 = 0;

void
done ()
{
  dummy++;
}

void c_char(void)
{
  if(char0 || char1)
    failures++;

  if(char0 && char1)
    failures++;

  if(char0 > char1)
    failures++;

  if((char0+1)  < char1)
    failures++;

  if((char0+5) >= (char1+9))
    failures++;

  char0++;

  if(char0 && char1)
    failures++;

  if(char0 != (char1+1) )
    failures++;

  if(!char0)
    failures++;

  if(char1 || !char0)
    failures++;

  if((char0 >5 ) && (char0 < 10)) 
    failures++;

  char0 +=5; /* char0 = 6 now */

  if(!((char0 >5 ) && (char0 < 10)))
    failures++;
}

void c_int(void)
{
  if(int0 || int1)
    failures++;

  if(int0 && int1)
    failures++;

  if(int0 > int1)
    failures++;

  if((int0+1)  < int1)
    failures++;

  if((int0+5) >= (int1+9))
    failures++;

  int0++;

  if(int0 && int1)
    failures++;

  if(int0 != (int1+1) )
    failures++;

  if(!int0)
    failures++;

  if(int1 || !int0)
    failures++;

  if((int0 >5 ) && (int0 < 10)) 
    failures++;

  int0 +=5; /* int0 = 6 now */

  if(!((int0 >5 ) && (int0 < 10)))
    failures++;
}

void c_long(void)
{
  if(long0 || long1)
    failures++;

  if(long0 && long1)
    failures++;

  if(long0 > long1)
    failures++;

  if((long0+1)  < long1)
    failures++;

  if((long0+5) >= (long1+9))
    failures++;

  long0++;

  if(long0 && long1)
    failures++;

  if(long0 != (long1+1) )
    failures++;

  if(!long0)
    failures++;

  if(long1 || !long0)
    failures++;

  if((long0 >5 ) && (long0 < 10)) 
    failures++;

  long0 +=5; /* long0 = 6 now */

  if(!((long0 >5 ) && (long0 < 10)))
    failures++;
}

void c_uminus(void)
{
  int1 = -int0;
  if(int1 < 0)
    failures++;
}

int
main (void)
{
  c_char();
  c_int();
  c_long();

  int0 = -1;
  c_uminus();
  if(int1 != 1)
    failures++;

  success = failures;
  done ();
  printf("failures: %d\n",failures);

  return failures;
}
