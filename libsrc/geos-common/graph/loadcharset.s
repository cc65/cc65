;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void LoadCharSet (struct fontdesc *myFont);

            .export _LoadCharSet

            .include "jumptab.inc"
            .include "geossym.inc"

_LoadCharSet:
        sta r0L
        stx r0H
        jmp LoadCharSet
