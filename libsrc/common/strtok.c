/*
** strtok.c
**
** Ullrich von Bassewitz, 11.12.1998
*/



#include <string.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Memory location that holds the last input */
static char* Last = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



char* __fastcall__ strtok (register char* s1, const char* s2)
{
    char c;
    char* start;

    /* Use the stored location if called with a NULL pointer */
    if (s1 == 0) {
        s1 = Last;
    }

    /* If s1 is empty, there are no more tokens. Return 0 in this case. */
    if (*s1 == '\0') {
        return 0;
    }

    /* Search the address of the first element in s1 that equals none
    ** of the characters in s2.
    */
    while ((c = *s1) && strchr (s2, c) != 0) {
        ++s1;
    }
    if (c == '\0') {
        /* No more tokens found */
        Last = s1;
        return 0;
    }

    /* Remember the start of the token */
    start = s1;

    /* Search for the end of the token */
    while ((c = *s1) && strchr (s2, c) == 0) {
        ++s1;
    }
    if (c == '\0') {
        /* Last element */
        Last = s1;
    } else {
        *s1 = '\0';
        Last = s1 + 1;
    }

    /* Return the start of the token */
    return start;
}



