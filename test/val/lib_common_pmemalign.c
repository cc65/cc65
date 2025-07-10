#include <errno.h>
#include <stdlib.h>
#include "unittest.h"

TEST
{
    void *buf;
    int r;

    r = posix_memalign(&buf, 123, 1024);
    ASSERT_IsTrue(r == EINVAL, "posix_memalign did not return EINVAL with wrong alignment");
    ASSERT_IsTrue(buf == NULL, "posix_memalign did not set buf to NULL with wrong alignment");

    r = posix_memalign(&buf, 0, 1024);
    ASSERT_IsTrue(r == EINVAL, "posix_memalign did not return EINVAL with 0 alignment");
    ASSERT_IsTrue(buf == NULL, "posix_memalign did not set buf to NULL with 0 alignment");

    r = posix_memalign(&buf, 256, 0);
    ASSERT_IsTrue(r == EINVAL, "posix_memalign did not return EINVAL with 0 size");
    ASSERT_IsTrue(buf == NULL, "posix_memalign did not set buf to NULL with 0 size");

    r = posix_memalign(&buf, 256, 32768U);
    ASSERT_IsTrue(r == 0, "posix_memalign did not return 0 on correct call");
    ASSERT_IsTrue(buf != NULL, "posix_memalign left buf set to NULL on correct call");
    ASSERT_IsTrue(((unsigned int)buf & 0x00FF) == 0x00, "posix_memalign did not align memory");

    r = posix_memalign(&buf, 256, 32768U);
    ASSERT_IsTrue(r == ENOMEM, "posix_memalign did not return ENOMEM when no memory is available");
    ASSERT_IsTrue(buf == NULL, "posix_memalign did not set buf to NULL when no memory is available");
}
ENDTEST
