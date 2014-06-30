/*
** test for conio cursor() function
** CPG 2002
**
** the cursor should be in the same state as mentioned in the printfs
*/

#include <conio.h>

#if 1
#define NEWLINE cprintf("\r\n")
#define PRINTF  cprintf
#else
#include <stdio.h>
#define NEWLINE printf("\n")
#define PRINTF  printf
#endif

int main(void)
{
        char c;
        NEWLINE;

        cursor (1);
        PRINTF("enter key (cursor on): ");
        c = cgetc();
        NEWLINE;

        cursor (0);
        PRINTF("enter key (cursor off): ");
        c = cgetc();
        NEWLINE;

        PRINTF("enter key (cursor on): ");
        cursor (1);
        c = cgetc();
        NEWLINE;

        PRINTF("enter key (cursor off): ");
        cursor (0);
        c = cgetc();
        NEWLINE;

        PRINTF("hit any key to exit...");
        c = cgetc();
        NEWLINE;

        return(0);
}
