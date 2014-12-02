/*
  !!DESCRIPTION!! solves the "8 queens" chess problem
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include <stdio.h>

int up[15], down[15], rows[8], x[8];
void queens(int c);
void print(void);

int main(void)
{
	int i;
	for (i = 0; i < 15; i++)
		up[i] = down[i] = 1;
	for (i = 0; i < 8; i++)
		rows[i] = 1;
	queens(0);
	return 0;
}

void queens(int c)
{
	int r;

	for (r = 0; r < 8; r++)
		if (rows[r] && up[r-c+7] && down[r+c]) {
			rows[r] = up[r-c+7] = down[r+c] = 0;
			x[c] = r;
			if (c == 7)
				print();
			else
				queens(c + 1);
			rows[r] = up[r-c+7] = down[r+c] = 1;
		}
}

void print(void)
{
	int k;

	for (k = 0; k < 8; k++) {
		printf("%c", x[k]+'1');
		if(k<7) printf(" ");
	}
	printf("\n");
}
