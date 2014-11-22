/*
  !!DESCRIPTION!! check if character constants are translated correctly
  !!ORIGIN!!      cc65 bug report
  !!LICENCE!!     Public Domain
*/

#include "common.h"
#include <limits.h>
#include <ctype.h>

void backslash(unsigned char c)
{
    printf("%c : ",c);

    switch (c)
    {
	case 'b':
		c = '\b';
	case 'f':
		c = '\f';
	case 'n':
		c = '\n';
	case 'r':
		c = '\r';
	case 't':
		c = '\t';
	case 'v':
    #ifndef NO_BACKSLASH_V
        c = '\v';
    #else
        c = 0x0b;
    #endif
	}

    if(!isprint(c))
    {
        printf("ok.\n");
    }
    else
    {
        printf("failed.\n");
    }
}

void testbackslash(void)
{
        backslash('b');
        backslash('f');
        backslash('n');
        backslash('r');
        backslash('t');
        backslash('v');
}

int main(void)
{
	testbackslash();

	return 0;
}
