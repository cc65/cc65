;
; Maciej 'YTM/Elysium' Witkowiak
;
; 29.10.99, 5.03.2003

; void DrawLine         (char mode, struct window *mywindow);

            .import _InitDrawWindow
            .import popa
            .importzp tmp1
            .export _DrawLine

            .include "jumptab.inc"
            .include "geossym.inc"
            .include "geosmac.inc"

_DrawLine:
        tay
        PushW r2
        tya
        jsr _InitDrawWindow
        MoveW r2, r11
        PopW r2
        jsr popa
        sta tmp1
        clc
        bit tmp1
        bvc @1
        sec
@1:     jmp DrawLine
