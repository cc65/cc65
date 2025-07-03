#include <string.h>
#include "unittest.h"

#define SHORT_STR "abcdefghijklmnopqrstuvwxyz"

#define MID_STR_LEN 700 /* Two pages and something */
TEST
{
    char *src;
    int i;

    /* Long enough for the whole string */
    ASSERT_IsTrue(strlen("") == 0, "strlen(\"\") != 0");
    ASSERT_IsTrue(strlen(SHORT_STR) == 26, "strlen(\""SHORT_STR"\") != 26");

    src = malloc(MID_STR_LEN+1);
    ASSERT_IsTrue(src != NULL, "Could not allocate source string");
    memset(src, 'a', MID_STR_LEN-1);
    src[MID_STR_LEN] = '\0';
    ASSERT_IsTrue(strlen(src) == MID_STR_LEN, "strlen(\"700 chars\") != 700");
}
ENDTEST
