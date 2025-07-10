#include <string.h>
#include "unittest.h"

#define SHORT_STR "abcdefghijklmnopqrstuvwxyz"

#define MID_STR_LEN 700 /* Two pages and something */
#define LONG_STR_LEN 40000UL /* Two long to duplicate */
TEST
{
    char *dst;
    char *src;
    int i;

    dst = strdup("");
    ASSERT_IsTrue(dst != NULL, "strdup returned NULL")
    ASSERT_IsTrue(!strcmp(dst, ""), "strings differ");
    free(dst);

    dst = strdup(SHORT_STR);
    ASSERT_IsTrue(dst != NULL, "strdup returned NULL");
    ASSERT_IsTrue(strlen(dst) == strlen(SHORT_STR), "string lengths differ");
    ASSERT_IsTrue(!strcmp(dst, SHORT_STR), "strings differ");
    free(dst);
    
    src = malloc(MID_STR_LEN+1);
    ASSERT_IsTrue(src != NULL, "Could not allocate source string");
    memset(src, 'a', MID_STR_LEN-1);
    src[MID_STR_LEN] = '\0';

    dst = strdup(src);
    ASSERT_IsTrue(dst != NULL, "strdup returned NULL");
    printf("strlens %zu %zu\n", strlen(src), strlen(dst));
    ASSERT_IsTrue(strlen(dst) == strlen(src), "string lengths differ");
    ASSERT_IsTrue(!strcmp(dst, src), "strings differ");
    free(dst);
    free(src);

    src = malloc(LONG_STR_LEN+1);
    ASSERT_IsTrue(src != NULL, "Could not allocate source string");
    memset(src, 'a', LONG_STR_LEN-1);
    src[LONG_STR_LEN] = '\0';

    dst = strdup(src);
    ASSERT_IsTrue(dst == NULL, "strdup did not return NULL");
    free(src);

    for (i = 254; i < 258; i++) {
      src = malloc(i+1);
      memset(src, 'a', i-1);
      src[i] = '\0';

      dst = strdup(src);
      ASSERT_IsTrue(dst != NULL, "strdup returned NULL");
      ASSERT_IsTrue(strlen(dst) == strlen(src), "string lengths differ");
      ASSERT_IsTrue(!strcmp(dst, src), "strings differ");
      free (dst);
      free(src);
    }
}
ENDTEST
