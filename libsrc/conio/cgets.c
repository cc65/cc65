/*  Created: 2025-06-15 Russell-S-Harper
** Modified: <iso-date> <author>
**    Notes: <e.g. revisions made to support target, edge cases, bugs, etc.>
**
** char *cgets(char *buffer);
*/

#include <stddef.h>
#include <string.h>
#include <conio.h>

#ifndef CRLF
#define CRLF    "\r\n"
#endif  /* CRLF */

enum {CGETS_SIZE, CGETS_READ, CGETS_DATA, CGETS_HDR_LEN = CGETS_DATA};

static char *cgetsx (char *buffer, int size);

char *cgets (char *buffer)
/* Get a string of characters directly from the console. The standard interface
** is quirky:
**
**   - set buffer[0] to the size of the buffer - 2, must be > 0
**   - call cgets
**   - buffer[1] will have the number of characters read
**   - the actual string starts at buffer + 2
**   - trailing CRLF are removed
**   - terminating \0 is appended
**   - therefore the maximum number of characters which can be read is the size
**     of the buffer - 3!
**
**   param: buffer - where to save the input
**  return: buffer + 2 (or start of the string) if successful, NULL otherwise
**     see: cgetsx for equivalent functionality but with a saner interface!
*/
{
    /* Default to NULL */
    char *result = NULL;

    if (buffer && buffer[CGETS_SIZE]) {
        /* Initialize just in case the caller didn't! */
        buffer[CGETS_READ] = 0;
        buffer[CGETS_DATA] = '\0';

        /* Call cgetsx to do the real work */
        result = cgetsx (buffer + CGETS_HDR_LEN, (unsigned char)buffer[CGETS_SIZE]);

        /* Trim trailing CRLF and set how many characters were read */
        if (result) {
            result[strcspn (result, CRLF)] = '\0';
            buffer[CGETS_READ] = (unsigned char)strlen (result);
        }
    }

    /* Done */
    return result;
}

static char *cgetsx (char *buffer, int size)
/* Like fgets but specifically for the console. Stops when CR/LF or size - 1
** characters are read. Will append a terminating \0, so at most size - 1
** characters can be read. Note that this function could be made public and
** have features like cursor vs no-cursor, and/or echo vs echo-pwd vs no-echo
** added to extend the functionality.
**
**   param: buffer - where to save the input
**   param: size - the size of buffer
**  return: buffer if successful, NULL otherwise
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
        for (i = 0, --size; i < size; ) {
            c = cgetc ();
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
                    /* Clear the character and the cursor */
                    gotoxy (x, y);
                    cputs ("  ");
                    gotoxy (x, y);
                }
            /* Handle CRLF */
            } else if (strchr (CRLF, c)) {
                /* Clear the cursor and advance to the next line */
                cputs (" " CRLF);
                buffer[i] = c;
                buffer[++i] = '\0';
                break;
            /* Handle regular characters */
            } else {
                cputc (c);
                buffer[i] = c;
                buffer[++i] = '\0';
            }
        }
        cursor (0);
    }

    return (i > 0)? buffer: NULL;
}
