/* ascii.c
**
** Shows the ASCII (or ATASCII, PETSCII) codes of typed characters.
**
** 2002-12-25, Greg King <gngking@erols.com>
*/

/* Define CONIO when you want to use the conio functions.
** Undefine it when you want to use the stdio functions.
** NOTE: Undefining CONIO will currently not work on the CBMs!!!
*/
#define CONIO

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define QUIT	'Q'

static unsigned char height, width, r, t;
static int c;

#ifdef CONIO
# define PRINT	cprintf
# define PUT(c)	cputc((char)(c))

/* conio doesn't echo typed characters.
** So, this function does it.
*/
static int GET(void) {
	PUT(c = (int)cgetc());
	return c;
	}
#else
# define PRINT	printf
# define GET	getchar
#endif

int main(void) {

#	ifdef CONIO
	/* conio doesn't scroll!  Avoid trouble by starting at the top
	** of the screen, and never going "below" the bottom of the screen.
	*/
	clrscr();
	r = 7;		/* allow for prompt */
#	endif

	/* This prompt fits on the VIC-20's narrow screen.
	*/
	PRINT("Type characters to see\r\ntheir hexadecimal code\r\nnumbers:\r\n\n");
	screensize(&width, &height);	/* get the screen's dimensions */
	width /= 6;			/* get number of codes on a line */
       	cursor(true);
	t = 0;
	while ((c = GET()) != EOF) {

#		ifdef CONIO
		if (r == height) {
			clrscr();
			PUT(c);	/* echo char. again because screen was erased */
			r = 1;
			}
#		endif

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
