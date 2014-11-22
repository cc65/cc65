/*
  !!DESCRIPTION!! 
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!
*/

#include <stdio.h>

/*
there is a bug in the preprocessor (i think) ... the following works 
(compiles) correctly:

unsigned long fs,fd,a;

unsigned long _func(unsigned long x,unsigned long y)
{
        return 0;
}

int main(void)
{
        fs=(_func((fd/a),(_func(2,0x0082c90f))));
}

now if i wrap _func into a macro like this:

#define func(x,y)       _func(x,y)

int main(void)
{
        fs=(func((fd/a),(func(2,0x0082c90f))));
}

i get "Error: `)' expected" on that line. (this is with the snapshot, freshly 
compiled 5 minutes ago)
*/

unsigned long fs,fd,a;

unsigned long _func1(unsigned long x,unsigned long y)
{
        return 0;
}

int test1(void)
{
        fs=(_func1((fd/a),(_func1(2,0x0082c90f))));
}

#define func(x,y)       _func1(x,y)

int test2(void)
{
        fs=(func((fd/a),(func(2,0x0082c90f))));
}

int main(void)
{
    printf("it works :)\n");

    return 0;
}
