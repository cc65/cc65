/*
** testprogram for _sys() function on Atari
**
** 17-Sep-2013, chris@groessler.org
**
** uses PUTCHR IOCB function to display a string
*/

#include <atari.h>
#include <6502.h>
#include <conio.h>

static struct regs regs;
static struct __iocb *iocb = &IOCB;  /* use IOCB #0 */

static char message[] = "I'm the sys test text\n";

int main(void)
{
    /* setup IOCB for CIO call */
    iocb->buffer = message;
    iocb->buflen = sizeof(message) - 1;
    iocb->command = IOCB_PUTCHR;

    /* setup input registers */
    regs.x = 0;         /* IOCB #0 */
    regs.pc = 0xe456;   /* CIOV */

    /* call CIO */
    _sys(&regs);

    if (regs.y != 1)
        cprintf("CIO error 0x%02\r\n", regs.y);

    cgetc();
    return 0;
}
