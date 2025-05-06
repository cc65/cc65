/* Tests for predefined macro __COUNTER__ */

#include <stdio.h>

static int failures = 0;

#if __COUNTER__         /* 0 */
#    error __COUNTER__ should begin at 0!
#elif __COUNTER__ == 1  /* 1 */
#    define CONCAT(a,b)         CONCAT_impl_(a,b)
#    define CONCAT_impl_(a,b)   a##b
#endif

#line 42 "What is the answer?"
int CONCAT(ident,__COUNTER__)[0+__LINE__] = {__LINE__}, CONCAT(ident,__COUNTER__)[0+__LINE__] = {__LINE__}; /* 2,3 */

#if __COUNTER__ == 4 ? 1 || __COUNTER__ : 0 && __COUNTER__ /* 4,5,6 */
_Static_assert(__COUNTER__ == 7, "__COUNTER__ should be 7 here!"); /* 7 */
#    define GET_COUNTER()       __COUNTER__
#    define GET_LINE()          __LINE__
#    warning __COUNTER__ in #warning is just output as text and will never increase!
#else
#    if __COUNTER__ + __COUNTER__ + __COUNTER__ /* Skipped as a whole and not incrementing */
#    endif
#    error __COUNTER__ is skipped along with the whole #error line and will never increase anyways! */
#endif

#include "counter.h"
#include "counter.h"

_Static_assert(GET_COUNTER() == 10, "__COUNTER__ should be 10 here!"); /* 10 */

int main(void)
{
    if (ident2[0] != 42) {
        printf("Expected ident2[0]: %s, got: %s\n", 42, ident2[0]);
        ++failures;
    }

    if (ident3[0] != 42) {
        printf("Expected ident3[0]: %s, got: %s\n", 42, ident3[0]);
        ++failures;
    }

    if (ident8 != 8) {
        printf("Expected ident8: %s, got: %s\n", 8, ident8);
        ++failures;
    }

    if (ident9 != 9) {
        printf("Expected ident9: %s, got: %s\n", 9, ident9);
        ++failures;
    }

    if (failures != 0) {
        printf("Failures: %d\n", failures);
    }

    return failures;
}
