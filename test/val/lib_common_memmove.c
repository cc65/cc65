#include <string.h>
#include "unittest.h"

#define BufferSize 384               // test correct page passing (>256, multiple of 128 here)

static char Buffer[BufferSize+3];    // +1 to move up (and down)


TEST
{
    unsigned i, v;
    char*    p;
    
    for (i=0; i < BufferSize; ++i)
      Buffer[i+1] = (i%128);

    Buffer[0] = 255;                  // to check if start position is untouched
    Buffer[BufferSize+2] = 255;       // to check if end position is untouched

    // copy upwards
    p = memmove(Buffer+2, Buffer+1, BufferSize);

    // check buffer consistency before target
    ASSERT_AreEqual(255, (unsigned)Buffer[0], "%u", "Unexpected value before range!");

    // check buffer consistency at starting point
    ASSERT_AreEqual(0, (unsigned)Buffer[1], "%u", "Unexpected value at range start!");

    // check buffer consistency after range
    ASSERT_AreEqual(255, (unsigned)Buffer[BufferSize+2], "%u", "Unexpected value after range!");

    // check buffer values
    for (i=0; i < BufferSize; ++i)
    {
        ASSERT_AreEqual(i%128, (unsigned)Buffer[i+2], "%u", "Unexpected value in buffer at position %u!" COMMA i+2);
    }

    v = Buffer[BufferSize+1];   // rember value of first untouched end-byte 

    // copy downwards
    p = memmove(Buffer+1, Buffer+2, BufferSize);

    // check buffer consistency before target
    ASSERT_AreEqual(255, (unsigned)Buffer[0], "%u", "Unexpected value before range!");

    // check buffer consistency at end point
    ASSERT_AreEqual(v, (unsigned)Buffer[BufferSize+1], "%u", "Unexpected value at range end!");

    // check buffer consistency after range
    ASSERT_AreEqual(255, (unsigned)Buffer[BufferSize+2], "%u", "Unexpected value after range!");

    // check buffer values
    for (i=0; i < BufferSize; ++i)
    {
        ASSERT_AreEqual(i%128, (unsigned)Buffer[i+1], "%u", "Unexpected value in buffer at position %u!" COMMA i+1);
    }
}
ENDTEST
