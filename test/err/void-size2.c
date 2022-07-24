/*
  !!DESCRIPTION!! Size of void cast
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Greg King
*/

unsigned test (void)
{
    return sizeof ((void)12345);
}
