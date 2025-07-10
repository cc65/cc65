#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int fails = 0;

#define STRSTR_TEST(needle,expected)                     \
  if (strstr(haystack, (needle)) != (expected)) {        \
    printf("strstr failure: expected %p for \"%s\", "    \
           "got %p\n",                                   \
           expected, needle, strstr(haystack, (needle)));\
    fails++;                                             \
  }

#define STRCASESTR_TEST(needle,expected)                     \
  if (strcasestr(haystack, (needle)) != (expected)) {        \
    printf("strcasestr failure: expected %p for \"%s\", "    \
           "got %p\n",                                       \
           expected, needle, strcasestr(haystack, (needle)));\
    fails++;                                                 \
  }

int main (void)
{
    const char *haystack = "This is a string to search in";

    STRSTR_TEST("This is", haystack + 0);
    STRSTR_TEST("a string", haystack + 8);
    STRSTR_TEST("This is a string to search in", haystack);
    STRSTR_TEST("search in", haystack + 20);
    STRSTR_TEST("This is a string to search in with extra chars", NULL);
    STRSTR_TEST("nowhere", NULL);

    STRCASESTR_TEST("this is", haystack + 0);
    STRCASESTR_TEST("a STRING", haystack + 8);
    STRCASESTR_TEST("this is a string TO search in", haystack);
    STRCASESTR_TEST("This is a string to search in with extra chars", NULL);
    STRCASESTR_TEST("search IN", haystack + 20);

    return fails;
}
