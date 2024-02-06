;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char GetCharWidth (char character);

            .export _GetCharWidth

            .include "jumptab.inc"

_GetCharWidth:
        jsr GetCharWidth
        ldx #0
        rts
