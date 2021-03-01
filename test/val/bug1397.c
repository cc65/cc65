
/* bug #1937 - Incorrect Behavior Related to OptBoolTrans */

#include <stdio.h>

unsigned char c;
int *p;

void f1(void) {
	int i = 1;
	int *pa = (int *)0xaaaa;
	int *pb = (int *)0xbbbb;

	p = (i == 0) ? pa : pb;
	c = 0x5a;	
}


struct data_t {
	unsigned char c;
	int *p;	
};

struct data_t data;

void f2(void) {
	int i = 1;
	int *pa = (int *)0xcccc;
	int *pb = (int *)0xdddd;
	struct data_t *po = &data;
	
 	po->p = (i == 0) ? pa : pb;
 	po->c = 0xa5;	
}

int ret = 0;

int main(void) {
	f1();
    if (c != 0x5a) {
        ret++;
    }
	printf("c: %hhx\n", c);
	printf("p: %p\n", p);
	f2();
    if (data.c != 0xa5) {
        ret++;
    }
	printf("c: %hhx\n", data.c);
	printf("p: %p\n", data.p);

	printf("failures: %d\n", ret);
    return ret;
}

