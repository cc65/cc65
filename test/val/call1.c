/*
  !!DESCRIPTION!!
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
unsigned int uint0 = 0;
unsigned int uint1 = 0;
unsigned char uchar0 = 0;
unsigned char uchar1 = 0;

unsigned char call3 (void);

void
done ()
{
  dummy++;
}

void
call1 (unsigned char uc0)
{
  if (uc0)
    failures++;
}

void
call2 (unsigned int ui0)
{
  if (ui0)
    failures++;
}

unsigned char
call3 (void)
{
  if (uchar0)
    failures++;

  return (failures);
}

unsigned int
call4 (void)
{
  unsigned int i = 0;

  if (uint0)
    i++;

  return (i);
}

unsigned int
call5 (unsigned int k)
{
  if (k)
    failures++;

  return (k);
}

unsigned char 
call6a(unsigned char uc)
{
  if(uc>uchar1)
    return 1;
  else
    return 0;
}

unsigned char
call6(unsigned char uc)
{
  return(call6a(uc));
}

unsigned int 
call7a(unsigned int ui)
{
  if(ui)
    return 1;
  else
    return 0;
}

unsigned int
call7(unsigned int ui)
{
  return(call7a(ui));
}

unsigned char
call8(unsigned char uc1,unsigned char uc2)
{
  return uc1+uc2;
}

void call9(unsigned int ui1, unsigned int ui2)
{
  if(ui1 != 0x1234)
    failures++;
  if(ui2 != 0x5678)
    failures++;
}

int
main (void)
{
  call1 (uchar0);
  call2 (uint0);
  uchar1 = call3 ();
  uint1 = call4 ();
  if (uint1)
    failures++;

  uint1 = call5 (uint0);
  if (uint1)
    failures++;

  if(call6(uchar0))
    failures++;

  if(call7(0))
    failures++;

  if(!call7(1))
    failures++;

  if(!call7(0xff00))
    failures++;

  uchar0=4;
  uchar1=3;
  uchar0 = call8(uchar0,uchar1);

  if(uchar0 != 7)
    failures++;

  call9(0x1234,0x5678);

  success = failures;
  done ();
  printf("failures: %d\n",failures);

  return failures;
}
