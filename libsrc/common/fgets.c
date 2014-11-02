/*
** Ullrich von Bassewitz, 11.08.1998
**
** char* fgets (char* s, int size, FILE* f);
*/



#include <stdio.h>
#include <errno.h>
#include "_file.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



char* __fastcall__ fgets (char* s, unsigned size, register FILE* f)
{
    register char* p = s;
    unsigned i;
    int c;

    if (size == 0) {
        /* Invalid size */
        return (char*) _seterrno (EINVAL);
    }

    /* Read input */
    i = 0;
    while (--size) {

        /* Get next character */
        if ((c = fgetc (f)) == EOF) {
            /* Error or EOF */
            if ((f->f_flags & _FERROR) != 0 || i == 0) {
                /* ERROR or EOF on first char */
                *p = '\0';
                return 0;
            } else {
                /* EOF with data already read */
                break;
            }
        }

        /* One char more */
        *p = c;
        ++p;
        ++i;

        /* Stop at end of line */
        if ((char)c == '\n') {
            break;
        }
    }

    /* Terminate the string */
    *p = '\0';

    /* Done */
    return s;
}



