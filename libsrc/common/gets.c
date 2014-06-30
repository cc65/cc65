/*
** gets.c
**
** Ullrich von Bassewitz, 11.08.1998
*/



#include <stdio.h>
#include "_file.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



char* __fastcall__ gets (char* s)
{
    register char* p = s;
    int c;
    unsigned i = 0;

    while (1) {

        /* Get next character */
        if ((c = fgetc (stdin)) == EOF) {
            /* Error or EOF */
            *p = '\0';
            if (stdin->f_flags & _FERROR) {
                /* ERROR */
                return 0;
            } else {
                /* EOF */
                if (i) {
                    return s;
                } else {
                    return 0;
                }
            }
        }

        /* One char more. Newline ends the input */
        if ((char) c == '\n') {
            *p = '\0';
            break;
        } else {
            *p = c;
            ++p;
            ++i;
        }

    }

    /* Done */
    return s;
}




