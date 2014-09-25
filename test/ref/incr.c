/*
  !!DESCRIPTION!! increment/decrement
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include <stdio.h>

int main(void)
{
    printf("disassemble this program to check the generated code.\n");
    return 0;
}

memchar() {
	char x, *p;

	&x, &p;
	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

memint() {
	int x, *p;

	&x, &p;
	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

regchar() {
	register char x, *p;

	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}

regint() {
	register int x, *p;

	x = *p++;
	x = *++p;
	x = *p--;
	x = *--p;
}
