;
; extzp.inc for the PC-Engine
;
; Groepaz/Hitmen, 2015-11-19
;
; Assembler include file that imports the runtime zero page locations used
; by the PC-Engine runtime, ready for usage in asm code.
;


        .global         CURS_X: zp
        .global         CURS_Y: zp
        .global         SCREEN_PTR: zp
        .global         CHARCOLOR: zp
        .global         RVS: zp
        .global         BGCOLOR: zp
        .global         tickcount: zp
        .global         vdc_flags: zp
