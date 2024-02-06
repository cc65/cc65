;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void BitmapUp (struct iconpic *myGfx);

            .import BitmapRegs
            .export _BitmapUp

            .include "jumptab.inc"

_BitmapUp:
        jsr BitmapRegs
        jmp BitmapUp
