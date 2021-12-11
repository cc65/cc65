// cl65 -t apple2 -C apple2-hgr.cfg hgrtest.c werner.s

#include <tgi.h>
#include <conio.h>

#pragma code-name (push, "LOWCODE")

void say (const char* text)
{
    tgi_setcolor (TGI_COLOR_BLACK);
    tgi_outtextxy (41, 33, text);
}

#pragma code-name (pop)

void main (void)
{
    tgi_install (a2_hi_tgi);
    tgi_init ();
    cgetc ();

    say ("Hi Dude !");
    cgetc ();

    tgi_uninstall ();
}
