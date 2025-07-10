/* Tests for predefined macros __LINE__ and __FILE__ as well as #line control */

#line __LINE__ INC_FILENAME_1
#define GET_LINE() __LINE__
char filename1[] = GET_FILE();
long line1 = GET_LINE();

#line 42
const char ans1[] = MKSTR(GET_LINE());

#line 40 INC_FILENAME_2
char filename2[] = GET_FILE();
const char ans2[] = \
MKSTR(GET_LINE());
