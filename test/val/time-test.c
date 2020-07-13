#include <stdio.h>
#include <time.h>



int main (void)
{
    struct tm tm;
    time_t t;
    char   buf[64];


    tm.tm_sec   = 9;
    tm.tm_min   = 34;
    tm.tm_hour  = 21;
    tm.tm_mday  = 12;
    tm.tm_mon   = 10;   /* 0..11, so this is november */
    tm.tm_year  = 102;  /* year - 1900, so this is 2002 */
    tm.tm_wday  = 2;    /* Tuesday */
    tm.tm_isdst = 0;

    /* Convert this broken down time into a time_t and back */
    t = mktime (&tm);
    printf ("Test passes if the following lines are\n"
            "all identical:\n");
    printf ("3DD173D1 - Tue Nov 12 21:34:09 2002\n");
    printf ("%08lX - %s", t, asctime (&tm));
    printf ("%08lX - %s", t, asctime (gmtime (&t)));
    strftime (buf, sizeof (buf), "%c", &tm);
    printf ("%08lX - %s\n", t, buf);
    strftime (buf, sizeof (buf), "%a %b %d %H:%M:%S %Y", &tm);
    printf ("%08lX - %s\n", t, buf);

    return 0;
}



