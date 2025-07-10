/*
  !!DESCRIPTION!! global non-static and static conflicts
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Greg King
*/

/*
  see: https://github.com/cc65/cc65/issues/191
       https://github.com/cc65/cc65/issues/2111
*/

static int n = 0;
extern int n;           /* extern is ignored, gives a warning but keeps previous static definiton */
static int n;           /* no error or warning, the previous static is still in effect */

int main(void)
{
    return n;
}
