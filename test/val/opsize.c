
/* Test for result types of certain unary operations */

#include <stdio.h>

signed char x;
struct S {
    unsigned char a : 3;
    unsigned int  b : 3;
} s;

int main(void)
{
    _Static_assert(sizeof (++x) == sizeof (char), "++x result should not have promoted type");
    _Static_assert(sizeof (--x) == sizeof (char), "--x result should not have promoted type");
    _Static_assert(sizeof (x++) == sizeof (char), "x++ result should not have promoted type");
    _Static_assert(sizeof (x--) == sizeof (char), "x-- result should not have promoted type");
    _Static_assert(sizeof (x=0) == sizeof (char), "x=0 result should not have promoted type");

    _Static_assert(sizeof (+x) == sizeof (int), "+x result should have promoted type");
    _Static_assert(sizeof (-x) == sizeof (int), "-x result should have promoted type");
    _Static_assert(sizeof (~x) == sizeof (int), "~x result should have promoted type");

    _Static_assert(sizeof (+s.a) == sizeof (int), "+s.a result should have promoted type");
    _Static_assert(sizeof (-s.a) == sizeof (int), "-s.a result should have promoted type");
    _Static_assert(sizeof (~s.a) == sizeof (int), "~s.a result should have promoted type");

    _Static_assert(sizeof (+s.b) == sizeof (int), "+s.b result should have promoted type");
    _Static_assert(sizeof (-s.b) == sizeof (int), "-s.b result should have promoted type");
    _Static_assert(sizeof (~s.b) == sizeof (int), "~s.b result should have promoted type");

    return 0;
}
