/* bug #1265 - misadjusted stack from unprototyped function call */

#include <stdio.h>
#include <string.h>

int failures = 0;

char str1[10];
char str2[10];

int f2 (int x) { return x == 2345 ? 23 : -1; }

int main (void) {
  int x, n;

  sprintf (str1, "%p\n", &x);
  x = 1234;
  n = f1 (x);
  sprintf (str2, "%p\n", &x);
  
  if (strcmp(str1, str2)) {
      puts("not equal");
      failures++;
  }
  if (n != 42) {
      puts("f1 returns wrong value");
      failures++;
  }

  sprintf (str1, "%p\n", &x);
  x = 2345;
  n = f2 (x);
  sprintf (str2, "%p\n", &x);
  
  if (strcmp(str1, str2)) {
      puts("not equal");
      failures++;
  }
  if (n != 23) {
      puts("f2 returns wrong value");
      failures++;
  }

  return failures;
}

int f1 (int x) { return x == 1234 ? 42 : -1; }
