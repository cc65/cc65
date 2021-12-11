/*
  !!DESCRIPTION!! global duplicated with static variable
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Piotr Fusik
*/

/*
  see: https://github.com/cc65/cc65/issues/191
*/

int n = 0;
static int n = 0;           /* should give an error */

int main(void)
{
    return n;
}
