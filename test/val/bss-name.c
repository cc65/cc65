/*
  !!DESCRIPTION!! bss-name pragma
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Piotr Fusik
*/

/*
  see: https://github.com/cc65/cc65/issues/409
*/

#pragma bss-name (push,"ZEROPAGE")

char zp_var;

#pragma bss-name (pop)

int main(void)
{
    return (unsigned) &zp_var < 0x100 ? 0 : 1;
}
