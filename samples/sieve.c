/*
 * Calculate all primes up to a specific number.
 */



#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>



/*****************************************************************************/
/*     	      	    	  	     Data				     */
/*****************************************************************************/



#define COUNT		8192		/* Up to what number? */
#define SQRT_COUNT	91		/* Sqrt of COUNT */

static unsigned char Sieve[COUNT];



/*****************************************************************************/
/*     	      	    	  	     Code	      			     */
/*****************************************************************************/



#pragma staticlocals(1);



int main (void)					      
{
    /* Clock variable */
    clock_t Ticks;

    /* This is an example where register variables make sense */
    register unsigned char* S;
    register unsigned 	    I;
    register unsigned 	    J;

    /* Read the clock */
    Ticks = clock();

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

    /* Calculate the time used */
    Ticks = clock() - Ticks;

    /* Print the time used */
    printf ("Time used: %lu ticks\n", Ticks);
    printf ("Press Q to quit, any other key for list\n");

    /* Wait for a key and print the list if not 'Q' */
    if (toupper (cgetc()) != 'Q') {
     	/* Print the result */
     	for (I = 2; I < COUNT; ++I) {
     	    if (Sieve[I] == 0) {
     		printf ("%4d\n", I);
     	    }
     	    if (kbhit() && toupper (cgetc()) == 'Q') {
     		break;
     	    }
     	}
    }

    return EXIT_SUCCESS;
}



