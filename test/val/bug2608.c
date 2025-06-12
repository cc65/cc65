
/* bug #2608: "zp_bss" is placed in BSS and NOT placed in ZEROPAGE as expected. */

#include <stdlib.h>
#include <stdio.h>

int err = 0;

int is_zeropage(void *p)
{
    if (/*(p >= ((void*)0)) &&*/
        (p <= ((void*)0xff))) {
        return 1;
    }
    return 0;
}

void foo(void) {
#pragma bss-name(push,"ZEROPAGE")
#pragma data-name(push,"ZEROPAGE")
    static int zp_data = 5;
    static char zp_bss;
#pragma bss-name(pop)
#pragma data-name(pop)
    printf("zp_data at 0x%04x (%szp)\n", &zp_data, is_zeropage(&zp_data) ? "" : "NOT ");
    printf("zp_bss  at 0x%04x (%szp)\n", &zp_bss, is_zeropage(&zp_bss) ? "" : "NOT ");
    if (!is_zeropage(&zp_data)) {
        err++;
    }
    if (!is_zeropage(&zp_bss)) {
        err++;
    }
}

int main(void)
{
    foo();
    printf("errors: %d\n", err);
    return err;
}
