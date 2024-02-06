/* Tests for predefined macros __LINE__ and __FILE__ as well as #line control */

#include <stdio.h>
#include <string.h>

static int failures = 0;

#if !defined __LINE__
#error __LINE__ is not predefined!
#endif

#if !defined __FILE__
#error __FILE__ is not predefined!
#endif

#define CONCAT(a,b)       CONCAT_impl_(a,b)
#define CONCAT_impl_(a,b) a##b
#define MKSTR(a)          MKSTR_impl_(a)
#define MKSTR_impl_(a)    #a
char CONCAT(ident,__LINE__)[0+__LINE__];
char CONCAT(ident,__LINE__)[0+__LINE__];

#define GET_FILE()        __FILE__
#define THIS_FILENAME_1   "bug1573.c"
#define THIS_FILENAME_2   "<<bar>>"
#define INC_FILENAME_1    "bug1573.h"
#define INC_FILENAME_2    "<<foo>>"

#line __LINE__ THIS_FILENAME_1      /* Note: #line sets the line number of the NEXT line */
void foo(void)
{
    if (strcmp (GET_FILE(), THIS_FILENAME_1) != 0) {
        printf("Expected: %s, got: %s\n", THIS_FILENAME_1, GET_FILE());
        ++failures;
    }
}

#line __LINE__ THIS_FILENAME_2      /* Note: #line sets the line number of the NEXT line */
#include INC_FILENAME_1
long line2 = __LINE__;

int main(void)
{
    if (strcmp (filename1, INC_FILENAME_1) != 0) {
        printf("Expected filename1: %s, got: %s\n", INC_FILENAME_1, filename1);
        ++failures;
    }

    if (strcmp (filename2, INC_FILENAME_2) != 0) {
        printf("Expected filename2: %s, got: %s\n", INC_FILENAME_2, filename2);
        ++failures;
    }

    foo();

#line 65535
    if (strcmp (GET_FILE(), THIS_FILENAME_2) != 0) {
        printf("Expected: %s, got: %s\n", THIS_FILENAME_2, GET_FILE());
        ++failures;
    }

    if (line1 != 5L) {
        printf("Expected line1: %ld, got: %ld\n", 5L, line1);
        ++failures;
    }

    if (line2 != 38L) {
        printf("Expected line2: %ld, got: %ld\n", 38L, line2);
        ++failures;
    }

    if (strcmp (ans1, ans2) != 0 || strcmp (ans1, "42") != 0) {
        ++failures;
        printf("Expected: 42, ans1: %s, ans2: %s\n", ans1, ans2);
    }

    if (failures != 0) {
        printf("Failures: %d\n", failures);
    }
    return failures;
}
