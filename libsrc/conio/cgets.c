/*  Created: 2025-06-15 Russell-S-Harper
** Modified: <iso-date> <author>
**    Notes: <e.g. revisions made to support target, edge cases, bugs, etc.>
**
** char *cgets(char *buffer);
*/

#include <stddef.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>

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
**   - terminating \0 is appended
**   - therefore the maximum number of characters which can be read is the size
**     of the buffer - 3!
**   - note: CR/LF are NOT echoed, typically a following call to cputs or
**     cprintf will need "\r\n" prepended - this is standard behavior!
**
**   param: buffer - where to save the input
**  return: buffer + 2 (i.e. start of the string) or NULL if buffer is NULL
*/
{
    /* Return buffer + 2 or NULL if buffer is NULL */
    char *result = buffer? buffer + CGETS_HDR_LEN: NULL;

    if (result) {
        /* Initialize just in case the caller didn't! */
        buffer[CGETS_READ] = 0;
        buffer[CGETS_DATA] = '\0';

        /* Call cgetsx to do the real work, ignore the result! */
        cgetsx (result, (unsigned char)buffer[CGETS_SIZE]);

        /* Set how many characters were read */
        buffer[CGETS_READ] = (unsigned char)strlen (result);
    }

    /* Done */
    return result;
}

static char *cgetsx (char *buffer, int size)
/* Like fgets but specifically for the console. Stops when CR/LF or size - 1
** characters are read. Will append a terminating \0, so at most size - 1
** characters can be read. Note that this function could be made public and
** have features added like cursor vs no-cursor, echo vs echo-pwd vs no-echo,
** or CR/LF handling to extend the functionality.
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
            /* Handle CR/LF */
            } else if (c == '\r' || c == '\n') {
                buffer[i] = '\0';
                break;
            }
        }
        cursor (0);
    }

    /* Done */
    return (i > 0)? buffer: NULL;
}
