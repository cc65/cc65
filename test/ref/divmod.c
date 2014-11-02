/*
  !!DESCRIPTION!! div/mod test
  !!ORIGIN!!
  !!LICENCE!!     public domain
*/

#include <stdio.h>

void printc(signed char a,signed char b){
signed char x=a/b,y=a%b,z=a*b;
	printf("%3d,%3d is %3d,%3d,%3d\n",a,b,x,y,z);
}
void prints(short a,short b){
short x=a/b,y=a%b,z=a*b;
	printf("%3d,%3d is %3d,%3d,%3d\n",a,b,x,y,z);
}
void printl(long a,long b){
long x=a/b,y=a%b,z=a*b;
	printf("%3ld,%3ld is %3ld,%3ld,%3ld\n",a,b,x,y,z);
}

int main(void) {
	printl( 3,-2);
	printl(-3,-2);
	printl(-3, 2);
	printl( 3, 2);
	printf("-\n");
	prints( 3,-2);
	prints(-3,-2);
	prints(-3, 2);
	prints( 3, 2);
	printf("-\n");
	printc( 3,-2);
	printc(-3,-2);
	printc(-3, 2);
	printc( 3, 2);
	return 0;
}
