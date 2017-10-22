#include <string.h>
#include "unittest.h"

#define EstimatedStringSize 384                         // test correct page passing (>256)

static char EstimatedString[EstimatedStringSize+1];     // +1 room for terminating null
static char* EmptyTestChars="";                         // empty test case...
static char* TestChars="1234567890";                    // we like to find numbers

TEST
{
    unsigned i;
    
    for (i=0; i < EstimatedStringSize; ++i)
      EstimatedString[i] = (i%10)+'0';                 // put 0123... into the string to be estimated

    ASSERT_AreEqual(strlen(EstimatedString), strspn(EstimatedString, TestChars), "%u", "Unxpected position returned for all participant case!");

    EstimatedString[EstimatedStringSize/2] = 'X';
    ASSERT_AreEqual(EstimatedStringSize/2, strspn(EstimatedString, TestChars), "%u", "Unxpected position returned for breaking case!");

    ASSERT_AreEqual(0, strspn(EstimatedString, EmptyTestChars), "%u", "Unxpected position returned for empty test case!");
}
ENDTEST
