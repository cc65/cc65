#include "unittest.h"

TEST
{
    unsigned int y=192;
    unsigned int d=y&0xFFF8;
    unsigned int e=d*32+d*8;
    unsigned int f=d*40;

    ASSERT_AreEqual(f, e, "%u", "Multiplication results differ (should be 7680)!");
}
ENDTEST
