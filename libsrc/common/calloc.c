/*
 * calloc.c
 *
 * Ullrich von Bassewitz, 06.06.1998
 */



#include <stdlib.h>
#include <string.h>



void* calloc (size_t count, size_t size)
{
    void* mem;
    size *= count;
    if (mem = malloc (size)) {
	memset (mem, 0, size);
    }
    return mem;
}



