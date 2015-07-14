
#include <conio.h>
#include <time.h>
#include <joystick.h>
#include <string.h>
#include <stdlib.h>

static int datavar = 10;

void main(void)
{
    int stackvar = 42;
    int i, j;
    clock_t clk;
    char *p;

    joy_install(&joy_static_stddrv);

    clrscr();

    cputs("hello world");
    cputsxy(0, 2, "colors:" );
    for (i = 0; i < 16; ++i) {
        textcolor(i);
        cputc('X');
    }
    textcolor(1);

    gotoxy(0,4);
    cprintf("datavar:  %02x\n\r", datavar);
    cprintf("stackvar: %02x\n\r", stackvar);

    j = joy_count();
    gotoxy(0,10);
    cprintf("Found %d Joysticks.", j);

    for (i = 0; i < 4; ++i) {
        gotoxy(0, 17 + i);
        p = malloc(16);
        memcpy(p, "01234567890abcdef", 16);
        cprintf("alloced at: %04p - %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", p,
            p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]
        );
    }

    for(;;)
    {
        gotoxy(13,4);
        cprintf("%02x", datavar);
        gotoxy(13,5);
        cprintf("%02x", stackvar);
        ++datavar; ++stackvar;

        gotoxy(0,8);
        clk = clock();
        cprintf("clock: %08lx", clk);

        for (i = 0; i < 4; ++i)
        {
            gotoxy(0, 12 + i);
            j = joy_read (i);
            cprintf ("pad %d: %02x %-6s%-6s%-6s%-6s%-6s%-6s",
                     i, j,
                     (j & joy_masks[JOY_UP])?    "  up  " : " ---- ",
                     (j & joy_masks[JOY_DOWN])?  " down " : " ---- ",
                     (j & joy_masks[JOY_LEFT])?  " left " : " ---- ",
                     (j & joy_masks[JOY_RIGHT])? "right " : " ---- ",
                     (j & joy_masks[JOY_FIRE])?  " fire " : " ---- ",
                     (j & joy_masks[JOY_FIRE2])? "fire2 " : " ---- ");
        }
    }
    for(;;);
}
