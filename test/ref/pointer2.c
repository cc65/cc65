/*
  !!DESCRIPTION!! pointer test
  !!ORIGIN!!
  !!LICENCE!!     public domain
*/

#include "common.h"
#include <stdio.h>

/*
  check behaviour on incompletely declared arrays
*/

char i1[];

void test1(void) {
int a;

	a=sizeof(i1[0]);
	printf("%04x - ",a);
	if(sizeof(i1[0])==sizeof(char)) {
		/* gcc gives size of element */
		printf("sizeof(i1[0]) gives size of element\n");
	}
	if(sizeof(i1[0])==sizeof(char*)) {
		printf("sizeof(i1[0]) gives size of pointer to element\n");
	}
}

/*
  check behaviour on string init
*/

char t1[]="abcde";
char t2[]={"abcde"};

char *t3="abcde";
char *t4={"abcde"};

void test2(void) {
char c1,c2,c3,c4;
int i,e=0;
	for(i=0;i<5;i++){
		c1=t1[i];c2=t2[i];c3=t3[i];c4=t4[i];
/*		printf("%02x %02x %02x %02x\n",c1,c2,c3,c4); */
		printf("%c %c %c %c\n",c1,c2,c3,c4);
		if(!((c1==c2)&(c1==c3)&(c1==c4))) e=1;
	}
	if(e) printf("test2 failed.\n");
	else printf("test2 ok.\n");
}

/*
  check behaviour on extern-declarations inside functions
*/

typedef struct {
  char *name;
  void *func;
} A3;

#ifdef NO_SLOPPY_STRUCT_INIT
A3 a3[] = {
  { "test3", (void*) NULL },
  { "test3", (void*) NULL },
};
#else
/*gcc warning: missing braces around initializer (near initialization for `a3[0]')
  this type of struct-initialization seems to be kinda common */
A3 a3[] = {
    "test3", (void*) NULL  ,
    "test3", (void*) NULL  ,
};
#endif

void test3a(A3 *list, int number){
	printf("%s %d\n",list->name,number);
}

static void test31(void)
{
    extern A3 a3[];
    test3a(a3, -1);
}

#if 0
/* this variation compiles and works with cc65, but gives an error with gcc :=P */
static void test32(void)
{
    extern A3 *a3;
    test3a(a3, -1);
}
#endif

static void test30(void)
{
    test3a(a3, -1);
}

/*
  todo: add test on function pointers in the form of (*func)(arg) ...
  cc65 seems to have problems here aswell ;/
*/

int main(void) {
  	test1();
	test2();
	test30();
	test31();
/*	test32(); */
	return 0;
}
