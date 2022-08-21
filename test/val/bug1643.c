/* bug #1643, macro expansion in #include */

#define MKSTR(a)        MKSTR_IMPL(a)
#define MKSTR_IMPL(a)   #a
#define BUG1643_H       bug1643.h

#include MKSTR(BUG1643_H)

int main(void)
{
    return BUG1643_RESULT;
}
