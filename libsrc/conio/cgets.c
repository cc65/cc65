/*  Created: 2025-06-15 Russell-S-Harper
** Modified: <iso-date> <author>
**    Notes: <e.g. revisions made to support target, edge cases, bugs, etc.>
**
** char* __fastcall__ cgets (char *buffer, int size);
*/

#include <stddef.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>

#ifndef CRLF
#define CRLF "\r\n"
#endif /* CRLF */

char* __fastcall__ cgets (char *buffer, int size)
/* Get a string of characters directly from the console. The function returns
** when size - 1 characters or either CR/LF are read. Note the parameters are
** more aligned with stdio fgets() as opposed to the quirky "standard" conio
** cgets(). Besides providing saner parameters, the function also echoes CRLF
** when either CR/LF are read but does NOT append either in the buffer. This is
** to correspond to stdio fgets() which echoes CRLF, but prevents a "gotcha"
** where the buffer might not be able to accommodate both CR and LF at the end.
**
**   param: buffer - where to save the input, must be non-NULL
**   param: size - size of the buffer, must be > 1
**  return: buffer if successful, NULL on error
*/
{
    int i = 0;
    unsigned char w, x, y;
    char c;

    if (buffer && size > 1) {
        /* Just need the width */
        screensize (&w, &y);
        /* Actually just the last column! */
        --w;
        cursor (1);
        for (buffer[i] = '\0', --size; i < size; ) {
            c = cgetc ();
            /* Handle CR/LF */
            if (strchr (CRLF, c)) {
                /* Echo CRLF, but don't append either CR/LF */
                cputs (CRLF);
                break;
            }
            /* Handle backspace */
            if (c == '\b') {
                if (i > 0) {
                    /* Remove the character */
                    buffer[--i] = '\0';
                    /* Logic to account for line wrapping */
                    y = wherey ();
                    x = wherex ();
                    y = x? y: y - 1;
                    x = x? x - 1: w;
                    /* Clear the character */
                    gotoxy (x, y);
                    cputc (' ');
                    gotoxy (x, y);
                }
            /* Handle regular characters */
            } else if (isprint (c)) {
                cputc (c);
                buffer[i] = c;
                buffer[++i] = '\0';
            }
        }
        cursor (0);
    }

    /* Done */
    return (i > 0)? buffer: NULL;
}
