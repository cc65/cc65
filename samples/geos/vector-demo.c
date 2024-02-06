#include <geos.h>
#include <conio.h>
#include <stdlib.h>

unsigned char x,y;

void_func oldMouseVector, oldKeyVector;

void foo1 (void)
{
    // do something on mouse press/release
    gotoxy(x,y);
    ++x;
    cputc('A');

    // call previous routine
    oldMouseVector();
}

void foo2 (void)
{
    // do something on key press/release
    gotoxy(x,y);
    ++y;
    cputc('B');

    // call previous routine
    oldKeyVector();
}

void hook_into_system(void)
{
    // hook into system vectors - preserve old value
    oldMouseVector = mouseVector;
    mouseVector = foo1;
    oldKeyVector = keyVector;
    keyVector = foo2;
}

void remove_hooks(void)
{
    mouseVector = oldMouseVector;
    keyVector = oldKeyVector;
}

int main(void)
{
    x = 0;
    y = 0;

    // To make cc65 do something for you before exiting you might register
    // a function to be called using atexit call. #include <stdlib.h> then and
    // write:
    atexit(&remove_hooks);

    clrscr();
    cputsxy(0,1, CBOLDON "Just" COUTLINEON  "a " CITALICON "string." CPLAINTEXT );

    hook_into_system();

    // This program will loop forever though
    MainLoop();

    // If not using atexit() you have to remember about restoring system vectors
    // right before exiting your application. Otherwise the system will most
    // likely crash.
    // remove_hooks();

    return 0;
}
