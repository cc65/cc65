/*
  !!DESCRIPTION!! switch test
  !!ORIGIN!!
  !!LICENCE!!     public domain
*/

/*#define STANDALONE*/

#include <stdio.h>

void testlimits(int i) {
	printf("%d:",i);

	switch(i) {
		case -1:        /* works */
		/* case 0xffff: */ /* 'range error' (-1) */

			printf("-1\n");
			break;
		/* max int */

/*		 case 0x7fff:	*/	   /* works */
		case 32767: 		   /* works */
		/* case 32768: */	   /* 'range error' (correct for that one!) */

			printf("max\n");
			break;
		/* min int */

		case -32768: 		   /* 'warning. constant is long' */
		/* case 0x8000: */	       /* 'range error' */
		/* case -32769: */	   /* 'range error' (correct for that one!) */
			printf("min\n");
			break;
	}
	printf("\n");
}

void testdefault1(unsigned char i) {
/* we want a signed char */
#ifdef REFCC

#ifdef REFCC_UNSIGNED_CHARS
signed char k;
#else
char k;
#endif
	
#else
	
#ifdef UNSIGNED_CHARS
signed char k;
#else
char k;
#endif

#endif

	for(;i<254;) {
		k = i;
		printf(">%d\n",i);i++;
		switch(k) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14:
			break;
		case 15:
			break;
		case 17:
			break;
		/* triggers bug ? */
		/* gcc warning: case label value exceeds maximum value for type */
		/* cc65 error: range error */

		/*
		case 170:
			break;
		*/
		case 18:
			break;
		case 19:
			break;
		case 20:
			break;
		case 21:
			break;
		case 22:
			break;
		case 23:
			break;
		case 24:
			switch(k) {
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
				case 5:
				break;
				case 6:
				case 7:
					break;
				case 8:
				case 9:
					break;
				}
			break;
		case 100:
			break;
		default:
			printf(">>>default\n");
			/* triggers bug if this break; is missing? */
			/* break; */
		}
	}
}

void testdefault2(unsigned char i) {
/* we want a unsigned char */
#ifdef REFCC

#ifdef REFCC_UNSIGNED_CHARS
char k;
#else
unsigned char k;
#endif
	
#else
	
#ifdef UNSIGNED_CHARS
char k;
#else
unsigned char k;
#endif

#endif

	for(;i<254;) {
		k = i;
		printf(">%d\n",i);i++;
		switch(k) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14:
			break;
		case 15:
			break;
		case 17:
			break;
		/* triggers bug ? */

		case 170:
			break;
		
		case 18:
			break;
		case 19:
			break;
		case 20:
			break;
		case 21:
			break;
		case 22:
			break;
		case 23:
			break;
		case 24:
			switch(k) {
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
				case 5:
				break;
				case 6:
				case 7:
					break;
				case 8:
				case 9:
					break;
				}
			break;
		case 100:
			break;
		default:
			printf(">>>default\n");
			/* triggers bug if this break; is missing? */
			/* break; */
		}
	}
}

int main(void) {
	testlimits(32767);
	testlimits(-32768);
	testlimits(-1);
	
	testdefault1(1);
	testdefault1(2);
	testdefault1(3);
	testdefault1(4);
	
	testdefault2(1);
	testdefault2(2);
	testdefault2(3);
	testdefault2(4);

	return 0;
}
