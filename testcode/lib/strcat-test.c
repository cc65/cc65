#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SourceStringSize 257                            // test correct page passing (>256)

static char SourceString[SourceStringSize+1];		// +1 room for terminating null
static char DestinationString[2*SourceStringSize+1];    // will contain two times the source buffer


int main (void)
{
    unsigned i,j;
    char*    p;

    /* Print a header */
    printf ("strcat(): ");

    for (i=0; i < SourceStringSize; ++i)
      SourceString[i] = (i%128)+1;

    SourceString[i] = 0;

    if (strlen(SourceString) != SourceStringSize)
    {
        printf ("Fail: Source string initialization or 'strlen()' problem!\n");
        printf ("Expected length: %u but is %u!\n", SourceStringSize, strlen(SourceString));
        exit (EXIT_FAILURE);
    }

    /* Ensure empty destination string */
    DestinationString[0] = 0;

    if (strlen(DestinationString) != 0)
    {
        printf ("Fail: Destination string initialization or 'strlen()' problem!\n");
        printf ("Expected length: %u but is %u!\n", 0, strlen(DestinationString));
        exit (EXIT_FAILURE);
    }

    /* Test concatenation to empty buffer */

    p = strcat(DestinationString, SourceString);

    if (strlen(DestinationString) != SourceStringSize)
    {
        printf ("Fail: String concatenation to empty buffer!\n");
        printf ("Expected length: %u but is %u!\n", SourceStringSize, strlen(DestinationString));
        exit (EXIT_FAILURE);
    }    

    /* Test concatenation to non empty buffer */

    p = strcat(DestinationString, SourceString);

    if (strlen(DestinationString) != 2*SourceStringSize)
    {
        printf ("Fail: String concatenation to non-empty buffer!\n");
        printf ("Expected length: %u but is %u!\n", 2*SourceStringSize, strlen(DestinationString));
        exit (EXIT_FAILURE);
    }   

    /* Test return value */

    if (p != DestinationString)
    {
        printf ("Invalid return value!\n");
        exit (EXIT_FAILURE);
    }   

    /* Test contents */

    for(j=0; j <2; ++j)
        for(i=0; i < SourceStringSize; ++i)
        {
            unsigned position = j*SourceStringSize+i;
            unsigned current = DestinationString[position];
            unsigned expected = (i%128)+1;
            if (current != expected)
            {
                printf ("Fail: Unexpected destination buffer contents at position %u!\n", position);
                printf ("Expected %u, but is %u!\n", expected, current);
                exit (EXIT_FAILURE);
            }
        }

    /* Test passed */
    printf ("Passed\n");
    return EXIT_SUCCESS;
}



