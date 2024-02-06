/*
  !!DESCRIPTION!! wrapped-call pragma w/ variadic function
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Lauri Kasanen
*/

#include <stdarg.h>

static unsigned char flag;

static void trampoline_set(void) {
        // The Y register is used for variadics - save and restore
        asm("sty tmp3");

        asm("ldy tmp4");
        asm("sty %v", flag);

        asm("ldy tmp3");
        asm("jsr callptr4");
}

#pragma wrapped-call(push, trampoline_set, 4)
unsigned adder(unsigned char num, ...);
#pragma wrapped-call(pop)

unsigned adder(unsigned char num, ...) {

        unsigned char i;
        unsigned sum = 0;
        va_list ap;
        va_start(ap, num);

        for (i = 0; i < num; i++) {
                sum += va_arg(ap, unsigned);
        }

        va_end(ap);

        return sum;
}

int main() {

        flag = 0;

        return adder(3, 0, 5, 500) == 505 && flag == 4 ? 0 : 1;
}
