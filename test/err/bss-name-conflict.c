/*
  !!DESCRIPTION!! conflicting bss-name pragmas
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Piotr Fusik
*/

/*
  see: https://github.com/cc65/cc65/issues/409
*/

char oam_off;
#pragma bss-name (push,"ZEROPAGE")
char oam_off;
#pragma bss-name (pop)

int main(void)
{
    return 0;
}
