/*
  !!DESCRIPTION!! Pointer tests
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
unsigned int aint0 = 0;
unsigned int aint1 = 0;
unsigned char achar0 = 0;
unsigned char achar1 = 0;
unsigned char *acharP = 0;

char buff[10];

void
done ()
{
  dummy++;
}

void
f1 (unsigned char *ucP)
{
  if (ucP == 0)
    {
      failures++;
      return;
    }

  if (*ucP)
    failures++;
}

void
f2 (unsigned int *uiP)
{
  if (uiP == 0)
    {
      failures++;
      return;
    }

  if (*uiP)
    failures++;
}

unsigned char *
f3 (void)
{
  return &achar0;
}

void f4(unsigned char *ucP, unsigned char uc)
{
  if(!ucP) {
    failures++;
    return;
  }

  if(*ucP != uc)
    failures++;
}

void init_array(char start_value)
{
  unsigned char c;

  for(c=0; c<sizeof(buff); c++)
    buff[c] = start_value++;
}

void check_array(char base_value)
{
  unsigned char c;

  for(c=0; c<sizeof(buff); c++)
    if(buff[c] != (base_value+c))
      failures++;
}

void index_by_pointer(unsigned char *index, unsigned char expected_value)
{
  if(buff[*index] != expected_value)
    failures++;
}

int
main (void)
{
  init_array(4);
  check_array(4);

  if(buff[achar0 + 7] != 4+7)
    failures++;

  dummy = buff[achar0];

  if(dummy != 4)
    failures++;

  if(dummy != (buff[achar0+1] -1))
    failures++;

  index_by_pointer(&dummy, 8);

  f1 (&achar0);
  f2 (&aint0);

  acharP = f3 ();
  if ((acharP == 0) || (*acharP))
    failures++;
  achar0 = 42;
  if(*acharP != 42)
    failures++;

  achar0 = 5;
  f4(&achar0, achar0);

  success = failures;
  done ();
  printf("failures: %d\n",failures);

  return failures;
}
