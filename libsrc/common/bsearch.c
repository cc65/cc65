/*
** bsearch.c
**
** 1998-06-17, Ullrich von Bassewitz
** 2015-06-21, Greg King
*/



#include <stdlib.h>



void* __fastcall__ bsearch (const void* key, const void* base, size_t n,
                            size_t size, int __fastcall__ (* cmp) (const void*, const void*))
{
    int current;
    int result;
    int found = 0;
    int first = 0;
    int last = n - 1;

    /* Binary search */
    while (first <= last) {

        /* Set current to mid of range */
        current = (last + first) / 2;

        /* Do a compare */
        result = cmp ((void*) (((int) base) + current*size), key);
        if (result < 0) {
            first = current + 1;
        } else {
            last = current - 1;
            if (result == 0) {
                /* Found one entry that matches the search key. However there may be
                ** more than one entry with the same key value and ANSI guarantees
                ** that we return the first of a row of items with the same key.
                */
                found = 1;
            }
        }
    }

    /* Did we find the entry? */
    return (void*) (found? ((int) base) + first*size : 0);
}



