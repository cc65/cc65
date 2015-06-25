/*
  !!DESCRIPTION!!
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>

#define NO_IMPLICIT_FUNCPTR_CONV

unsigned char success=0;
unsigned char failures=0;
unsigned char dummy=0;

#ifdef SUPPORT_BIT_TYPES
bit bit0 = 0;
#endif
unsigned int uint0 = 0;
unsigned int uint1 = 0;

/*
 * BUG: if these aren't volatile, an overzealous optimizer or somthing
 * wreaks havoc with the simple tests like "if(uchar != 3)failures++"
 */

#if 0
unsigned char uchar0 = 0;
unsigned char uchar1 = 0;
unsigned char uchar2 = 0;
#else
volatile unsigned char uchar0 = 0;
volatile unsigned char uchar1 = 0;
volatile unsigned char uchar2 = 0;
#endif

#ifdef NO_IMPLICIT_FUNCPTR_CONV
void (*pfunc)(void);
void (*p1func)(void);
unsigned char (*pcfunc)(void);
#else
void (*pfunc)();
void (*p1func)();
unsigned char (*pcfunc)();
#endif

void done()
{
  dummy++;
}

void call0(void)
{
	uchar0++;
}

void call1(void)
{
	uchar1++;
}

unsigned char call2(void)
{
	return uchar0 + 9;
}

void docall0(void)
{
	pfunc = call0;
	(pfunc)();
	if(uchar0 != 1)
		failures++;
}

void docall1()
{
	unsigned char i;
	for(i = 0; i < 3; i++) {
		(*p1func)();
	}
}

#ifdef NO_IMPLICIT_FUNCPTR_CONV
void docall2( void(*pf)(void) )
#else
void docall2( void(*pf)() )
#endif
{
	unsigned char i;
	for(i = 0; i < 2; i++) {
		pf();
	}
}

int  main(void)
{
    docall0();

	p1func = call1;
	docall1();
	if(uchar1 != 3)
		failures++;
	if(uchar0 != 1)
		failures++;

	p1func = call0;
	docall1();
	if(uchar1 != 3)
		failures++;
	if(uchar0 != 4)
		failures++;

	docall2(call0);
	if(uchar1 != 3)
		failures++;
	if(uchar0 != 6)
		failures++;

	docall2(call1);
	if(uchar1 != 5)
		failures++;
	if(uchar0 != 6)
		failures++;

 	pcfunc = call2;
 	uchar2 = (*pcfunc)();
	if(uchar2 != 15)
		failures++;

 	uchar2 += (pcfunc)();
	uchar2 += pcfunc();

	success = failures;
	done();
	printf("failures: %d\n",failures);

	return failures;
}
