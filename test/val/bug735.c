#include <stdio.h>

unsigned char failures = 0;

int main(void)
{
  int i;

  i = 0;
  if ((i > 1) && (i < 3)) {
    failures++;
  }

  printf("failures: %u\n", failures);
  return failures;
}

