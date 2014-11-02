/*
** rewind.c
**
** Christian Groessler, 07-Aug-2000
*/



#include <stdio.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void __fastcall__ rewind (FILE* f)
{
    fseek(f, 0L, SEEK_SET);
    clearerr(f);
}

                  
