/*
** fgetpos.c
**
** Christian Groessler, 07-Aug-2000
*/



#include <stdio.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int __fastcall__ fgetpos (FILE* f, fpos_t* pos)
{
    *pos = ftell (f);

    if (*pos != -1)
        return 0;
    return -1;
}

