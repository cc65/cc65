;
; extzp.inc for the Atari 7800
;
; Karri Kaksonen, 2022-04-05
;
; Assembler include file that imports the runtime zero page locations used
; by the atari7800 runtime, ready for usage in asm code.
;

        .global         ptr7800: zp
        .global         ptrtmp: zp
        .global         cursorzone: zp
        .global         CURS_X: zp
        .global         CURS_Y: zp

