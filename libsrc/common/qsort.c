/*
** qsort.c
**
** 1998.12.09, Ullrich von Bassewitz
** 2015-06-21, Greg King
*/



#include <stdlib.h>



static void QuickSort (register unsigned char* Base, int Lo, int Hi,
                       register size_t Size,
                       int __fastcall__ (* Compare) (const void*, const void*))
/* Internal recursive function. Works with ints, but this shouldn't be
** a problem.
*/
{
    int I, J;

    /* Quicksort */
    while (Hi > Lo) {
        I = Lo + Size;
        J = Hi;
        while (I <= J) {
            while (I <= J && Compare (Base + Lo, Base + I) >= 0) {
                I += Size;
            }
            while (I <= J && Compare (Base + Lo, Base + J) < 0) {
                J -= Size;
            }
            if (I <= J) {
                __swap (Base + I, Base + J, Size);
                I += Size;
                J -= Size;
            }
        }
        if (J != Lo) {
            __swap (Base + J, Base + Lo, Size);
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
                         int __fastcall__ (* compare) (const void*, const void*))
/* Quicksort implementation */
{
    if (nmemb > 1) {
        QuickSort (base, 0, (nmemb-1) * size, size, compare);
    }
}



