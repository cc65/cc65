#include <string.h>
#include "unittest.h"
                                                    
static char TestString[] = "01234567890123456789";  // two times the same string
static char Found[256];

TEST
{
    unsigned len;
    unsigned i;
    char*    p;

    len = strlen(TestString)/2; // test only one half of the string, to find last appearance

    /* Search for all characters in the string, including the terminator */
    for (i = 0; i < len; ++i)
    {
        /* Search for this char */
        p = strrchr (TestString, TestString[i]);
        ASSERT_AreEqual(i+len, p-TestString, "%u", "Unexpected location of character '%c' found!" COMMA TestString[i]);

        /* Mark the char as checked */
        Found[TestString[i]] = 1;
    }

    /* Search for all other characters and make sure they aren't found */
    for (i = 0; i < 256; ++i)
    {
        if (!Found[i])
        {
            p = strrchr (TestString, i);
            ASSERT_IsFalse(p, "Unexpected location of character '%c' found!" COMMA TestString[i]);
        }
    }
}
ENDTEST


