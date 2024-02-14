#include <string.h>
#include "unittest.h"

#define BufferSize 384               // test correct page passing (>256, multiple of 128 here)

static char Buffer1[BufferSize];    // +1 to move up (and down)
static char Buffer2[BufferSize];    // +1 to move up (and down)


TEST
{
    unsigned i, v;
    char*    p;

    for (i=0; i < BufferSize; ++i) {
      Buffer1[i] = i;
      Buffer2[i] = ~i;
    }

    memcpy(Buffer2, Buffer1, sizeof(Buffer1));
    if (memcmp(Buffer1, Buffer2, sizeof(Buffer1))) {
      printf("First memcpy failed\n");
      exit(1);
    }

    for (i=0; i < BufferSize; ++i) {
      Buffer1[i] = i;
      Buffer2[i] = ~i;
    }

    memcpy(Buffer1, Buffer2, sizeof(Buffer1));
    if (memcmp(Buffer2, Buffer1, sizeof(Buffer1))) {
      printf("Second memcpy failed\n");
      exit(1);
    }
}
ENDTEST
