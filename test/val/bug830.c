#include "unittest.h"

char test[1];
char *dst = &test[0];

TEST
{
    char src = 0;
    *dst = (src == 0) ? 42 : src;

    ASSERT_AreEqual(42, *dst, "%u", "Incorrect ternary expression evaluation!");
}
ENDTEST
