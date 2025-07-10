#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "unittest.h"

TEST
{
    char *buf, *buf2;
    unsigned int i;

    buf = malloc(0);
    ASSERT_IsTrue (buf == NULL, "malloc (0) returned something");

    for (i = 1; i < 10; i++) {
      buf = malloc(i);
      ASSERT_IsTrue (buf != NULL, "small returned nothing");
    }

    buf = malloc(4096);
    ASSERT_IsTrue (buf != NULL, "malloc (4096) returned nothing");

    buf = malloc(61000UL);
    ASSERT_IsTrue (buf == NULL, "malloc (61000) returned something");

    for (i = 65535UL; i > _heapmaxavail(); i--) {
      buf = malloc(i);
      ASSERT_IsTrue (buf == NULL, "malloc returned something but shouldn't have");
    }

    buf = malloc(i);
    ASSERT_IsTrue (buf != NULL, "malloc returned nothing but should have");
    ASSERT_IsTrue(_heapmaxavail() == 0, "heapmaxavail should be 0");
}
ENDTEST
