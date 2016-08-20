/*
  !!DESCRIPTION!! Testing empty bodied switch statements.
  !!ORIGIN!!      
  !!LICENCE!!     GPL, read COPYING.GPL
*/

#include <stdio.h>

unsigned char success=0;
unsigned char failures=0;
unsigned char dummy=0;

void done()
{
  dummy++;
}

void switch_no_body(void)
{
  switch(0);
}

void switch_empty_body(void)
{
  switch(0) {}
}

/* only worried about this file compiling successfully */
int main(void)
{
  switch_no_body();
  switch_empty_body();

  success=failures;
  done();
  printf("failures: %d\n",failures);

  return failures;
}
