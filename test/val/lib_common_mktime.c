#include <stdio.h>
#include <string.h>
#include <time.h>

int fails = 0;

time_t timestamps[] = {
  0,
  0x2FFFFFFF,
  0x6FFFFFFF,
  0xF48656FF,
  0xF4865700,
  0xFC5A3EFF,
  0x6D6739FF,
  0x6D673A00,
  0xFFFFFFFF,
};

/* Values checked against glibc 2.37's implementation of ctime() */
const char *dates[] = {
  "Thu Jan  1 00:00:00 1970\n",
  "Sun Jul  9 16:12:47 1995\n",
  "Wed Jul 18 05:49:51 2029\n",
  "Thu Dec 31 23:59:59 2099\n",
  "Fri Jan  1 00:00:00 2100\n",
  "Fri Feb 29 23:59:59 2104\n",
  "Tue Feb 29 23:59:59 2028\n",
  "Wed Mar  1 00:00:00 2028\n",
  "Sun Feb  7 06:28:15 2106\n",
  NULL
};

int main (void)
{
    struct tm tm;
    time_t t;
    int i;

    /* Verify conversion both ways */
    for (t = 0x0FFFFFFF; ; t += 0x10000000) {
      struct tm *tm = gmtime(&t);
      time_t r = mktime(tm);
      if (t != r) {
        fails++;
        printf("Unexpected result for t %lx: %lx\n", t, r);
      }
      if (t == 0xFFFFFFFF) {
        break;
      }
    }

    for (i = 0; dates[i] != NULL; i++) {
      char *str = ctime(&timestamps[i]);
      if (strcmp(str, dates[i])) {
        fails++;
        printf("Unexpected result for t %lx: Expected \"%s\", got \"%s\"\n",
               timestamps[i], dates[i], str);
      }
    }
    return fails;
}
