/*
  !!DESCRIPTION!! basic array properties
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include <stdio.h>

#ifndef NO_NEW_PROTOTYPES_FOR_OLD_FUNC_DECL
int f(void);
int g(int x[][4],int *y[]);
#endif

int x[3][4], *y[3];

main() {
	int z[3][4];
	int i, j, *p;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 4; j++)
			x[i][j] = 1000*i + j;
		y[i] = x[i];
	}
	f();
	for (i = 0; i < 3; i++) {
		y[i] = p = &z[i][0];
		for (j = 0; j < 4; j++)
			p[j] = x[i][j];
	}
	g(z, y);
	
	return 0;
}

f() {
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 4; j++)
			printf(" %d", x[i][j]);
	printf("\n");
	for (i = 0; i < 3; i++)
		for (j = 0; j < 4; j++)
			printf(" %d", y[i][j]);
	printf("\n");
}

g(x, y)
int x[][4], *y[];
{
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 4; j++)
			printf(" %d", x[i][j]);
	printf("\n");
	for (i = 0; i < 3; i++)
		for (j = 0; j < 4; j++)
			printf(" %d", y[i][j]);
	printf("\n");
}
