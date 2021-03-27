
/* Issue #1438 fix #1439 - crash in cc65, related to delayed post-counting 

  this is an odd issue, the compile would crash *sometimes*, perhaps in one
  of ten compilation runs.
*/

/* #define __fastcall__ */

unsigned short a[10] = {0,1,2,3,4,5,6,7,8,9};

unsigned short __fastcall__ func2(void)
{
    return 42;
}

void func1(unsigned short *wp)
{
    *wp++ = func2();
}

int main(void)
{
    func1(&a[3]);
    if (a[2] != 2) {
        return 1;
    }
    if (a[3] != 42) {
        return 1;
    }
    if (a[4] != 4) {
        return 1;
    }
    return 0;
}
