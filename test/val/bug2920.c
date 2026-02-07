#include "unittest.h"

TEST {
    /* The bug causes __func__ to be not null terminated. */
    const char *f = __func__;
    size_t size = sizeof(__func__);
    size_t i;

    /* Ensure the size is correct (5 for "main" + null terminator) */
    ASSERT_AreEqual((unsigned)size, 5u, "%u", "Sizeof __func__ should be 5");

    /* Check content */
    for (i = 0; i < size - 1; ++i) {
        ASSERT_AreEqual(f[i], "main"[i], "%c", "Character mismatch");
    }

    /* Check null terminator specifically */
    ASSERT_AreEqual(f[size - 1], '\0', "%02X", "Null terminator missing");
}
ENDTEST
