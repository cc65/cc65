/*
 * simple clock test
 *
 * 06-Nov-2001, Christian Groessler
 */

#include <stdio.h>
#include <conio.h>
#include <time.h>

int main(void)
{
  printf("\ncc65 time routines test program\n");
  printf("-------------------------------\n");

  printf("clocks per second: %d\n", CLOCKS_PER_SEC);
  printf("current clock: %ld\n", clock());

#ifdef __ATARI__
  /* Atari DOS 2.x clears the screen after program termination, so wait... */
  printf("hit <return> to exit...");
  cgetc();
#endif
  printf("\n");
  return(0);
}

