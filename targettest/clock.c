/*
** simple clock test
**
** 06-Nov-2001, Christian Groessler
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

  printf("hit <return> to exit...");
  cgetc();
  printf("\n");
  return(0);
}

