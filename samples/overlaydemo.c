/*
 * Minimalistic overlay demo program.
 *
 * 2009-10-02, Oliver Schmidt (ol.sc@web.de)
 *
 */



#include <stdio.h>
#include <conio.h>
#include <fcntl.h>
#include <unistd.h>


extern void _OVERLAY1_LOAD__, _OVERLAY1_SIZE__;
extern void _OVERLAY2_LOAD__, _OVERLAY2_SIZE__;
extern void _OVERLAY3_LOAD__, _OVERLAY3_SIZE__;


void log (char *msg)
{
    printf ("Log: %s\n", msg);
}


#pragma code-name (push, "OVERLAY1");

void foo (void)
{
    log ("Calling main from overlay 1");
}

#pragma code-name (pop);


#pragma code-name (push, "OVERLAY2");

void bar (void)
{
    log ("Calling main from overlay 2");
}

#pragma code-name (pop);


#pragma code-name (push, "OVERLAY3");

void foobar (void)
{
    log ("Calling main from overlay 3");
}

#pragma code-name(pop);


unsigned char loadfile (char *name, void *addr, void *size)
{
    int file = open (name, O_RDONLY);
    if (file == -1) {
        log ("Opening overlay file failed");
        return 0;
    }

    read (file, addr, (unsigned) size);
    close (file);
    return 1;
}


void main (void)
{
    log ("Calling overlay 1 from main");
    if (loadfile ("ovrldemo.1", &_OVERLAY1_LOAD__, &_OVERLAY1_SIZE__)) {
        foo ();
    }

    log ("Calling overlay 2 from main");
    if (loadfile ("ovrldemo.2", &_OVERLAY2_LOAD__, &_OVERLAY2_SIZE__)) {
        bar ();
    }

    log ("Calling overlay 3 from main");
    if (loadfile ("ovrldemo.3", &_OVERLAY3_LOAD__, &_OVERLAY3_SIZE__)) {
        foobar ();
    }

    cgetc ();
}
