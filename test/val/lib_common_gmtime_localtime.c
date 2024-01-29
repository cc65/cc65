#include <stdio.h>
#include <string.h>
#include <time.h>

int fails = 0;

typedef struct _test_data {
  time_t t;
  char *gmt;
  char *local;
} test_data;

/* Test data generated using glibc 2.37 */
test_data data[] = {
  /* First year */
  {0x00000000, "Thu Jan  1 00:00:00 1970\n", "Thu Jan  1 01:00:00 1970\n"},
  {0x004e7970, "Sun Mar  1 12:34:56 1970\n", "Sun Mar  1 13:34:56 1970\n"},
  {0x01e1337f, "Thu Dec 31 23:59:59 1970\n", "Fri Jan  1 00:59:59 1971\n"},

  /* First leap year */
  {0x03c26700, "Sat Jan  1 00:00:00 1972\n", "Sat Jan  1 01:00:00 1972\n"},
  {0x03c8fe7f, "Wed Jan  5 23:59:59 1972\n", "Thu Jan  6 00:59:59 1972\n"},
  {0x041180ff, "Tue Feb 29 23:59:59 1972\n", "Wed Mar  1 00:59:59 1972\n"},
  {0x04118100, "Wed Mar  1 00:00:00 1972\n", "Wed Mar  1 01:00:00 1972\n"},
  {0x05a4ebff, "Sun Dec 31 23:59:59 1972\n", "Mon Jan  1 00:59:59 1973\n"},

  /* A non-leap year */
  {0x63b0cd00, "Sun Jan  1 00:00:00 2023\n", "Sun Jan  1 01:00:00 2023\n"},
  {0x63fe957f, "Tue Feb 28 23:59:59 2023\n", "Wed Mar  1 00:59:59 2023\n"},
  {0x63fe9580, "Wed Mar  1 00:00:00 2023\n", "Wed Mar  1 01:00:00 2023\n"},
  {0x656d4ec0, "Mon Dec  4 04:00:00 2023\n", "Mon Dec  4 05:00:00 2023\n"},
  {0x6592007f, "Sun Dec 31 23:59:59 2023\n", "Mon Jan  1 00:59:59 2024\n"},

  /* Another leap year */
  {0x65920080, "Mon Jan  1 00:00:00 2024\n", "Mon Jan  1 01:00:00 2024\n"},
  {0x65e11a7f, "Thu Feb 29 23:59:59 2024\n", "Fri Mar  1 00:59:59 2024\n"},
  {0x65e11a80, "Fri Mar  1 00:00:00 2024\n", "Fri Mar  1 01:00:00 2024\n"},
  {0x6774857f, "Tue Dec 31 23:59:59 2024\n", "Wed Jan  1 00:59:59 2025\n"},

  /* End of century */
  {0xf48656ff, "Thu Dec 31 23:59:59 2099\n", "Fri Jan  1 00:59:59 2100\n"},

  /* A non-leap year for exceptional reasons */
  {0xf4865700, "Fri Jan  1 00:00:00 2100\n", "Fri Jan  1 01:00:00 2100\n"},
  {0xf4d41f7f, "Sun Feb 28 23:59:59 2100\n", "Mon Mar  1 00:59:59 2100\n"},
  {0xf4d41f80, "Mon Mar  1 00:00:00 2100\n", "Mon Mar  1 01:00:00 2100\n"},
  {0xf4fceff0, "Wed Mar 31 23:00:00 2100\n", "Thu Apr  1 00:00:00 2100\n"},
  {0xf6678a7f, "Fri Dec 31 23:59:59 2100\n", "Sat Jan  1 00:59:59 2101\n"},

  /* First post-2100 leap year */
  {0xfc0b2500, "Tue Jan  1 00:00:00 2104\n", "Tue Jan  1 01:00:00 2104\n"},
  {0xfc5a3eff, "Fri Feb 29 23:59:59 2104\n", "Sat Mar  1 00:59:59 2104\n"},
  {0xfc5a3f00, "Sat Mar  1 00:00:00 2104\n", "Sat Mar  1 01:00:00 2104\n"},
  {0xfcaa9c70, "Wed Apr 30 23:00:00 2104\n", "Thu May  1 00:00:00 2104\n"},

  /* End of epoch */
  {0xfdedaa00, "Thu Jan  1 00:00:00 2105\n", "Thu Jan  1 01:00:00 2105\n"},
  {0xffffffff, "Sun Feb  7 06:28:15 2106\n", "Thu Jan  1 00:59:59 1970\n"}
};

int main (void)
{
    int i;
    struct tm *tm;
    char *str;

    tm = gmtime(NULL);
    if (tm != NULL) {
      printf("gmtime should return NULL with a NULL parameter\n");
      fails++;
    }

    tm = localtime(NULL);
    if (tm != NULL) {
      printf("localtime should return NULL with a NULL parameter\n");
      fails++;
    }

    /* Verify conversion both ways */
    for (i = 0; ; i++) {
      time_t t = data[i].t;

      tm = gmtime(&t);
      str = asctime(tm);
      if (strcmp(data[i].gmt, str)) {
        printf("0x%lx: gmtime: unexpected result: expected %s, got %s\n", t, data[i].gmt, str);
        fails++;
      }
      
      _tz.timezone = 0;
      tm = localtime(&t);
      str = asctime(tm);
      if (strcmp(data[i].gmt, str)) {
        printf("0x%lx: localtime (UTC+0): unexpected result: expected %s, got %s\n", t, data[i].gmt, str);
        fails++;
      }

      _tz.timezone = 3600;
      tm = localtime(&t);
      str = asctime(tm);
      if (strcmp(data[i].local, str)) {
        printf("0x%lx: localtime (UTC+1): unexpected result: expected %s, got %s\n", t, data[i].local, str);
        fails++;
      }

      if (t == 0xFFFFFFFF)
        break;
    }
    return fails;
}
