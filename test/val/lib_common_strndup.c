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

    dst = strndup("", 0);
    ASSERT_IsTrue(dst != NULL, "strndup returned NULL")
    ASSERT_IsTrue(!strcmp(dst, ""), "strings differ");
    free(dst);

    for (i = 0; i < 30; i+=10) {
      dst = strndup(SHORT_STR, i);
      ASSERT_IsTrue(dst != NULL, "strndup returned NULL");
      printf("strlen %s = %d (%d expected)\n", dst, strlen(dst), i);
      ASSERT_IsTrue(strlen(dst) == i, "string lengths differ");
      ASSERT_IsTrue(!strncmp(dst, SHORT_STR, i), "strings differ");
      free(dst);
    }

    dst = strndup(SHORT_STR, 50);
    ASSERT_IsTrue(dst != NULL, "strndup returned NULL");
    printf("strlen %s = %d (%d expected)\n", dst, strlen(dst), i);
    ASSERT_IsTrue(strlen(dst) == 26, "string lengths differ");
    ASSERT_IsTrue(!strcmp(dst, SHORT_STR), "strings differ");
    free(dst);


    src = malloc(MID_STR_LEN+1);
    ASSERT_IsTrue(src != NULL, "Could not allocate source string");
    memset(src, 'a', MID_STR_LEN);
    src[MID_STR_LEN] = '\0';

    for (i = 0; i < MID_STR_LEN -1; i+=10) {
      dst = strndup(src, i);
      ASSERT_IsTrue(dst != NULL, "strndup returned NULL");
      printf("strlen %s = %d (%d expected)\n", dst, strlen(dst), i);
      ASSERT_IsTrue(strlen(dst) == i, "string lengths differ");
      ASSERT_IsTrue(!strncmp(dst, src, i), "strings differ");
      free(dst);
    }

    for (i = MID_STR_LEN; i < MID_STR_LEN * 2; i+=10) {
      dst = strndup(src, i);
      ASSERT_IsTrue(dst != NULL, "strndup returned NULL");
      printf("%d, strlen %s = %d (%d expected)\n", i, dst, strlen(dst), strlen(src));
      ASSERT_IsTrue(strlen(dst) == strlen(src), "string lengths differ");
      ASSERT_IsTrue(!strcmp(dst, src), "strings differ");
      free(dst);
    }

}
ENDTEST
