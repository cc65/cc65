#include <string.h>
#include "unittest.h"

#define SHORT_STR "abcdefghijklmnopqrstuvwxyz"

#define MID_STR_LEN 700 /* Two pages and something */
TEST
{
    char *src;
    int i;

    /* Long enough for the whole string */
    ASSERT_IsTrue(strnlen("", 0) == 0, "strnlen(\"\", 0) != 0");
    ASSERT_IsTrue(strnlen("", 10) == 0, "strnlen(\"\", 10) != 0");
    ASSERT_IsTrue(strnlen(SHORT_STR, 0) == 0, "strnlen(\""SHORT_STR"\", 0) != 0");
    ASSERT_IsTrue(strnlen(SHORT_STR, 10) == 10, "strnlen(\""SHORT_STR"\", 10) != 10");
    ASSERT_IsTrue(strnlen(SHORT_STR, 26) == 26, "strnlen(\""SHORT_STR"\", 26) != 26");
    ASSERT_IsTrue(strnlen(SHORT_STR, 50) == 26, "strnlen(\""SHORT_STR"\", 50) != 26");

    src = malloc(MID_STR_LEN+1);
    ASSERT_IsTrue(src != NULL, "Could not allocate source string");
    memset(src, 'a', MID_STR_LEN-1);
    src[MID_STR_LEN] = '\0';
    ASSERT_IsTrue(strnlen(src, 0) == 0, "strnlen(src, 0) != 0");
    ASSERT_IsTrue(strnlen(src, 10) == 10, "strnlen(src, 10) != 10");
    ASSERT_IsTrue(strnlen(src, 260) == 260, "strnlen(src, 260) != 260");
    ASSERT_IsTrue(strnlen(src, MID_STR_LEN+1) == MID_STR_LEN, "strnlen(src, MID_STR_LEN+1) != MID_STR_LEN");
}
ENDTEST
