;
; Maciej 'YTM/Elysium' Witkowiak
;
; 29.10.99, 05.3.2003

; void DrawPoint        (char mode, struct pixel *mypixel);

            .import PointRegs
            .import popa
            .importzp tmp1
            .export _DrawPoint

            .include "jumptab.inc"

_DrawPoint:
        jsr PointRegs
        jsr popa
        sta tmp1
        clc
        bit tmp1
        bvc @1
        sec
@1:     jmp DrawPoint
