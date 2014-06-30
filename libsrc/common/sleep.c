/*
** sleep.c
**
** Stefan Haubenthal, 2003-06-11
** Ullrich von Bassewitz, 2003-06-12
**
*/



#include <time.h>
#include <unistd.h>



/* We cannot implement this function without a working clock function */
#if defined(CLOCKS_PER_SEC)
unsigned __fastcall__ sleep (unsigned wait)
{
    clock_t goal = clock () + ((clock_t) wait) * CLOCKS_PER_SEC;
    while ((long) (goal - clock ()) > 0) ;
    return 0;
}
#endif



