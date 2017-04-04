#include <string.h>
#include "unittest.h"

#define SourceStringSize 384                            // test correct page passing (>256, multiple of 128 here)

static char SourceString[SourceStringSize+1];           // +1 room for terminating null
static char DestinationString[2*SourceStringSize+1];    // will contain two times the source buffer


TEST
{
    unsigned i;
    char*    p;
    
    for (i=0; i < SourceStringSize; ++i)
      SourceString[i] = (i%128)+1;

    SourceString[i] = 0;

    ASSERT_AreEqual(SourceStringSize, strlen(SourceString), "%u", "Source string initialization or 'strlen()' problem!");

    /* Ensure empty destination string */
    DestinationString[0] = 0;

    ASSERT_AreEqual(0, strlen(DestinationString), "%u", "Destination string initialization or 'strlen()' problem!");
    
    /* Test "unlimted" concatenation to empty buffer */

    strncat(DestinationString, SourceString, 1024);
    
    ASSERT_AreEqual(SourceStringSize, strlen(DestinationString), "%u", "Unexpected string length while string concatenation to empty buffer!");
    
    /* Test limited concatenation to non empty buffer */

    p = strncat(DestinationString, SourceString, 128);

    ASSERT_AreEqual(SourceStringSize+128, strlen(DestinationString), "%u", "Unexpected string length while string concatenation to non-empty buffer!");

    /* Test return value */

    ASSERT_IsTrue(p == DestinationString, "Invalid return value!");

    /* Test contents */

    for(i=0; i < strlen(DestinationString); ++i)
    {
        unsigned current = DestinationString[i];
        unsigned expected = (i%128)+1;
        ASSERT_AreEqual(expected, current, "%u", "Unexpected destination buffer contents at position %u!\n" COMMA i);
    }
}
ENDTEST
