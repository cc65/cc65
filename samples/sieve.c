/*
 * Calculate all primes up to a specific number.
 */



#include <stdio.h>
#include <conio.h>



/*****************************************************************************/
/*     	      	    	  	     Data				     */
/*****************************************************************************/



#define COUNT		8192		/* Up to what number? */
#define SQRT_COUNT	91		/* Sqrt of COUNT */

static unsigned char Sieve[COUNT];



/*****************************************************************************/
/*     	      	    	  	     Code				     */
/*****************************************************************************/



int main (void)
{
    /* This is an example where register variables make sense */
    register unsigned char* S;
    register unsigned 	    I;
    register unsigned 	    J;

    /* Execute the sieve */
    I = 2;
    while (I < SQRT_COUNT) {
	if (Sieve[I] == 0) {
	    /* Prime number - mark multiples */
	    S = &Sieve[J = I*2];
       	    while (J < COUNT) {
	      	*S = 1;
	      	S += I;
	      	J += I;
	    }
	}
	++I;
    }

    /* Print the result */
    for (I = 2; I < COUNT; ++I) {
	if (Sieve[I] == 0) {
	    printf ("%4d\n", I);
	}
	if (kbhit() && cgetc() == 'q') {
	    break;
	}
    }

    return 0;
}



