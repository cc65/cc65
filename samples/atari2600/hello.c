/*****************************************************************************/
/*                                                                           */
/* Atari VCS 2600 sample C program                                           */
/*                                                                           */
/* Florent Flament (contact@florentflament.com), 2017                        */
/*                                                                           */
/*****************************************************************************/

#include <atari2600.h>

// PAL Timings
// Roughly computed based on Stella Programmer's guide (Steve Wright)
// scanlines count per section.
#define VBLANK_TIM64 51 // 45 lines * 76 cycles/line / 64 cycles/tick
#define KERNAL_T1024 17 // 228 lines * 76 cycles/line / 1024 cycles/tick
#define OVERSCAN_TIM64 42 // 36 lines * 76 cycles/line / 64 cycles/tick

// Testing memory zones
const unsigned char rodata_v[] = "Hello!";
unsigned char data_v = 0x77;
unsigned char bss_v;

void main(void) {
    unsigned char color = 0x79; // Stack variable
    bss_v = 0x88; // Testing BSS variable

    for/*ever*/(;;) {
        // Vertical Sync signal
        TIA.vsync = 0x02;
        TIA.wsync = 0x00;
        TIA.wsync = 0x00;
        TIA.wsync = 0x00;
        TIA.vsync = 0x00;

        // Vertical Blank timer setting
        RIOT.tim64t = VBLANK_TIM64;

        // Doing frame computation during blank
        TIA.colubk = color++; // Update color

        // Wait for end of Vertical Blank
        while (RIOT.timint == 0) {}
        TIA.wsync = 0x00;
        TIA.vblank = 0x00; // Turn on beam

        // Display frame
        RIOT.t1024t = KERNAL_T1024;
        while (RIOT.timint == 0) {}
        TIA.wsync = 0x00;
        TIA.vblank = 0x02; // Turn off beam

        // Overscan
        RIOT.tim64t = OVERSCAN_TIM64;
        while (RIOT.timint == 0) {}
    }
}
