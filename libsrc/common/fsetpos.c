/*
 * fsetpos.c
 *
 * Christian Groessler, 07-Aug-2000
 */


#include <stdio.h>


int fsetpos(FILE* f, const fpos_t *pos)
{
    return fseek (f, (fpos_t)*pos, SEEK_SET);
}

