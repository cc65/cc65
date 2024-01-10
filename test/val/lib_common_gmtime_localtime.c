#include <stdio.h>
#include <string.h>
#include <time.h>

int fails = 0;

time_t timestamps[] = {
  0,
  0x41eb00,
  0x1e7cb00,
  0x21c8700,
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
const char *dates_gmt[] = {
  "Thu Jan  1 00:00:00 1970\n",
  "Fri Feb 20 00:00:00 1970\n",
  "Wed Jan  6 00:00:00 1971\n",
  "Mon Feb 15 00:00:00 1971\n",
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

const char *dates_gmt_plus_one[] = {
  "Thu Jan  1 01:00:00 1970\n",
  "Fri Feb 20 01:00:00 1970\n",
  "Wed Jan  6 01:00:00 1971\n",
  "Mon Feb 15 01:00:00 1971\n",
  "Sun Jul  9 17:12:47 1995\n",
  "Wed Jul 18 06:49:51 2029\n",
  "Fri Jan  1 00:59:59 2100\n",
  "Fri Jan  1 01:00:00 2100\n",
  "Sat Mar  1 00:59:59 2104\n",
  "Wed Mar  1 00:59:59 2028\n",
  "Wed Mar  1 01:00:00 2028\n",
  "Thu Jan  1 00:59:59 1970\n",
  NULL
};

int main (void)
{
    int i;

    for (i = 0; dates_gmt[i] != NULL; i++) {
      struct tm *tm;
      char *str;

      /* Check gmtime */
      tm = gmtime(&timestamps[i]);
      str = asctime(tm);
      if (strcmp(str, dates_gmt[i])) {
        fails++;
        printf("gmtime: Unexpected result for t %lx: Expected \"%s\", got \"%s\"\n",
               timestamps[i], dates_gmt[i], str);
      }

      /* Check localtime with UTC timezone */
      _tz.timezone = 0;
      tm = localtime(&timestamps[i]);
      str = asctime(tm);
      if (strcmp(str, dates_gmt[i])) {
        fails++;
        printf("localtime: Unexpected result for t %lx: Expected \"%s\", got \"%s\"\n",
               timestamps[i], dates_gmt[i], str);
      }

      // /* Check localtime at UTC+1 */
      // _tz.timezone = 3600;
      // tm = localtime(&timestamps[i]);
      // str = asctime(tm);
      // if (strcmp(str, dates_gmt_plus_one[i])) {
      //   fails++;
      //   printf("localtime: Unexpected result for t %lx: Expected \"%s\", got \"%s\"\n",
      //          timestamps[i], dates_gmt_plus_one[i], str);
      // }
    }
    return fails;
}
