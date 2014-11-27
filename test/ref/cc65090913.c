/*
  !!DESCRIPTION!! optimizer bug
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Oliver Schmidt
*/

/*
> I found the problem and fixed it. cc65 treated a label as a statement, but
> the standard says, that a label is part of a statement. In a loop without
> curly braces like
>
>         while (foo < bar)
>             label:  ++foo;
>
> the following statement is the one that is looped over - and because cc65
> treated just the label as a statement, it created code that looped forever.

*/

#include <stdio.h>

int foo=0,bar=2;

int main(void)
{
    while(foo<bar)
	    label: ++foo;

    printf("foo: %d bar: %d\n",foo,bar);

    return 0;
}
