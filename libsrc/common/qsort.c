/*
 * qsort.c
 *
 * Ullrich von Bassewitz, 09.12.1998
 */



#include <stdlib.h>



static void QuickSort (void* Base, int Lo, int Hi, size_t Size,
	               int (*Compare)(const void*, const void*))
/* Internal recursive function. Works with ints, but this shouldn't be
 * a problem.
 */
{
    int I, J;

    /* Get a char pointer */
    unsigned char* B = Base;

    /* Quicksort */
    while (Hi > Lo) {
   	I = Lo + Size;
   	J = Hi;
   	while (I <= J) {
   	    while (I <= J && Compare (B + Lo, B + I) >= 0) {
   	     	I += Size;
   	    }
   	    while (I <= J && Compare (B + Lo, B + J) < 0) {
   	     	J -= Size;
   	    }
   	    if (I <= J) {
   	     	_swap (B + I, B + J, Size);
   	     	I += Size;
   	     	J -= Size;
   	    }
      	}
   	if (J != Lo) {
   	    _swap (B + J, B + Lo, Size);
   	}
       	if (((unsigned) J) * 2 > (Hi + Lo)) {
	    QuickSort (Base, J + Size, Hi, Size, Compare);
	    Hi = J - Size;
	} else {
	    QuickSort (Base, Lo, J - Size, Size, Compare);
	    Lo = J + Size;
	}
    }
}



void __fastcall__ qsort (void* base, size_t nmemb, size_t size,
	                 int (*compare)(const void*, const void*))
/* Quicksort implementation */
{
    if (nmemb > 1) {
       	QuickSort (base, 0, (nmemb-1) * size, size, compare);
    }
}





