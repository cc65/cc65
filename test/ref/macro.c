/*
  !!DESCRIPTION!! macro bug test program
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Groepaz/Hitmen
*/

#include <stdio.h>
#include <stdlib.h>

unsigned long fs=7;
unsigned long fd=5;
unsigned long a=3;

unsigned long _func(unsigned long x,unsigned long y)
{
	printf("x:%ld y:%ld\n",x,y);
	return 0;
}

#define func(x,y)	_func(x,y)

int main(void)
{
	fs= func( (fd/a) ,  func(2,0x0082c90f) );
	printf("fs:%ld\n",fs);
	fs=_func( (fd/a) , _func(2,0x0082c90f) );
	printf("fs:%ld\n",fs);
	return 0;
}
