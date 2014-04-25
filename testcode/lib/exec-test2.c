/*
** These programs test cc65's exec() program-chaining function.
** exec-test1 runs exec-test2 -- that tests the loading and starting of another
** program.  Then, exec-test2 runs arg-test -- that tests command-line argument
** passing.
**
** 2013-08-24, Greg King
*/

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <conio.h>

int main (void) {
    cprintf ("\nExec-test #2 -- launching arg-test...\r\n\n");

    exec ("arg-test", "arg1 arg2 \"\" arg4");

    cprintf ("\nFailed to find arg-test:\r\n %s.\r\n", _stroserror (_oserror));
    cgetc ();
    return _oserror;
}
