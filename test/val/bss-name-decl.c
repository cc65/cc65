/*
  !!DESCRIPTION!! bss-name pragma not affecting declarations
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Piotr Fusik
*/

/*
  see: https://github.com/cc65/cc65/issues/409
*/

#pragma bss-name (push,"ZEROPAGE")

char n; /* only a declaration because followed by definition */
char n = 1; /* not BSS */

#pragma bss-name (pop)

int main(void)
{
    return (unsigned) &n >= 0x100 ? 0 : 1;
}
