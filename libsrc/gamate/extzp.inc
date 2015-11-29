;
; extzp.inc for the Gamate
;
; Groepaz/Hitmen, 2015-11-27
;
; Assembler include file that imports the runtime zero page locations used
; by the Gamate runtime, ready for usage in asm code.
;

        .global         CURS_X: zp
        .global         CURS_Y: zp
        .global         CHARCOLOR: zp
        .global         RVS: zp
        .global         BGCOLOR: zp
        .global         tickcount: zp

