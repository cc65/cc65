/*
  !!DESCRIPTION!! bitfield test
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include "common.h"

#ifdef NO_BITFIELDS

main()
{
        printf("NO_BITFIELDS\n\r");
}

#else

#ifdef SIZEOF_INT_16BIT

#ifdef REFCC
#include <stdint.h>
struct foo {
	int16_t a;
	char b;
	int16_t x : 12, y : 4;
	int16_t zz : 1, : 0, : 4, z : 3;
	char c;
} x = { 1, 2, 3, 4, 5, 6 };

struct baz { uint16_t a:2, b:4, c:16;} y = { 7, 8, 9};
int16_t i = 8;

#else

struct foo {
	int a;
	char b;
	int x : 12, y : 4;
	int zz : 1, : 0, : 4, z : 3;
	char c;
} x = { 1, 2, 3, 4, 5, 6 };

struct baz { unsigned int a:2, b:4, c:16;} y = { 7, 8, 9};
int i = 8;
#endif

#else
struct foo {
	int a;
	char b;
	int x : 12, y : 4, : 0, : 4, z : 3;
	char c;
} x = { 1, 2, 3, 4, 5, 6 };

struct baz { unsigned int a:2, b:4, c:32;} y = { 7, 8, 9};
int i = 16;
#endif

#ifdef NO_IMPLICIT_FUNC_PROTOTYPES
f1(struct baz *p);
f2(struct baz *p);
#endif

main()
{
	printf("x = %d b:%d %d %d %d c:%d\n", x.a, x.b, x.x, x.y, x.z, x.c);
	printf("y = %d b:%d c:%d\n", y.a, y.b, y.c);
	x.y = i;
	x.z = 070;
	printf("x = %d b:%d %d %d %d c:%d\n", x.a, x.b, x.x, x.y, x.z, x.c);
	y.a = 2;
	y.c = i;
	printf("y = %d b:%d c:%d\n", y.a, y.b, y.c);
#ifdef CAST_STRUCT_PTR
	f2((struct baz *)&x);
#else
	f2(&x);
#endif
	return 0;
}

f1(struct baz *p) {
	p->a = p->b = 0;
	if (p->b)
		printf("p->b != 0!\n");
	p->a = 0x3; p->b = 0xf;
	printf("p->a = 0x%x, p->b = 0x%x\n", p->a, p->b);
}
f2(struct baz *p) {
	p->a = (i==0);
	p->b = (f1(p),0);
}

#endif
