#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __ATARI__
#include <atari.h>
#include <conio.h>
#endif

static int do_test(const char *s1, const char *s2, size_t n)
{
    printf("strnicmp(\"%s\", \"%s\", %d):  ", s1, s2, (int)n);
    return strncasecmp(s1, s2, n);
}

int main(void)
{
    int ret;

    ret = do_test("Wurzl", "wURZL", 5);
    if (ret)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

    ret = do_test("Wurzl", "wURZL", 6);
    if (ret)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

    ret = do_test("Wurzl", "wURZL", 10);
    if (ret)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

    ret = do_test("Wurzla", "wURZLB", 10);
    if (ret >= 0)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

    ret = do_test("Wurzla", "wURZLb", 5);
    if (ret)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

    ret = do_test("BLI", "bla", 5);
    if (ret <= 0)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

    ret = do_test("", "bla", 5);
    if (ret >= 0)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

    ret = do_test("BLI", "", 5);
    if (ret <= 0)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

    ret = do_test("", "", 5);
    if (ret)
        printf("fail (%d)\n", ret);
    else
        printf("OK (%d)\n", ret);

#ifdef __ATARI__
    if (_dos_type != 1) {
        cgetc();
    }
#endif

    return 0;
}
