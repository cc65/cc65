
/*
  !!DESCRIPTION!! Addition tests - mostly int's
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <stdlib.h>

static unsigned int failures = 0;

/*
  this test assumes:
  sizeof(long) == 4

  CAUTION: the wraparound behaviour is actually undefined, to get the "expected"
           behaviour with GCC, use -fwrapv or -fno-strict-overflow

  see: https://gcc.gnu.org/wiki/FAQ#signed_overflow
*/

#ifdef REFERENCE

/*
   make sure the reference output uses types with
   proper size
*/

#include <stdint.h>

int32_t long0 = 0;

#else

long long0 = 0;

#endif

void print(void)
{
#if defined(REFERENCE) && defined(REFCC_SIZEOF_LONG_64BIT)
    printf("long0: %d\n", long0);
#else
    printf("long0: %ld\n", long0);
#endif
}

int main(void)
{
    long0 = 0x7f000000L;
    /* wrap around zero */
    print();
    long0 = long0 + 0x2000000L;
    if(long0 != -0x7f000000L) {
        printf("failed!\n");
        failures++;
    }
    print();

    long0 = 0x7f000000L;
    /* wrap around zero */
    print();
    long0 = long0 + 0x2000000L;
    print();
    if(long0 != -0x7f000000L) {
        printf("failed!\n");
        failures++;
    }
    print();

    return failures;
}
