;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void DrawSprite (char spritenum, char *tab63 );

            .import popa
            .export _DrawSprite

            .include "jumptab.inc"
            .include "geossym.inc"

_DrawSprite:
        sta r4L
        stx r4H
        jsr popa
        sta r3L
        jmp DrawSprite