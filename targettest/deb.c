/*
** test program for the debugger
**
** press 'd' to enter the debugger
**
*/

#include <stdio.h>
#include <conio.h>
#include <6502.h>
#include <dbg.h>

int main(void)
{
  char c;

  /* Initialize the debugger */
  DbgInit (0);

  clrscr();
  cputsxy(4,10,"Debugger test...."); cgetc();
  while(1) {
    printf("press d to debug, q to exit....\n");
    c = cgetc();
    if (c == 'q') {
      printf("exiting....\n");
      return(0);
    }
    if (c == 'd') {
      printf("entering debug...\n");
      BREAK();
      printf("return from debug...\n");
    }
    else {
      printf("unknown key '%c'\n",c);
    }
  }
}
