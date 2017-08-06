#include <string.h>
#include "unittest.h"

                                                        
/* Test string. Must NOT have duplicate characters! */
static char S[] = "Helo wrd!\n";

static char Found[256];



TEST
{
    unsigned Len;
    unsigned I;
    char*    P;

    /* Get the length of the string */
    Len = strlen (S);

    /* Search for all characters in the string, including the terminator */
    for (I = 0; I < Len+1; ++I)
    {
        /* Search for this char */
        P = strchr (S, S[I]);

        /* Check if we found it */
        ASSERT_IsFalse(P == 0 || (P - S) != I, "For code 0x%02X, offset %u!\nP = %04X offset = %04X\n" COMMA S[I] COMMA I COMMA P COMMA P-S);
        /* Mark the char as checked */
        Found[S[I]] = 1;
    }

    /* Search for all other characters and make sure they aren't found */
    for (I = 0; I < 256; ++I)
    {
        if (Found[I] == 0)
        {
            ASSERT_IsFalse(strchr (S, (char)I), "Failed for code 0x%02X\n" COMMA I);
        }
    }
}
ENDTEST
