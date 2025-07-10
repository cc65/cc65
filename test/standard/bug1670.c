
/* #1670 - Standard headers contain non standard identifiers in C89/C99 mode */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if __CC65_STD__ != __CC65_STD_CC65__
/* implement our own clock_gettime, using a different signature than the POSIX one */
const char* clock_gettime(void)
{
    static char buf[32];
    struct tm *my_tm;
#if 0
    /* FIXME: this will not work in the simulator */
    time_t t = time(NULL);
#else
    time_t t = 0x12345678;
#endif
    my_tm = localtime(&t);
    printf("%2d:%2d:%2d\n", my_tm->tm_hour, my_tm->tm_min, my_tm->tm_sec);
    strftime(buf, sizeof(buf), "<%H:%M:%S>", my_tm);
    return buf;
}
#endif

int main(void)
{
#if __CC65_STD__ != __CC65_STD_CC65__
    printf("The time is %s\n", clock_gettime());
#endif
    return EXIT_SUCCESS;
}
