
#include <geos.h>
#include <conio.h>
#include <stdlib.h>

unsigned char x,y;

void (*oldMouseVector)();
void (*oldKeyVector)();

void foo1 (void) {
    // do something on mouse press/release
    gotoxy(x,y);
    ++x;
    cputc('A');
    // call previous routine
    oldMouseVector();
}

void foo2 (void) {
    // do something on key press/release
    gotoxy(x,y);
    ++y;
    cputc('B');
    // call previous routine
    oldKeyVector();
}

void hook_into_system(void) {
    // hook into system vectors - preserve old value
    oldMouseVector = (void (*)())mouseVector;
    mouseVector = (int)foo1;
    oldKeyVector = (void (*)())keyVector;
    keyVector = (int)foo2;
}

void remove_hooks(void) {
    mouseVector = (int)oldMouseVector;
    keyVector = (int)oldKeyVector;
}

int main(void) {

    x = 0;
    y = 0;

/*
    To make cc65 do something for you before exiting you might register
    a function to be called using atexit call. #include <stdlib.h> then and
    write:
*/
    atexit(&remove_hooks);

    clrscr();
    cputsxy(0,1, CBOLDON "Just" COUTLINEON  "a " CITALICON "string." CPLAINTEXT );

    hook_into_system();

/* This program will loop forever though */

    MainLoop();

/*
    If not using atexit() you have to remember about restoring system vectors
    right before exiting your application. Otherwise the system will most
    likely crash.

    remove_hooks();
*/
    return 0;
}
