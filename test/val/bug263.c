
/* issue #263 - cc65 miscompiles w/ a static variable and -O */

#include <stdint.h>
#include <stdio.h>

int failures = 0;

void __fastcall__ set_vram_update(unsigned char *ptr)
{
    printf("set_vram_update: %04x\n", ptr);
    if (ptr != NULL) {
        failures++;
    }
}

unsigned char __fastcall__ ppu_wait_nmi(void)
{
    // we need to make sure somehow the akku is not zero before the break
    return 0x1234;
}

unsigned char ctrl, ret, i;

unsigned char gameloop (void)
{
    ctrl = 0;
    ret = 0;
    while(1) {
        if (ctrl & 1) {
            while (--i) {
                ppu_wait_nmi();
            }
            break;
        }
        ctrl = 1;
    }
    // This will pass garbage, not NULL.
    set_vram_update(NULL);
    return ret;
}

int main(void)
{
    gameloop();
    printf("failures: %d\n", failures);
    return failures;
}

