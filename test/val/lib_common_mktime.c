#include <stdio.h>
#include <string.h>
#include <time.h>

int fails = 0;

typedef struct _test_data {
  time_t t;
  struct tm tm;
  char *str;
} test_data;

/* Test data generated using glibc 2.37 */
test_data data[] = {
  /* First year */
  {0x00000000, {0, 0, 0, 1, 0, 70, 0, 4}, "Thu Jan  1 00:00:00 1970\n"},
  {0x004e7970, {56, 34, 12, 1, 2, 70, 59, 0}, "Sun Mar  1 12:34:56 1970\n"},
  {0x01e1337f, {59, 59, 23, 31, 11, 70, 364, 4}, "Thu Dec 31 23:59:59 1970\n"},

  /* First leap year */
  {0x03c26700, {0, 0, 0, 1, 0, 72, 0, 6}, "Sat Jan  1 00:00:00 1972\n"},
  {0x03c8fe7f, {59, 59, 23, 5, 0, 72, 4, 3}, "Wed Jan  5 23:59:59 1972\n"},
  {0x041180ff, {59, 59, 23, 29, 1, 72, 59, 2}, "Tue Feb 29 23:59:59 1972\n"},
  {0x04118100, {0, 0, 0, 1, 2, 72, 60, 3}, "Wed Mar  1 00:00:00 1972\n"},
  {0x05a4ebff, {59, 59, 23, 31, 11, 72, 365, 0}, "Sun Dec 31 23:59:59 1972\n"},

  /* A non-leap year */
  {0x63b0cd00, {0, 0, 0, 1, 0, 123, 0, 0}, "Sun Jan  1 00:00:00 2023\n"},
  {0x63fe957f, {59, 59, 23, 28, 1, 123, 58, 2}, "Tue Feb 28 23:59:59 2023\n"},
  {0x63fe9580, {0, 0, 0, 1, 2, 123, 59, 3}, "Wed Mar  1 00:00:00 2023\n"},
  {0x656d4ec0, {0, 0, 4, 4, 11, 123, 337, 1}, "Mon Dec  4 04:00:00 2023\n"},
  {0x6592007f, {59, 59, 23, 31, 11, 123, 364, 0}, "Sun Dec 31 23:59:59 2023\n"},

  /* Another leap year */
  {0x65920080, {0, 0, 0, 1, 0, 124, 0, 1}, "Mon Jan  1 00:00:00 2024\n"},
  {0x65e11a7f, {59, 59, 23, 29, 1, 124, 59, 4}, "Thu Feb 29 23:59:59 2024\n"},
  {0x65e11a80, {0, 0, 0, 1, 2, 124, 60, 5}, "Fri Mar  1 00:00:00 2024\n"},
  {0x6774857f, {59, 59, 23, 31, 11, 124, 365, 2}, "Tue Dec 31 23:59:59 2024\n"},

  /* End of century */
  {0xf48656ff, {59, 59, 23, 31, 11, 199, 364, 4}, "Thu Dec 31 23:59:59 2099\n"},

  /* A non-leap year for exceptional reasons */
  {0xf4865700, {0, 0, 0, 1, 0, 200, 0, 5}, "Fri Jan  1 00:00:00 2100\n"},
  {0xf4d41f7f, {59, 59, 23, 28, 1, 200, 58, 0}, "Sun Feb 28 23:59:59 2100\n"},
  {0xf4d41f80, {0, 0, 0, 1, 2, 200, 59, 1}, "Mon Mar  1 00:00:00 2100\n"},
  {0xf4fceff0, {0, 0, 23, 31, 2, 200, 89, 3}, "Wed Mar 31 23:00:00 2100\n"},
  {0xf6678a7f, {59, 59, 23, 31, 11, 200, 364, 5}, "Fri Dec 31 23:59:59 2100\n"},

  /* First post-2100 leap year */
  {0xfc0b2500, {0, 0, 0, 1, 0, 204, 0, 2}, "Tue Jan  1 00:00:00 2104\n"},
  {0xfc5a3eff, {59, 59, 23, 29, 1, 204, 59, 5}, "Fri Feb 29 23:59:59 2104\n"},
  {0xfc5a3f00, {0, 0, 0, 1, 2, 204, 60, 6}, "Sat Mar  1 00:00:00 2104\n"},
  {0xfcaa9c70, {0, 0, 23, 30, 3, 204, 120, 3}, "Wed Apr 30 23:00:00 2104\n"},

  /* End of epoch */
  {0xfdedaa00, {0, 0, 0, 1, 0, 205, 0, 4}, "Thu Jan  1 00:00:00 2105\n"},
  {0xffffffff, {15, 28, 6, 7, 1, 206, 37, 0}, "Sun Feb  7 06:28:15 2106\n"}
};

static int compare_tm(time_t t, struct tm *tm, struct tm *ref) {
  if (memcmp(tm, ref, sizeof(tm))) {
    printf("0x%lx: unexpected tm from gmtime: "
           "expected {%u, %u, %u, %u, %u, %u, %u, %u}, "
           "got {%u, %u, %u, %u, %u, %u, %u, %u}\n",
           t,
           ref->tm_sec, ref->tm_min, ref->tm_hour, ref->tm_mday, ref->tm_mon, ref->tm_year, ref->tm_yday, ref->tm_wday,
           tm->tm_sec, tm->tm_min, tm->tm_hour, tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_yday, tm->tm_wday);
    return 1;
  }
  return 0;
}

int main (void)
{
    int i;

    if (mktime(NULL) != (time_t)-1) {
      printf("mktime should return -1 with a NULL parameter\n");
      fails++;
    }

    /* Verify conversion both ways */
    for (i = 0; ; i++) {
      time_t t = data[i].t;
      time_t r;
      struct tm *tm = gmtime(&t);
      r = mktime(tm);

      if (t != r) {
        printf("unexpected timestamp from mktime: expected 0x%lx, got 0x%lx\n", t, r);
        fails++;
      }
      if (compare_tm(t, tm, &data[i].tm)) {
        fails++;
      }
      if (strcmp(data[i].str, ctime(&t))) {
        printf("0x%lx: unexpected ctime result: expected %s, got %s", t, data[i].str, ctime(&t));
        fails++;
      }

      if (t == 0xFFFFFFFF)
        break;
    }
    return fails;
}
