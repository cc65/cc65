/*
** Program-chaining function for Commodore platforms.
**
** 2013-08-24, Greg King
**
** This function exploits the program-chaining feature in CBM BASIC's ROM.
** It puts the desired program's name and unit number into a LOAD statement.
** Then, it points BASIC to that statement, so that the ROM will run that
** statement after this program quits.  The ROM will load the next program,
** and will execute it (because the LOAD will be seen in a running program).
*/

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <device.h>
#if   defined(__CBM610__)
#  include <cbm610.h>
#elif defined(__CBM510__)
#  include <cbm510.h>
#endif


#pragma data-name(push, "LOWCODE")
static struct line {
    const char end_of_line;
    const struct line *const next;
    const unsigned line_num;
    const char load_token, quotes[2], add_token, quote;
    char name[21];
    const char comma;
    char unit[3];
} basic = {
    '\0', &basic + 1,           /* high byte must be non-zero */
    0, 0x93,

    /* This string operation copies the name to high BASIC RAM.
    ** So, it won't be overwritten when the next program is loaded.
    */
    "\"\"", 0xaa, '\"',
    "\"                    ",   /* format: "123:1234567890123456\"" */
    ',', "01"
};
#pragma data-name(pop)

/* These values are platform-specific. */
extern const struct line *txtptr;
#pragma zpsym("txtptr")
extern char basbuf[];           /* BASIC's input buffer */
extern void basbuf_len[];
#pragma zpsym("basbuf_len")


int __fastcall__ exec (const char* progname, const char* cmdline)
{
    static int fd;
    static unsigned char dv, n = 0;

    /* Exclude devices that can't load files. */
    dv = getcurrentdevice ();
    if (dv < 8 && dv != 1 || dv > 30) {
        return _mappederrno (9);        /* illegal device number */
    }
    utoa (dv, basic.unit, 10);

    /* Don't try to run a program that can't be found. */
    fd = open (progname, O_RDONLY);
    if (fd < 0) {
        return fd;
    }
    close (fd);

    do {
        if ((basic.name[n] = progname[n]) == '\0') {
            break;
        }
    } while (++n < 20);         /* truncate long names */
    basic.name[n] = '\"';

    /* Build the next program's argument list. */
    basbuf[0] = 0x8f;           /* REM token */
    basbuf[1] = '\0';
    if (cmdline != NULL) {
        strncat (basbuf, cmdline, (size_t)basbuf_len - 2);
    }

#if defined(__CBM510__) || defined(__CBM610__)
    pokewsys ((unsigned)&txtptr, (unsigned)&basic);
#else
    txtptr = &basic;
#endif

    /* (The return code, in ST, will be destroyed by LOAD.
    ** So, don't bother to set it here.)
    */
    exit (__AX__);
}
