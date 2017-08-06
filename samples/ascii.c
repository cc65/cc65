/* ascii.c
**
** Shows the ASCII (or ATASCII, PETSCII) codes of typed characters.
**
** 2003-03-09, Greg King <gngking@erols.com>
*/

/* Define USE_STDIO, when you want to use the stdio functions.
** Do not define it, when you want to use the conio functions.
** NOTE:  stdin on some targets is line-bufferred.  You might need to type
**        a key, then tap the return(enter)-key, in order to see each code.
*/
/*
#define USE_STDIO
*/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define QUIT    'Q'

/* r -- row.  t -- table-column.
*/
static unsigned char height, width, r, t;
static int c;

#ifndef USE_STDIO

# define PRINT  cprintf
# define PUT(c) cputc((char)(c))

/* conio doesn't echo typed characters.
** So, this function does it.
*/
static int GET(void) {
        PUT(c = (int)cgetc());
        return c;
        }

#else

# define PRINT  printf
# define GET    getchar

#endif

int main(void) {

#       ifndef USE_STDIO
        /* conio doesn't scroll!  Avoid trouble by starting at the top
        ** of the screen, and never going "below" the bottom of the screen.
        */
        clrscr();
        r = 7;                          /* allow for prompt */
#       endif

        /* This prompt fits on the VIC-20's narrow screen.
        */
        PRINT("Type characters to see\r\ntheir hexadecimal code\r\nnumbers - 'Q' quits:\r\n\n");
        screensize(&width, &height);    /* get the screen's dimensions */
        width /= 6;                     /* get number of codes on a line */
        cursor(true);
        t = 0;
        while ((c = GET()) != EOF) {

#               ifndef USE_STDIO
                if (r == height) {
                        clrscr();
                        t = 0;
                        PUT(c); /* echo char. again because screen was erased */
                        r = 1;
                        }
                if (c == '\n')
                        ++r;
#               endif

                PRINT("=$%02x ", c);
                if (c == QUIT)
                        break;
                if (++t == width) {
                        PRINT("\r\n");
                        ++r;
                        t = 0;
                        }
                }
        PRINT("\r\n");
        return EXIT_SUCCESS;
        }
