#include <cc65.h>
#include "unittest.h"

TEST
{
    unsigned i;
    
    for (i=0; i < 256; ++i)
    {
        ASSERT_AreEqual(i*20, mul20(i), "%u", "Invalid 'mul20(%u)' calculation!" COMMA i);
    }

    for (i=0; i < 256; ++i)
    {
        ASSERT_AreEqual(i*40, mul40(i), "%u", "Invalid 'mul40(%u)' calculation!" COMMA i);
    }
}
ENDTEST
