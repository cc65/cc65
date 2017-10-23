#include <string.h>
#include "unittest.h"

#define EstimatedStringSize 384                         // test correct page passing (>256)

static char EstimatedString[EstimatedStringSize+1];     // +1 room for terminating null
static char* EmptyTestChars="";                         // strlen equivalent...
static char* TestChars="1234567890";                    // we like to find numbers


TEST
{
    unsigned i;
    
    for (i=0; i < EstimatedStringSize; ++i)
      EstimatedString[i] = (i%26)+'A';                 // put ABCD... into the string to be estimated

    ASSERT_AreEqual(strlen(EstimatedString), strcspn(EstimatedString, TestChars), "%u", "Unxpected position returned for non-participant case!");

    EstimatedString[EstimatedStringSize/2] = TestChars[strlen(TestChars-1)];
    ASSERT_AreEqual(EstimatedStringSize/2, strcspn(EstimatedString, TestChars), "%u", "Unxpected position returned for participant case!");

    ASSERT_AreEqual(strlen(EstimatedString), strcspn(EstimatedString, EmptyTestChars), "%u", "Unxpected position returned for empty test case!");
}
ENDTEST
