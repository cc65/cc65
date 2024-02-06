/*
** system.c
**
** Stefan Haubenthal, 2003-05-26
** Ullrich von Bassewitz, 2003-05-27
*/

#include <stdio.h>
#include <stdlib.h>

int __fastcall__ system (const char* s)
{
    if (s == NULL) {
        return 0;               /* no shell */
    } else {
        return -1;              /* always fail */
    }
}



