#include <stdio.h>
#include <stdlib.h>
#include <string.h>


                                                        
/* Test string. Must NOT have duplicate characters! */
static char S[] = "Helo wrd!\n";

static char Found[256];



int main (void)
{
    unsigned Len;
    unsigned I;
    char*    P;

    /* Print a header */
    printf ("strchr(): ");

    /* Get the length of the string */
    Len = strlen (S);

    /* Search for all characters in the string, including the terminator */
    for (I = 0; I < Len+1; ++I) {

        /* Search for this char */
        P = strchr (S, S[I]);

        /* Check if we found it */
        if (P == 0 || (P - S) != I) {
            printf ("Failed for code 0x%02X, offset %u!\n", S[I], I);
            printf ("P = %04X offset = %04X\n", P, P-S);
            exit (EXIT_FAILURE);
        }

        /* Mark the char as checked */
        Found[S[I]] = 1;
    }

    /* Search for all other characters and make sure they aren't found */
    for (I = 0; I < 256; ++I) {
        if (Found[I] == 0) {
            if (strchr (S, (char)I) != 0) {
                printf ("Failed for code 0x%02X\n", I);
                exit (EXIT_FAILURE);
            }
        }
    }

    /* Test passed */
    printf ("Passed\n");
    return EXIT_SUCCESS;
}



