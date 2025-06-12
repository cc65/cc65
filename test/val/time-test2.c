/* Another test for time() */

#include <stdio.h>
#include <time.h>

static int failures = 0;
#define INV_TIME        ((time_t)-1)
#define TEST_TIME       ((time_t)0x78AB1234)

/* We supply our own clock_gettime function so we can control the values
** supplied to time() internally.
*/
static time_t timeval;
static int timeres;
int __fastcall__ clock_gettime (clockid_t, struct timespec *tp)
{
    /* Don't touch tp in case of an error */
    if (timeres != -1) {
        tp->tv_sec = timeval;
        tp->tv_nsec = 0;
    }
    return timeres;
}

int main()
{
    time_t res, pres;

    /* First test: Force time() to return an error. Check that both, the
    ** returned value and the value passed via pointer are (time_t)-1.
    */
    timeval = 42;
    timeres = -1;
    res = time(&pres);
    if (res != INV_TIME || pres != INV_TIME) {
        printf("Error in test 1\n");
        ++failures;
    }

    /* Second test: Return a valid value and check both results */
    timeval = TEST_TIME;
    timeres = 0;
    res = time(&pres);
    if (res != TEST_TIME || pres != TEST_TIME) {
        printf("Error in test 2\n");
        ++failures;
    }

    /* Third test: Return no error but an invalid value and check both
    ** results
    */
    timeval = INV_TIME;
    timeres = 0;
    res = time(&pres);
    if (res != INV_TIME || pres != INV_TIME) {
        printf("Error in test 3\n");
        ++failures;
    }

    return failures;
}
