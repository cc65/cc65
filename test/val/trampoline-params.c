/*
  !!DESCRIPTION!! wrapped-call pragma w/ many params
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Lauri Kasanen
*/

#include <stdarg.h>

static unsigned char flag;

static void trampoline_set(void) {
        asm("ldy tmp4");
        asm("sty %v", flag);
        asm("jsr callptr4");
}

#pragma wrapped-call(push, trampoline_set, 4)
long adder(long in);
#pragma wrapped-call(pop)

long adder(long in) {

        return in + 7;
}

int main() {

        flag = 0;

        return adder(70436) == 70436 + 7 && flag == 4 ? 0 : 1;
}
