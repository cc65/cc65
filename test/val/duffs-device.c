/*
  !!DESCRIPTION!! Implementation of Duff's device (loop unrolling).
  !!ORIGIN!!      
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

#define ASIZE (100)

unsigned char success=0;
unsigned char failures=0;
unsigned char dummy=0;

#ifdef SUPPORT_BIT_TYPES
bit bit0 = 0;
#endif

void done()
{
  dummy++;
}

int acmp(char* a, char* b, int count)
{
  int i;

  for(i = 0; i < count; i++) {
    if(a[i] != b[i]) {
      return 1;
    }
  }
  return 0;
}

void duffit (char* to, char* from, int count) 
{
  int n = (count + 7) / 8;

  switch(count % 8) {
    case 0: do {    *to++ = *from++;
    case 7:         *to++ = *from++;
    case 6:         *to++ = *from++;
    case 5:         *to++ = *from++;
    case 4:         *to++ = *from++;
    case 3:         *to++ = *from++;
    case 2:         *to++ = *from++;
    case 1:         *to++ = *from++;
    } while(--n > 0);
  }
}

int main(void)
{
  char a[ASIZE] = {1};
  char b[ASIZE] = {2};
  
  /* a and b should be different */
  if(!acmp(a, b, ASIZE)) {
    failures++;
  }
  
  duffit(a, b, ASIZE);
  
  /* a and b should be the same */
  if(acmp(a, b, ASIZE)) {
    failures++;
  }

  success=failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
