#include <string.h>
#include "unittest.h"

#define SourceStringSize 257                            // test correct page passing (>256)

static char SourceString[SourceStringSize+1];           // +1 room for terminating null
static char DestinationString[2*SourceStringSize+1];    // will contain two times the source buffer


TEST
{
    unsigned i,j;
    char*    p;
    
    for (i=0; i < SourceStringSize; ++i)
      SourceString[i] = (i%128)+1;

    SourceString[i] = 0;

    ASSERT_AreEqual(SourceStringSize, strlen(SourceString), "%u", "Source string initialization or 'strlen()' problem!");

    /* Ensure empty destination string */
    DestinationString[0] = 0;

    ASSERT_AreEqual(0, strlen(DestinationString), "%u", "Destination string initialization or 'strlen()' problem!");
    
    /* Test concatenation to empty buffer */

    strcat(DestinationString, SourceString);
    
    ASSERT_AreEqual(SourceStringSize, strlen(DestinationString), "%u", "Unexpected string length while string concatenation to empty buffer!");
    
    /* Test concatenation to non empty buffer */

    p = strcat(DestinationString, SourceString);

    ASSERT_AreEqual(2*SourceStringSize, strlen(DestinationString), "%u", "Unexpected string length while string concatenation to non-empty buffer!");

    /* Test return value */

    ASSERT_IsTrue(p == DestinationString,"Invalid return value!");

    /* Test contents */

    for(j=0; j <2; ++j)
        for(i=0; i < SourceStringSize; ++i)
        {
            unsigned position = j*SourceStringSize+i;
            unsigned current = DestinationString[position];
            unsigned expected = (i%128)+1;
            ASSERT_AreEqual(expected, current, "%u", "Unexpected destination buffer contents at position %u!\n" COMMA position);
        }
}
ENDTEST
