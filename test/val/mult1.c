/*
  !!DESCRIPTION!!
  !!ORIGIN!!      SDCC regression tests
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>
#include <limits.h>

#define TESTLIT 0x05

unsigned char success=0;
unsigned char failures=0;
unsigned char dummy=0;

signed char c1,c2,c3;
unsigned char uc1,uc2,uc3;

unsigned int ui1,ui2,ui3;
signed int i1,i2;

void done()
{
  dummy++;
}

void m1(void)
{
  c1 = c1*5;        /* char = char * lit */
  c2 = c1*c3;       /* char = char * char */

  uc1 = uc1*3;      /* uchar = uchar * lit */
  uc2 = uc1*uc3;    /* uchar = uchar * uchar */

  if(c2 != 25)
    failures++;

  if(uc2 != 36)
    failures++;
}

void m2(unsigned char uc)
{
  uc2 = uc1 * uc;

  if(uc2 != 0x20)
    failures++;
}

void m3(unsigned char uc)
{
  volatile unsigned char vuc;

  /* uchar = uchar * lit */
  /* testing literal multiply with same source and destination */
  vuc = uc;
  uc2 = 0;
  uc1 = vuc; uc1 = uc1*0; if( uc1 != 0 )              failures++;
  uc1 = vuc; uc1 = uc1*1; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*2; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*3; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*4; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*5; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*6; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*7; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*8; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*9; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*10; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*11; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*12; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*13; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*14; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*15; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*16; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*17; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*18; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*19; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*20; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*21; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*22; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*23; if( uc1 != (uc2+=TESTLIT) ) failures++;
  uc1 = vuc; uc1 = uc1*24; if( uc1 != (uc2+=TESTLIT) ) failures++;

  uc1 = vuc; uc1 = uc1*31; if( uc1 != ((31*TESTLIT) & 0xff) ) failures++;
  uc1 = vuc; uc1 = uc1*32; if( uc1 != ((32*TESTLIT) & 0xff) ) failures++;
  uc1 = vuc; uc1 = uc1*64; if( uc1 != ((64*TESTLIT) & 0xff) ) failures++;
  uc1 = vuc; uc1 = uc1*128;if( uc1 != ((128*TESTLIT)& 0xff) ) failures++;

  /* testing literal multiply with different source and destination */
  uc1 = vuc*1; if( uc1 != ((1*TESTLIT) & 0xff) ) failures++;
  uc1 = vuc*2; if( uc1 != ((2*TESTLIT) & 0xff) ) failures++;
  uc1 = vuc*4; if( uc1 != ((4*TESTLIT) & 0xff) ) failures++;
}

int  main(void)
{
  dummy = 0;

  c1 = 1;
  c3 = 5;

  uc1 = 2;
  uc3 = 6;

  m1();

  uc1 = 0x10;
  m2(2);

  ui1 = uc1*uc2;   /* uint = uchar * uchar */

  i1 = c1*c2;      /* int = char * char */

  ui3 = ui1*ui2;   /* uint = uint * unit */

  m3(TESTLIT);

  success = failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
