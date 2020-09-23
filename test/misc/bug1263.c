
/* bug #1263 - erroneous error for implicit function declaration */

#include <stdlib.h>

enum E { I };
extern int f(enum E);
int f(e)
  enum E e;
{
  return 1;
}

int main(void)
{
    return f(1) ? EXIT_SUCCESS : EXIT_FAILURE;
}
