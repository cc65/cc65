/*
  !!DESCRIPTION!! 
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!
*/

/*
The following code produces an 'Error: Incompatible pointer types' at
the last line when compiling with snapshot-2.11.9.20080316 without
optimizations. If I remove the struct inside f() it compiles fine ?!?

Best, Oliver
*/

#include <stdio.h>

void f(void){struct{int i;}d;}
struct{void(*p)(void);}s={f};

int main(void)
{
    printf("it works :)\n");

    return 0;
}
