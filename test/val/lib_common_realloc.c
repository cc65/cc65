#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "unittest.h"

TEST
{
    char *buf, *buf2;
    unsigned int i;

    buf = realloc(NULL, 0);
    ASSERT_IsTrue (buf == NULL, "realloc (NULL, 0) returned something");

    for (i = 1; i < 10; i++) {
      buf2 = realloc(buf, i);
      ASSERT_IsTrue (buf2 != NULL, "small realloc returned nothing");
      if (i > 1) {
        ASSERT_IsTrue (buf2 == buf, "buf shouldn't have moved");
      }
      buf = buf2;
    }

    buf = realloc(NULL, 15);
    ASSERT_IsTrue (buf != NULL, "realloc (NULL, 15) returned nothing");

    buf = realloc(buf, 0);
    ASSERT_IsTrue (buf == NULL, "realloc (buf, 0) returned something");

    buf = realloc(buf, 32);
    memset(buf, 'a', 32);
    for (i = 0; i < 32; i++) {
      ASSERT_IsTrue(buf[i] == 'a', "wrong contents in buf");
    }

    /* Now realloc larger, while there's nothing else in the heap */
    buf = realloc(buf, 64);
    memset(buf+32, 'b', 32);
    for (i = 0; i < 32; i++) {
      ASSERT_IsTrue(buf[i] == 'a', "wrong contents in start of buf");
    }
    for (i = 32; i < 64; i++) {
      ASSERT_IsTrue(buf[i] == 'b', "wrong contents in end of buf");
    }

    /* Now realloc smaller, while there's nothing else in the heap */
    buf = realloc(buf, 40);
    for (i = 0; i < 32; i++) {
      ASSERT_IsTrue(buf[i] == 'a', "wrong contents in start of buf");
    }
    for (i = 32; i < 40; i++) {
      ASSERT_IsTrue(buf[i] == 'b', "wrong contents in end of buf");
    }

    /* Allocate something else, so next realloc has to change block */
    malloc(50);

    /* Now realloc larger, with something else in the heap */
    buf = realloc(buf, 128);
    for (i = 0; i < 32; i++) {
      ASSERT_IsTrue(buf[i] == 'a', "wrong contents in start of buf");
    }
    for (i = 32; i < 40; i++) {
      ASSERT_IsTrue(buf[i] == 'b', "wrong contents in end of buf");
    }

    for (i = 129; i < 8192; i++) {
      buf = realloc(buf, i);
      ASSERT_IsTrue(buf != NULL, "realloc failed");
    }

    malloc(4096);

    buf2 = realloc(buf, 58000UL);
    ASSERT_IsTrue (buf2 == NULL, "realloc (buf, 58000) returned something");

    for (i = 65535UL; i > 65527UL; i--) {
      buf2 = realloc(buf, i);
      ASSERT_IsTrue (buf2 == NULL, "realloc returned something but shouldn't have");
    }
}
ENDTEST
