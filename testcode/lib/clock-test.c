/* Calendar-clock test program
**
** 2018-Sep-25, chris@groessler.org
** 2019-Dec-30, Greg King
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifdef __CC65__
 #include <conio.h>
 #include <cc65.h>
#endif

static int print_time(void)
{
    struct tm *cur_tm;
    time_t cur_time = time(NULL);

    if (cur_time == -1) {
        printf("time() failed: %s\n", strerror(errno));
        return 1;
    }
    cur_tm = localtime(&cur_time);

    printf("time: %s\n", asctime(cur_tm));
    // DEBUG:
    printf("year=%d, mon=%d, mday=%d\nhour=%d, min=%d, sec=%d\n",
           cur_tm->tm_year, cur_tm->tm_mon, cur_tm->tm_mday,
           cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);
    return 0;
}

int main(int argc, char **argv)
{
    char c;
    int s;
    struct tm cur_time;
    struct timespec new_time;

#ifdef __CC65__
    /* If DOS automatically will clear the screen after the program exits,
    ** then wait for a key-press.
    */
    if (doesclrscrafterexit())
        atexit((void (*)(void))cgetc);
#endif

    if (argc == 1) {
        return print_time();
    }

    if (argc != 2) {
#if defined(__APPLE2__)
        printf("USAGE: CALL2051 [:REM YY-MM-DD-HH-MM-SS]\n");
#elif defined(__ATMOS__) || defined(__CBM__)
        printf("Usage: run [:rem YY-MM-DD-HH-MM-SS]\n");
#else
        printf("Usage: %s [YY-MM-DD-HH-MM-SS]\n", argv[0]);
#endif
        return 1;
    }

    memset(&cur_time, 0, sizeof cur_time);
    s = sscanf(argv[1], "%d-%d-%d-%d-%d-%d",
               &cur_time.tm_year, &cur_time.tm_mon, &cur_time.tm_mday,
               &cur_time.tm_hour, &cur_time.tm_min, &cur_time.tm_sec);
    if (s != 6 || cur_time.tm_year > 99 /* other input values aren't being verified */) {
        printf("Invalid date-time format\n");
        return 1;
    }
    cur_time.tm_year += 100;            /* assume 21st century */
    --cur_time.tm_mon;

    memset(&new_time, 0, sizeof new_time);
    new_time.tv_sec = mktime(&cur_time);

    printf("\nYou are about to set the time to\n-->  %s\nContinue (y/n)? ", ctime(&new_time.tv_sec));
    do {
#ifdef __CC65__
        c = cgetc();
#else
        c = getchar();
#endif
    } while (c != 'y' && c != 'Y' && c != 'n' && c != 'N');
    printf("%c\n", c);

    if (c == 'n' || c == 'N') {
        printf("User abort\n");
        return 0;
    }

    s = clock_settime(CLOCK_REALTIME, &new_time);
    if (s) {
        printf("clock_settime() failed: %s\n", strerror(errno));
        return 1;
    }
    printf("Time set!\n\n");

    //DEBUG test begin
    return print_time();
    //DEBUG test end
}
/* Local Variables: */
/* c-file-style: "cpg" */
/* c-basic-offset: 4 */
/* End: */
