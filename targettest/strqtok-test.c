/* strqtok-test.c
**
** 2014-04-21, Paul Foerster
** 2014-05-20, Greg King
**
** This program tests that strqtok() correctly will parse strings
** with quotation marks in them.  It should show this list of tokens
** from the test strings:
**
** >This<
** >  is only    <
** >a<
** >short<
** >quoting<
** >test ,  honoring  blanks, commas<
** >and<
** >(4)<
** >empty<
** ><
** ><
** ><
** ><
** >strings,   EOT  <
**
** It shouldn't show
**
** >Bogus token<
**
*/

#include <string.h>
#include <stdio.h>

void main (void)
{
    /* b[] and s[] are declared as automatic, not static, variables
    ** because strqtok() will change them.
    ** They must be defined together; and, b[] must be defined first
    ** (because they're copied onto the top-down stack).
    */
    char  b[] = "Bogus token ";
    char  s[] = "  This ,  \"  is only    \"a   short   "
        "quoting\"test ,  honoring  blanks"
        ", commas\", and (4) empty \"\"\"\"\"\"\"\" \"strings,   EOT  ";
    char* t = strqtok (s, " ,");

    while (t != NULL) {
        printf (">%s<\n", t);
        t = strqtok (NULL, " ,");
    }
}
