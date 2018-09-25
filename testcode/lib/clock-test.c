/* Clock test program
 *
 * 25-Sep-2018, chris@groessler.org
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifdef __CC65__
#include <conio.h>
#include <cc65.h>
#endif /* #ifdef __CC65__ */

static void print_time(void)
{
    struct tm *cur_tm;
    time_t cur_time = time(NULL);
    if (cur_time == -1) {
        printf("time() failed: %s\n", strerror(errno));
        return;
    }
    cur_tm = localtime(&cur_time);

    printf("time: %s\n", asctime(cur_tm));
    // DEBUG:
    printf("mday=%d mon=%d year=%d\nhour=%d min=%d sec=%d\n", cur_tm->tm_mday, cur_tm->tm_mon, cur_tm->tm_year, cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);
}

int main(int argc, char **argv)
{
    char c = 0;
    int s;
    struct tm cur_time;
    struct timespec new_time;

#ifdef __CC65__
    /* if DOS will automatically clear the screen after the program exits, wait for a keypress... */
    if (doesclrscrafterexit())
        atexit((void (*)(void))cgetc);
#endif

    if (argc <= 1) {
        print_time();
        return 0;
    }

    if (argc != 3 || strcasecmp(*(argv + 1), "set")) {
        printf("usage: CLOCKTST [set DD-MM-YY-HH-MM-SS]\n");
        return 1;
    }

    memset(&cur_time, 0, sizeof(cur_time));
    s = sscanf(*(argv + 2), "%d-%d-%d-%d-%d-%d", &cur_time.tm_mday, &cur_time.tm_mon, &cur_time.tm_year, &cur_time.tm_hour, &cur_time.tm_min, &cur_time.tm_sec);
    if (s != 6 || cur_time.tm_year > 99 /* other input values aren't being verified... */) {
        printf("invalid time/date format\n");
        return 1;
    }
    --cur_time.tm_mon;
    if (cur_time.tm_year < 79)
        cur_time.tm_year += 100;  /* adjust century */

    memset(&new_time, 0, sizeof(new_time));
    new_time.tv_sec = mktime(&cur_time);

    printf("\nyou are about to set the time to\n-->  %s\n\nContinue (y/n)?", ctime(&new_time.tv_sec));

    while (c != 'y' && c != 'Y' && c != 'n' && c != 'N') {
#ifdef __CC65__
        c = cgetc();
#else
        c = getchar();
#endif
    }
    printf("%c\n", c);

    if (c == 'n' || c == 'N') {
        printf("user abort\n");
        return 0;
    }

    s = clock_settime(CLOCK_REALTIME, &new_time);
    if (s) {
        printf("clock_settime() failed: %s\n", strerror(errno));
        return 1;
    }
    printf("time set!\n");
    //DEBUG test begin
    print_time();
    //DEBUG test end
    return 0;
}
/* Local Variables: */
/* c-file-style: "cpg" */
/* c-basic-offset: 4 */
/* End: */
