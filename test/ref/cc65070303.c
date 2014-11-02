/*
  !!DESCRIPTION!!
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!
*/

#include <stdio.h>

typedef signed int TypA[3];
typedef struct TypB {
	TypA Data[2];
} sTypB;
sTypB Bs[10];
TypA * APtr;

int main(int argc, char* argv[])
{
	Bs[7].Data[1][2]=11;
	APtr=&(Bs[7].Data[1]);
	printf("Hallo Welt! %i = %i \n",Bs[7].Data[1][2], (*APtr)[2] );
	return 0;
}

/*
....gives
test.c(20): Error: Incompatible pointer types
for   APtr=&(Bs[7].Data[1]);

My experience in C is very limited, but as this works both in MSVC and 
the 8 bit Z80 compiler i originally used, i guess its an bug in CC65.

As a workaround, an typecast via  APtr=(TypA*)&(Bs[7].Data[1]);
seems to work.

greetings,
   Andreas
*/