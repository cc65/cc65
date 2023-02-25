
// test if memset and bzero work as expected after optimizations

#include <string.h>

char s1[10] = { 1,2,3,4,5,6,7,8,9,10 };
char r1[10] = { 0,0,0,0,0,6,7,8,9,10 };

char s2[10] = { 1,2,3,4,5,6,7,8,9,10 };
char r2[10] = { 0,0,0,0,0,0,7,8,9,10 };

char s3[10] = { 1,2,3,4,5,6,7,8,9,10 };
char r3[10] = { 0,0,0,0,0,0,0,8,9,10 };

char *p1, *p2, *p3;

int res = 0;

int main(void)
{
    /* regular bzero */
    bzero(s1, 5);
    p1 = __AX__; /* this works because bzero jumps into memset */
    /* this gets converted to __bzero */
    p2 = memset(s2, 0, 6);
    /* call internal __bzero (we should not do this in real code) */
    p3 = __bzero(s3, 7);

    /* check the results */
    if (memcmp(s1, r1, 10) != 0) {
        res++;
    }
    if (memcmp(s2, r2, 10) != 0) {
        res++;
    }
    if (memcmp(s3, r3, 10) != 0) {
        res++;
    }

    if (p1 != s1) {
        res++;
    }
    if (p2 != s2) {
        res++;
    }
    if (p3 != s3) {
        res++;
    }

    return res;
}
