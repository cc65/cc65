/*
  !!DESCRIPTION!!
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

unsigned char success=0;
unsigned char failures=0;
unsigned char dummy=0;

#ifdef SUPPORT_BIT_TYPES
bit bit0 = 0;
#endif
unsigned int uint0 = 0;
unsigned int uint1 = 0;
unsigned char uchar0 = 0;
unsigned char uchar1 = 0;

void dput(unsigned char val)
{
	/*PORTB = val;
	PORTA = 0x01;
	PORTA = 0x00;
	*/
}

void done()
{
  dummy++;
}

/* both loops use the loop variable inside the inner loop */
void for1(void)
{
	unsigned char i, j;

	uchar0 = 0;
	uchar1 = 0;
	for(i = 0; i < 3; i++) {
		uchar0++;
		for(j = 0; j < 4; j++) {
			uchar1++;
			dput(i);
			dput(j);
		}
	}
	if(uchar0 != 3)
		failures++;
	if(uchar1 != 12)
		failures++;
}

/* only the outer loop's variable is used inside, inner can be optimized into a repeat-loop */
void for2(void)
{
	unsigned char i, j;

	uchar0 = 0;
	uchar1 = 0;
	for(i = 0; i < 3; i++) {
		uchar0++;
		for(j = 0; j < 4; j++) {
			uchar1++;
			dput(i);
		}
	}
	if(uchar0 != 3)
		failures++;
	if(uchar1 != 12)
		failures++;
}

/* only the inner loop's variable is used inside */
void for3(void)
{
	unsigned char i, j;

	uchar0 = 0;
	uchar1 = 0;
	for(i = 0; i < 3; i++) {
		uchar0++;
		for(j = 0; j < 4; j++) {
			uchar1++;
			dput(j);
		}
	}
	if(uchar0 != 3)
		failures++;
	if(uchar1 != 12)
		failures++;
}

/* neither loop variable used inside the loops */
void for4(void)
{
	unsigned char i, j;

	uchar0 = 0;
	uchar1 = 0;
	for(i = 0; i < 3; i++) {
		uchar0++;
		for(j = 0; j < 4; j++) {
			uchar1++;
			dput(uchar0);
			dput(uchar1);
		}
	}
	if(uchar0 != 3)
		failures++;
	if(uchar1 != 12)
		failures++;
}

/* like for1 but different condition in inner loop */
void for5(void)
{
	unsigned char i, j;

	uchar0 = 0;
	uchar1 = 0;
	for(i = 0; i < 3; i++) {
		uchar0++;
		for(j = 10; j >= 5; j--) {
			uchar1++;
			dput(i);
			dput(j);
		}
	}
	if(uchar0 != 3)
		failures++;
	if(uchar1 != 18)
		failures++;
}

int  main(void)
{
  for1();
  for2();
  for3();
  for4();
  for5();

  success = failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
