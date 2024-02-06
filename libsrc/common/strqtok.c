/*
** strqtok() is like strtok():  It finds pieces of text, in a string, that are
** surrounded by given delimiter characters.  It returns each piece, in turn,
** as a string, until every piece has been found.  Then, it returns NULL.  But,
** strqtok() recognizes quotation marks.  A mark makes delimiters look ordinary
** until another quotation mark is seen.  That allows us to include delimiters
** in tokens.  (This version doesn't allow escaped quotation marks.)
**
** 2014-04-19, Daniel Serpell
** 2014-04-21, Paul Foerster
** 2014-04-25, Greg King
*/


#include <string.h>


char* __fastcall__ strqtok (register char* s1, const char* s2)
{
    static char  c;
    static char* start;
    static char* next = "";

    if (s1 == NULL) {
        s1 = next;
        if (c == '\"') {
            goto inQuote;
        }
    }

    /* Search for the start of a token. */
    while (strchr (s2, c = *s1)) {
        if (c == '\0') {
            /* No more tokens. */
            return NULL;
        }
        ++s1;
    }
    if (c == '\"') {
        goto skipQuote;
    }

    /* Save the start of the token. */
    start = s1;

    /* Search for the end of a non-quoted token. */
    while ((c = *s1) != '\"' && !strchr (s2, c)) {
        ++s1;
    }
    if (c == '\0') {
        /* The end of the last token is the end of the token list;
        ** don't go beyond it.
        */
        goto found;
    }

    /* (A possible begin-quote mark will be rememberred.) */
    goto terminate;

  skipQuote:
    ++s1;

  inQuote:
    /* Don't let a quote mark be rememberred. */
    c = '\0';

    /* Save the start of the token. */
    start = s1;

    /* Search for the end of a quoted token. */
    if ((s1 = strchr (s1, '\"')) == NULL) {
        /* The quoted token ended with '\0'; therefore, point to a '\0',
        ** so that the next call will return NULL.
        */
        next = "";
        return start;
    }

  terminate:
    *s1 = '\0';
    ++s1;

  found:
    next = s1;
    return start;
}
